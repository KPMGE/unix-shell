#ifndef HELPERS_H
#define HELPERS_H
#define AMOUNT_ARGS       4
#define MAX_ARG_SIZE      50
#define AMOUNT_COMMANDS   5
#define COLOR_GREEN_BOLD       "\033[1;32m"  
#define COLOR_RESET       "\033[0m"
#define COLOR_RED         "\033[0;31m"

#include <stddef.h>

void exec_commands_on_new_session(char ***commands, size_t amount_commads);

// Allocating buffer
char *** init_buffer();

// Set buffer pointers to NULL
void set_buffer(char *** buffer);

// If buffer was dinamic allocated
void end_buffer(char *** buffer);

// Reading terminal input
// Returns buffer if SUCESS or NULL if ERROR
char *** read_shell_input(char ***buffer, bool *foreground_exec, short * commands_amount);

#endif // !HELPERS_H
