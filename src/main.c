#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "../include/helpers.h"

// Client================================//
int main(int argc, char **argv) {
  // exec_commands_on_new_session(create_test_commands(), AMOUNT_COMMANDS);
  bool foreground_execution = false;
  short commands_amount=0;
  char *** buffer = init_buffer();
  
  int a = 0;  //Testing

  while(true){
    // Initializing prompt
    printf(COLOR_GREEN_BOLD"acsh > "COLOR_RESET);
    read_shell_input(buffer, &foreground_execution, &commands_amount);

    // for(int i=0; i<commands_amount; i++){
    //   printf("command %d\n", i+1);
    //   for(int j=0; j<AMOUNT_ARGS; j++){
    //     if(!buffer[i][j]) break;
    //     printf("arg %d: %s\n", j+1, buffer[i][j]);       
    //   }
    // }

    printf("a = %d\n", a);

    // Testing
    set_buffer(buffer);
    if(a == 2){
      break;
    } 
    a++;
    printf("a = %d\n", a);
  }

  end_buffer(buffer);

  return 0;  
}

// Testing commands=============//
char ***create_test_commands() {
  char ***commands = malloc(AMOUNT_COMMANDS * sizeof(char ***)); 

  for (int i = 0; i < AMOUNT_COMMANDS; i++) {
    commands[i] = calloc(AMOUNT_ARGS, sizeof(char **)); 

    char test_command[100];
    // sprintf(test_command, "command %d ", i);
    commands[i][0] = strdup("sleep");
    commands[i][1] = strdup("60s");

    // for (int j = 1; j < AMOUNT_ARGS; j++) {
    //   char test_arg[100];
    //   sprintf(test_arg, "arg %d ", j - 1);
    //   commands[i][j] = strdup(test_arg);
    // }
  }

  return commands;
}