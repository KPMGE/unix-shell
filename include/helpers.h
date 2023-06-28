#ifndef HELPERS_H
#define HELPERS_H

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define AMOUNT_ARGS 4
#define MAX_ARG_SIZE 50
#define AMOUNT_COMMANDS 5
#define COLOR_GREEN_BOLD "\033[1;32m"
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[0;31m"
#define MAX_PID 32768
#define MAX_BACKGROUND_PROCESS 1000

// Global environment variables
extern bool foreground_execution;
extern pid_t foreground_pid;
extern pid_t background_pid[MAX_BACKGROUND_PROCESS];
extern int b_size;

// Initialize environment variables
void initialize_unix_envs();

// Initialize signal processing
void initialize_unix_signals();

// Run foreground process
void exec_command(char *command, char **args);

// Run background process
void exec_commands_on_new_session(char ***commands, size_t amount_commads);

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

#endif  // !HELPERS_H
