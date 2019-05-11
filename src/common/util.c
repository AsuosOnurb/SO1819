#include <fcntl.h>

#include "util.h"
#include "strings.h"

int file_open(fdb_t *fdbuffer, char *file, bool read, bool write) {
    int flags = O_CREAT;

    if(read == write && read == true)
        flags |= O_RDWR;
    else if(read)
        flags |= O_RDONLY;
    else if(write)
        flags |= O_WRONLY;

    if(fdb_fopen(fdbuffer, file, flags, 0644) != 0)
        return -1;

    return 0;
}

int file_close(fdb_t fdbuffer) {
    if(fdb_fclose(fdbuffer) != 0)
        return -1;

    return 0;
}
