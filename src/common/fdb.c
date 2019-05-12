#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fdb.h"

int fdb_create(fdb_t *fdbufLoc, int fd) {
    // Check parameters
    if(fdbufLoc == NULL)
        return -1;

    // Do a system call to get the block size
    struct stat sb;
    if(fd >= 0 && fstat(fd, &sb) != 0)
        return -2;
    
    size_t blockSize;
    if(fd >= 0)
        blockSize = (size_t) sb.st_blksize;
    else
        blockSize = 1; // default until we can actually read this information

    // Allocate memory for the buffer struct
    fdb_t fdbuf = (fdb_t) malloc(sizeof(struct fdb));

    if(fdbuf == NULL)
        return -3;

    // Initialize struct values
    fdbuf->fd = fd;
    fdbuf->size = blockSize;
    fdbuf->start = 0;
    fdbuf->occupation = 0;
    fdbuf->buffer = (char *) malloc(sizeof(char) * blockSize); // Attempt to allocate the actual buffer memory
    fdbuf->eof = false;
    fdbuf->path = NULL;
    fdbuf->is_fifo = false;

    // Check if allocation was successful
    if(fdbuf->buffer == NULL) {
        free(fdbuf);
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

    // We don't destroy fifos ever
    // This flag will be unset when necessary
    if(fdbuf->is_fifo)
        return 0;

    // Release buffer and struct memory
    free(fdbuf->buffer);
    free(fdbuf);

    // Success
    return 0;    
}

/**
 * @brief Attempts to fill the fdbuf->buffer with bytes from the file descriptor.<br>
 * Handles FIFO special files.
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
        return -2; // Data from last read(2) isn't exhausted yet
    
    fdbuf->start = 0;
    fdbuf->occupation = 0;

    // Before anything else, we must check if we're dealing with a FIFO
    // If we are and the FIFO is closed, open it
    if(fdbuf->is_fifo && fdbuf->fd < 0) {
        if((fdbuf->fd = open(fdbuf->path, fdbuf->flags, fdbuf->mode)) < 0) {
            perror("fdbuffer_fillbuf::open() failed");
            return -3;
        }

        // Do a system call to get the block size
        struct stat sb;
        if(fstat(fdbuf->fd, &sb) != 0)
            return -4;

        fdbuf->size = (size_t) sb.st_blksize;
        fdbuf->buffer = (char *) realloc(fdbuf->buffer, sizeof(char) * fdbuf->size);
    }

    // Actually perform the system call
    ssize_t bytesRead = read(fdbuf->fd, fdbuf->buffer, fdbuf->size);

    // Check if the system call succeeded
    if(bytesRead < 0) {
        perror("fdbuffer_fillbuf::read() failed: ");
        return -5; // Syscall failed
    }

    // Check if EOF reached, and set eof flag on struct if so
    if(bytesRead == 0) {
        if(fdbuf->is_fifo) {
            // In FIFOs, reads() don't block if there's no other end of the pipe open for writing
            // In these cases, only open() blocks until there's another end of the pipe open for writing
            // Because of this, we must close the file descriptor and reopen it
            if(close(fdbuf->fd) != 0)
                perror("fdbuffer_fillbuf::close() failed");
            fdbuf->fd = -1;

            return fdbuffer_fillbuf(fdbuf);
        }

        //bytesRead = read(fdbuf->fd, fdbuf->buffer, fdbuf->size);

        //if(bytesRead == 0)
            fdbuf->eof = true;
    }

    // Set occupation 
    fdbuf->occupation = bytesRead;

    // Success: return the number of bytes effectively read from the buffer
    return bytesRead;
}

/**
 * @brief Reads a single character from fdbuf->buffer. Does not perform checks of any kind for performance reasons.
 */
#define fdbuffer_readc_unchecked(f) (f)->buffer[(f)->start++]

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

ssize_t fdb_read(fdb_t fdbuf, void *pVoid, size_t size) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;

    if(pVoid == NULL)
        return -2;

    // Actually read the data from the file descriptor
    char *buf = (char *) pVoid; // sizeof(char) == 1
    size_t totalCapacity = size;

    while(size > 0) {
        // Check if fdbuf->buffer requires refilling
        if(fdbuf->start >= fdbuf->occupation) {
            ssize_t bytesRead = fdbuffer_fillbuf(fdbuf);

            // Check if refill was successful
            if(bytesRead == 0) {
                // Refill unsuccessful: we have reached EOF.
                // Return immediately, as there's nothing else to do
                return totalCapacity - size;
            } else if(bytesRead < 0) {
                // Refill unsuccessful: an error occurred.
                // Return -2 bytes read; the internal fdbuf->buffer has changed and cannot be reliably restored.
                // Therefore, its behaviour in future calls in undefined.
                return -2;
            }
        }

        size_t bytesAvailable = size;

        // min(size, fdbuf->occupation - fdbuf->start)
        if(size > fdbuf->occupation - fdbuf->start)
            bytesAvailable = fdbuf->occupation - fdbuf->start;

        size -= bytesAvailable;

        memcpy(buf, fdbuf->buffer + fdbuf->start, bytesAvailable);
        fdbuf->start += bytesAvailable;

        buf += bytesAvailable;
    }

    // Returns the number of bytes effectively read from the buffer
    return totalCapacity - size;
}

ssize_t fdb_readln(fdb_t fdbuf, char *buf, size_t size) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;

    if(buf == NULL)
        return -2;

    if(*buf == '\n')
        // Se o valor do buffer na posição 0 for já um '\n', o loop abaixo de leitura nunca irá começar, logo, vamos removê-lo
        *buf = '\0';

    size_t totalCapacity = size;

    --size; // Make space for the finishing '\0' character

    // As long as we want more characters, we keep reading
    while(size > 0 && *buf != '\n') {
        // Check if fdbuf->buffer requires refiling
        if(fdbuf->start >= fdbuf->occupation) {
            ssize_t bytesRead = fdbuffer_fillbuf(fdbuf);

            // Check if refill was successful
            if(bytesRead == 0) {
                // Refill unsuccessful: we have reached EOF.
                // Return buf as null, and return 0 on this function
                *buf = '\0';
                return 0;
            }
            else if(bytesRead < 0)
                // Refill unsuccessful: an error has occurred.
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
        while(bytesToRead > 0) {
            *buf = fdbuffer_readc_unchecked(fdbuf);
            bytesToRead--;

            // This condition MUST be kept inside the loop, otherwise,
            // buf++ would make it such that, next iteration, *buf != '\n',
            // and therefore, we would hit an infinite loop of read(2)s until
            // we exhaust the supplied buffer.
            if(*buf == '\n')
                break;

            buf++;
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

int fdb_write(fdb_t fdbuf, const void *buf, size_t size) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;
    
    if(buf == NULL)
        return -2;

    // Actually perform the write syscall
    ssize_t writtenBytes = write(fdbuf->fd, buf, size);

    // Check if syscall was successful
    if(writtenBytes < 0 || ((size_t) writtenBytes) != size) {
        perror("write() failed: ");
        return -4;
    }

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
    int result = fdb_write(fdbuf, out, requiredBytes + 1);

    // Free the memory used by the properly formatted string
    free(out);

    // Return the result of the writes call
    return result;
}

int fdb_fopen(fdb_t *fdbuf, const char *path, int flags, mode_t mode) {
    // Check parameters
    if(path == NULL)
        return -1;
    
    if(fdbuf == NULL)
        return -3;
    
    // Open the file descriptor
    int fd = open(path, flags, mode);

    // Check if open was successful
    if(fd == -1) {
        char buf[9999] = "open() failed";
        perror(strcat(buf, path));
        return -4;
    }
    
    // Create a new fdbuf and return it
    if(fdb_create(fdbuf, fd) != 0)
        return -5;
    (*fdbuf)->path = strcpy((char *) malloc(strlen(path) + 1), path);
    (*fdbuf)->flags = flags;
    (*fdbuf)->mode = mode;

    // Success
    return 0;
}

int fdb_fclose(fdb_t fdbuf) {
    // Check parameters
    if(fdbuf == NULL)
        return -1;

    // Veriicar se o file descriptor é válido
    if(fdbuf->fd < 0)
        return fdb_destroy(fdbuf); // O fd já está fechado, destruir o fdbuffer...
    
    // Close the file descriptor
    int res = close(fdbuf->fd);

    // Check success
    if(res == -1) {
        perror("close() failed: ");
        return -2;
    }
    
    return fdb_destroy(fdbuf);
}

int fdb_lseek(fdb_t fdbuf, off_t offset, int seekFlags) {
    // Verificar parâmetros
    if(fdbuf == NULL)
        return -1;

    // Fazer a system call
    offset = lseek(fdbuf->fd, offset, seekFlags);

    // Verificar se a system call foi bem sucedida
    if(offset == -1) {
        perror("lseek() failed: ");
        return -2;
    }

    // Invalidar o buffer atualmente lido e guardado no fdbuffer->buffer,
    // pois este corresponde a dados de uma posição diferente no ficheiro
    fdbuf->start = 0;
    fdbuf->occupation = 0;
    fdbuf->eof = false;

    // Sucesso!
    return offset;
}

int fdb_mkfifo(fdb_t *fdbufLoc, const char *path, int flags, mode_t mode) {
    // Verificar parâmetros
    if(fdbufLoc == NULL)
        return -1;

    if(path == NULL)
        return -2;

    // Criar a fifo
    if(mkfifo(path, mode) != 0)
        return -3;

    // Criar um fdb_t para a FIFO
    if(fdb_create(fdbufLoc, -1) != 0)
        return -4;
    (*fdbufLoc)->path = strcpy((char *) malloc(strlen(path) + 1), path);
    (*fdbufLoc)->flags = flags;
    (*fdbufLoc)->mode = mode;
    (*fdbufLoc)->is_fifo = true;

    // Não abrimos a FIFO porque a chamada open() poderia bloquear, e apenas queremos que bloqueie nos reads

    // Sucesso
    return 0;
}

int fdb_unlink(fdb_t fdbuf) {
    // Verificar parâmetris
    if(fdbuf == NULL)
        return -1;

    // Dar unlink da FIFO
    if(unlink(fdbuf->path) != 0)
        return -2;

    // Verificar se o file descriptor da FIFO ainda está aberto, e fechar se for o caso
    if(fdb_fclose(fdbuf) != 0)
        return -5;

    // Sucesso
    return 0;
}
