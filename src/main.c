#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/helpers.h"
#include <sys/wait.h>
#include <string.h>

#define AMOUNT_COMMANDS 10
#define AMOUNT_ARGS 5

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

int main(int argc, char **argv) {
  exec_commands_on_new_session(create_test_commands(), AMOUNT_COMMANDS);
}
