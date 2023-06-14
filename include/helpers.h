#ifndef HELPERS_H
#define HELPERS_H
#define AMOUNT_ARGS         4
#define MAX_ARG_SIZE        50
#define AMOUNT_COMMANDS     5
#define ANSI_COLOR_RED      "\033[1;31m"  
#define ANSI_COLOR_RESET    "\033[0m"
#include <stddef.h>

void exec_commands_on_new_session(char ***commands, size_t amount_commads);

// Allocating buffer
char *** init_buffer();

// Set buffer pointers to NULL
void set_buffer(char *** buffer);

// If buffer was dinamic allocated
// void end_buffer(char *** buffer);

// Reading terminal input
char *** read_shell_input(char ***buffer, bool *foreground_exec);

#endif // !HELPERS_H
