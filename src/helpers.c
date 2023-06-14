#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "../include/helpers.h"

// ======================================================================= //
void exec_commands_on_new_session(char ***commands, size_t amount_commads) {
  setsid();

  for (int i = 0; i < amount_commads; i++) {
    int pid = fork();

    if (pid == -1) {
      perror("fork error: ");
      exit(-1);
    }

    if (pid == 0) {
      execvp(commands[i][0], commands[i]);
      perror("exec error: ");
      exit(-1);
    }
  }
}

// ================== //
char *** init_buffer(){
  
  // Alternative
  static char buffer[AMOUNT_COMMANDS][AMOUNT_ARGS][MAX_ARG_SIZE];


  // Dinamic allocation 
  // char *** buffer = calloc(AMOUNT_COMMANDS, sizeof(char**));
  // 
  // for(int x=0; x<AMOUNT_COMMANDS; x++){
    // buffer[x] = calloc(AMOUNT_ARGS, sizeof(char*));
    // for(int y=0; y<AMOUNT_ARGS; y++){
      // buffer[x][y] = calloc(MAX_ARG_SIZE, sizeof(char));
    // } 
  // }

  return buffer;
}

// ============================================ //
char *** read_shell_input(char ***buffer, bool *foreground_exec){
  
  // We can add a callback function here to manage data or return filled buffer!

  // Reading line
  char line[AMOUNT_ARGS * MAX_ARG_SIZE] = "";
  scanf("%[^\n]%*c", line);

  // printf("line = %s\n", line); debug

  // Setting foreground flag
  if(line[strlen(line) - 1] == '%'){
    printf("foreground\n"); //debug
    *foreground_exec = true;
  }

  // Spliting 
  // char *command = strtok(line, "<3");
  // char *arg = NULL;

  // while(command){
  //   printf("command = %s\n", command);

  //   arg = strtok(command, " ");
  //   while(arg){
  //     printf("arg = %s\n", arg);
  //     arg = strtok(NULL, " ");
  //   }

  //   command = strtok(NULL, "<3");
  // }
}
