#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strings.h"
#include "commands.h"

int execute_command(char *command, int *returnCode) {
    return execute_command_with(command, STDIN_FILENO, STDOUT_FILENO, returnCode);
}

int execute_command_with(char *command, int stdinfd, int stdoutfd, int *returnCode) {
    // Check parameters
    if(command == NULL)
        return -1;

    // Parse argv
    char **argv;
    str_split(command, ' ', &argv);

    // Create a child process to execute the command on
    pid_t childPid = fork();

    if(childPid == 0) {
        // In the child

        // Redirect the standard input and output descriptors to the ones provided
        dup2(stdinfd, STDIN_FILENO);
        dup2(stdoutfd, STDOUT_FILENO);

        // Execute the command
        execvp(argv[0], argv);
        
        // Exit if we failed to execute the command
        _exit(-1);
    } else {
        // In the parent

        // Wait for the child to terminate, if required
        int status = 0;

        waitpid(childPid, &status, 0);
        
        // Get return code from child
        if(WIFEXITED(status)) {
            *returnCode = WEXITSTATUS(status);
            status = 0;
        } else {
            status = -2;
        }

        // Free the argument array
        free_all(argv);

        // Return the exit code of the child
        return status;
    }
}

int execute_command_with_pipes(char *command_with_pipes) {
    size_t len = strlen(command_with_pipes);

    if(len <= 0)
        return -1;

    bool isWorker = false;
    if(command_with_pipes[len] == '&') {
        command_with_pipes[len] = '\0';
        len--;

        // Execute the command in a subprocess
        int childPid = fork();
        if(childPid != 0)
            return childPid; // The parent returns immediately; only the child keeps executing this function
        else isWorker = true;
    }

    char **commands;
    int argc = str_split(command_with_pipes, '|', &commands);

    int stdinFd = STDIN_FILENO;

    int i = 0;
    while(i < argc) {
        char *command = commands[i];

        int pd[2];

        if(i + 1 != argc) {
            if(pipe(pd) != 0)
                return -2;
        } else {
            pd[0] = STDIN_FILENO;
            pd[1] = STDOUT_FILENO;
        }

        int subReturnCode = 0;
        if(execute_command_with(command, stdinFd, pd[1], &subReturnCode) != 0 && subReturnCode != 0) {
            if(stdinFd != STDIN_FILENO)
                if(close(stdinFd) != 0)
                    return -4;

            if(pd[1] != STDOUT_FILENO)
                if(close(pd[1]) != 0)
                    return -5;

            return -3;
        }
        // Agora a partir do pd[0] podemos ler a output do comando executado
        // Logo vamos fazer do stdinFd = pd[0]
        // Antes disso, vamos fechar a pipe de leitura que foi usada para este processo,
        // pois já não é necessária, excepto se esta for o stdin da própria bash
        if(stdinFd != STDIN_FILENO)
            if(close(stdinFd) != 0)
                return -4;
        stdinFd = pd[0];

        // Agora vamos fechar a pipe de escrita,
        // excepto se esta for o stdout da própria bash
        if(pd[1] != STDOUT_FILENO)
            if(close(pd[1]) != 0)
                return -5;

        i++;
    }

    // If we're a subprocess that executed in parallel to our parent,
    // then we kill ourselves as soon as our work is done
    if(isWorker)
        _exit(0);

    return 0;
}
