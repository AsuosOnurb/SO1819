#include <sys/types.h>
#include <stdbool.h>

/**
 * Defines a buffer for storing file descriptor data for reading.
 */
typedef struct fdbuffer {
    /** @brief The file descriptor this buffer reads from. */
    int fd;
    /** @brief Buffer size. */
    size_t size;
    /** @brief Buffer start position. */
    size_t start;
    /** @brief Buffer occupation. */
    size_t occupation;
    /** @brief Bytes already read from the file descriptor. */
    char *buffer;
    /** @brief True if EOF reached. */
    bool eof;
} *fdbuffer_t;

/**
 * @brief Creates a FD Buffer for reading and writing to file descriptors.
 * 
 * @param fd The file descriptor to use for buffer operations
 * @param fdbuf Where to store the buffer
 * 
 * @return 0 on success, <0 on error
 */
int fdbuffer_create(int fd, fdbuffer_t *fdbuf);

/**
 * @brief Destroys a FD Buffer.
 * 
 * @param fdbuf The buffer to destroy
 * 
 * @return 0 on success, <0 on error
 */
int fdbuffer_destroy(fdbuffer_t fdbuf);

/**
 * @brief Reads a character from the file descriptor associated with the specified buffer.
 * 
 * @param fdbuf The buffer to read from
 * 
 * @return <0 on error, or the character read from the file descriptor
 */
char fdbuffer_readc(fdbuffer_t fdbuf);

/**
 * @brief Reads a full line from the file descriptor associated with the specified buffer.
 * 
 * @param fdbuf The buffer to read from
 * @param buf The buffer where to store the line that was read
 * @param size The size of the storage buffer
 * 
 * @return <0 on error, or the number of bytes read on success
 */
ssize_t fdbuffer_readln(fdbuffer_t fdbuf, char *buf, size_t size);

/**
 * @brief Writes a string to the file descriptor associated with the specified buffer.
 * 
 * @param fdbuf The buffer to write to
 * @param buf The string to write to the file descriptor
 * 
 * @return <0 on error, 0 on success
 */
int fdbuffer_writes(fdbuffer_t fdbuf, const char *buf);

/**
 * @brief Writes a formatted string to the file descriptor associated with the specified buffer.
 *
 * @param fdbuf The buffer to write to
 * @param fmt The string to format
 * @param ... The format arguments
 * @return
 */
int fdbuffer_printf(fdbuffer_t fdbuf, const char *fmt, ...);

/**
 * @brief Opens a file and returns a fdbuffer associated with it.
 * 
 * @param path The path of the file to open
 * @param flags Bitwised-flags of how to open the file
 * @param fdbuf Where to store the fdbuffer
 * 
 * @return <0 on error, or 0 on success
 */
int fdbuffer_fopen(const char *path, int flags, fdbuffer_t *fdbuf);

/**
 * @brief Closes a file descriptor and destroys the fdbuffer associated with it.
 * 
 * @param fdbuf The buffer to close and destroy
 * 
 * @return <0 on error, 0 on success
 */
int fdbuffer_fclose(fdbuffer_t fdbuf);
