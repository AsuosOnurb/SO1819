#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fdb.h"

int fdb_create(int fd, fdb_t *fdbufLoc) {
    // Check parameters
    if(fd < 0)
        return -1;

    if(fdbufLoc == NULL)
        return -2;

    struct stat sb;
    if(fstat(fd, &sb) != 0)
        return -3;
    
    size_t blockSize = (size_t) sb.st_blksize;

    // Allocate memory for the buffer struct
    fdb_t fdbuf = (fdb_t) malloc(sizeof(struct fdb));

    // Initialize struct values
    fdbuf->fd = fd;
    fdbuf->size = blockSize;
    fdbuf->start = 0;
    fdbuf->occupation = 0;
    fdbuf->buffer = (char *) malloc(sizeof(char) * blockSize); // Attempt to allocate the actual buffer memory
    fdbuf->eof = false;

    // Check if allocation was successful
    if(fdbuf->buffer == NULL) {
        return -4;
    }

    // Success
    *fdbufLoc = fdbuf;
    return 0;
}

int fdb_destroy(fdb_t fdbuf) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;

    // Release buffer and struct memory
    free(fdbuf->buffer);
    free(fdbuf);

    // Success
    return 0;    
}

/**
 * @brief Attempts to fill the fdbuf->buffer with bytes from the file descriptor.
 * 
 * @param fdbuf The buffer to fill
 * 
 * @return <0 on error, 0 on EOF, or else the number of bytes read (max BYTES_SYSCALL_READ)
 */
ssize_t fdbuffer_fillbuf(fdb_t fdbuf) {
    // Check parameters
    //if(fdbuf == NULL) // Unnecessary: internal function that's only ever called after these checks were already done by the "main" api function
    //    return -1;
    
    // Check preconditions
    if(fdbuf->start < fdbuf->occupation)
        return -2; // Data from last read(2) isn't exausted yet
    
    fdbuf->start = 0;
    fdbuf->occupation = 0;

    // First do a system call to get the readable bytes on the file
    struct stat sb;
    if(fstat(fdbuf->fd, &sb) != 0)
        return -3;

    // Actually perform the system call
    ssize_t bytesRead = read(fdbuf->fd, fdbuf->buffer, fdbuf->size);

    // Check if the system call succeeded
    if(bytesRead < 0)
        return -4; // Syscall failed

    // Check if EOF reached, and set eof flag on struct if so
    if(bytesRead == 0) {
        //bytesRead = read(fdbuf->fd, fdbuf->buffer, fdbuf->size);

        //if(bytesRead == 0)
            fdbuf->eof = true;
    }

    // Set occupation 
    fdbuf->occupation = bytesRead;

    return bytesRead;
}

/**
 * @brief Reads a single character from fdbuf->buffer. Does not perform checks of any kind for performance reasons.
 */
#define fdbuffer_readc_unchecked(f) f->buffer[f->start++]

char fdb_readc(fdb_t fdbuf) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;
    
    // Check if fdbuf->start < fdbuf->occupation
    if(fdbuf->start >= fdbuf->occupation) {
        // Attempt to fill buffer
        ssize_t bytesRead = fdbuffer_fillbuf(fdbuf);

        // Check if fill was successful
        if(bytesRead < 0)
            return -2;

        if(bytesRead == 0)
            return '\0';
    }

    return fdbuffer_readc_unchecked(fdbuf);
}

ssize_t fdb_readln(fdb_t fdbuf, char *buf, size_t size) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;

    if(buf == NULL)
        return -2;

    size_t totalCapacity = size;

    --size; // Make space for the finishing '\0' character

    // As long as we want more characters, we keep reading
    while(size > 0 && *buf != '\n') {
        // Check if fdbuf->buffer requires refiling
        if(fdbuf->start >= fdbuf->occupation) {
            ssize_t bytesRead = fdbuffer_fillbuf(fdbuf);

            // Check if refill was successfull
            if(bytesRead == 0) {
                // Refill unsuccessful: we have reached EOF.
                // Return buf as null, and return 0 on this function
                *buf = '\0';
                return 0;
            }
            else if(bytesRead < 0)
                // Refill unsuccessful: an error has occured.
                // Return 0 bytes read; the internal fdbuf->buffer has changed and cannot be reliably restored.
                // Therefore, its behaviour in future calls in undefined.
                return -2;
        }

        // Perform a read loop
        int bytesToRead = size;

        // Essentially a min(size, fdbuf->occupation - fdbuf->start)
        if(size > fdbuf->occupation - fdbuf->start)
            bytesToRead = fdbuf->occupation - fdbuf->start;

        // Assumes we don't stop reading until we exhaust the number of available bytes
        size -= bytesToRead;
        
        // Read character by character until we find a newline, or exceed the buffer's size
        while(bytesToRead > 0 && *buf != '\n') {
            *buf = fdbuffer_readc_unchecked(fdbuf);
            buf++;
            
            bytesToRead--;
        }

        // If bytesToRead == 0, then nothing happens
        // But if a newline character was found before we exhausted the buffer,
        // then we need to restore those bytes to the count of bytes read this iteration,
        // so that we're able to determine the number of bytes read overall
        size += bytesToRead;
    }

    *buf = '\0'; // Null-terminate the string

    // Return the number of bytes effectively read from the buffer
    return totalCapacity - size;
}

int fdb_writes(fdb_t fdbuf, const char *buf) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;
    
    if(buf == NULL)
        return -2;
    
    // Get the size of the string to write
    size_t size = strlen(buf);

    // Actually perform the write syscall
    ssize_t writtenBytes = write(fdbuf->fd, buf, size);

    // Check if syscall was successful
    if(writtenBytes < 0 || ((size_t) writtenBytes) != size)
        return -4;

    // Success!
    return 0;
}

int fdb_printf(fdb_t fdbuf, const char *fmt, ...) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;

    // Initialize the list of variable arguments
    va_list argList;
    va_start(argList, fmt);

    // Compute the number of bytes of the output, properly formatted string
    size_t requiredBytes = vsnprintf(NULL, 0, fmt, argList);

    // Restart the va_list struct to the beginning
    va_end(argList);
    va_start(argList, fmt);

    // Allocate enough space for the output string
    char *out = (char *) malloc(requiredBytes + 1);

    if(out == NULL) {
        va_end(argList);
        return -2;
    }

    // Properly format the string with the list of arguments
    vsprintf(out, fmt, argList);

    // Free the resources of the variable arguments list
    va_end(argList);

    // Write the output string to the buffer
    int result = fdb_writes(fdbuf, out);

    // Free the memory used by the properly formatted string
    free(out);

    // Return the result of the writes call
    return result;
}

int fdb_fopen(const char *path, int flags, fdb_t *fdbuf) {
    // Check parameters
    if(path == NULL)
        return -1;
    
    if(fdbuf == NULL)
        return -3;
    
    // Open the file descriptor
    int fd = open(path, flags);

    // Check if open was successful
    if(fd == -1)
        return -4;
    
    // Create a new fdbuf and return it
    return fdb_create(fd, fdbuf);
}

int fdb_fclose(fdb_t fdbuf) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;
    
    // Close the file descriptor
    int res = close(fdbuf->fd);

    // Check success
    if(res == -1)
        return -2;
    
    return 0;
}
