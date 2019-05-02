#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "../common/fdb.h"
#include "../common/commands.h"

#define DEBIAN_STANDARD_LINE_SIZE 128 * 1024

int main(int argc, const char *argv[]) {
    fdb_t fdbufStderr, fdbufStdout, fdbufStdin;

    int err = 0;
    if((err = fdb_create(STDERR_FILENO, &fdbufStderr)) < 0) {
        fprintf(stderr, "Error creating fdbufStderr! Error code %d\n", err);
        return -1;
    }

    if((err = fdb_create(STDOUT_FILENO, &fdbufStdout)) < 0) {
        fprintf(stderr, "ERROR CREATING fdbufStdout! ERROR CODE %d\n", err);
        return -2;
    }

    if((err = fdb_create(STDIN_FILENO, &fdbufStdin)) < 0) {
        fprintf(stderr, "Error creating fdbufStdin! Error code %d\n", err);
        return -3;
    }

    char line[DEBIAN_STANDARD_LINE_SIZE] = {0}; // Default size on Debian bash is 128KB
    while(true) {
        fdb_printf(fdbufStdout, "> ");

        if(fdb_readln(fdbufStdin, line, DEBIAN_STANDARD_LINE_SIZE) >= 0) {
            if(strcmp(line, "exit") == 0)
                break; // Check for exit command

            if(execute_command_with_pipes(line) != 0)
                fdb_printf(fdbufStdout, "Error in command: \"%s\"!\n", line);
        }
    }

    fdb_destroy(fdbufStdin);
    return 0;
}
