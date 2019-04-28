#include <stdbool.h>

/**
 * @brief Executes a command.
 * 
 * Current state of compatibility: Guião 3, Exercício 7.
 * 
 * @param command The command to execute, including arguments
 * @param returnCode Where to store the return code of the program
 * 
 * @return 0 on success, or <0 on error
 */ 
int execute_command(char *command, int *returnCode);

/**
 * @brief Executes a command.
 * 
 * @param command The command to execute, including arguments
 * @param stdinfd The file descriptor to use as standard input for the new process
 * @param stdoutfd The file descriptor to use as standard output for the new process
 * @param returnCode Where to store the return code of the program
 * 
 * @return The return code of the program, or <0 if error
 */
int execute_command_with(char *command, int stdinfd, int fdstdout, int *returnCode);

/**
 * @brief Executes a command that contains multiple commands piped together.
 *
 * @param command_with_pipes The command to execute, including arguments
 *
 * @return <0 if error, 0 on success
 */
int execute_command_with_pipes(char *command_with_pipes);
