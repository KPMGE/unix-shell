#ifndef HELPERS_H
#define HELPERS_H

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define COLOR_GREEN_BOLD "\033[1;32m"
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[0;31m"

// Initialize environment variables
void initialize_unix_envs(pid_t pid);

// Finalize environment variables
void finalize_unix_envs();

// Initialize signal processing
void initialize_unix_signals();

// Run foreground process
void exec_command(char *command, char **args);

// Run background process
void exec_commands_on_new_session(char ***commands, size_t amount_commads);

// Register a new foreground process in environment variables
void register_foreground_process(pid_t pid);

// Register a new background process in environment variables
void register_background_process(pid_t pid);

// Allocating buffer
char ***init_buffer();

// Set buffer pointers to NULL
void set_buffer(char ***buffer);

// If buffer was dinamic allocated
void end_buffer(char ***buffer);

// Reading terminal input
// Returns buffer if SUCESS or NULL if ERROR
char ***read_shell_input(char ***buffer, int *commands_amount);

// Checks if the command is "cd"
bool is_cd_function(char *str);

// Checks if the command is "exit"
bool is_exit_function(char *str);

// Checks if the command should be run in foreground
bool is_foreground_execution();

#endif  // !HELPERS_H
