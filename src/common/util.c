#include <fcntl.h>

#include "util.h"
#include "strings.h"

int file_open(fdb_t *fdbuffer, char *file, int write) {
    int flags = O_CREAT;

    if(write) flags |= O_RDWR;
    else flags |= O_RDONLY;

    if(fdb_fopen(fdbuffer, file, flags, S_IWUSR) != 0)
        return -1;

    return 0;
}

int file_close(fdb_t fdbuffer) {
    if(fdb_fclose(fdbuffer) != 0)
        return -1;

    return 0;
}
