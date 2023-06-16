#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "../include/helpers.h"

// ======================================================================= //
void exec_commands_on_new_session(char ***commands, size_t amount_commads)
{
  setsid();

  for (int i = 0; i < amount_commads; i++)
  {
    int pid = fork();

    if (pid == -1)
    {
      perror("fork error: ");
      exit(-1);
    }

    if (pid == 0)
    {
      execvp(commands[i][0], commands[i]);
      perror("exec error: ");
      exit(-1);
    }
  }
}

// ================== //
char ***init_buffer()
{
  // Alternative
  // static char buffer[AMOUNT_COMMANDS][AMOUNT_ARGS][MAX_ARG_SIZE];
  //=============================================================//

  char ***buffer = calloc(AMOUNT_COMMANDS, sizeof(char **));

  for (int x = 0; x < AMOUNT_COMMANDS; x++)
  {
    buffer[x] = calloc(AMOUNT_ARGS, sizeof(char *));
    for (int y = 0; y < AMOUNT_ARGS; y++)
    {
      buffer[x][y] = calloc(MAX_ARG_SIZE, sizeof(char));
    }
  }

  return buffer;
}

// ============================================ //
char ***read_shell_input(char ***buffer, bool *foreground_exec)
{
  bool error = false;
  // Reading line
  char line[AMOUNT_ARGS * MAX_ARG_SIZE] = "";
  scanf("%[^\n]%*c", line);

  // Setting foreground flag
  if (line[strlen(line) - 1] == '%')
  {
    printf("foreground\n"); // debug
    *foreground_exec = true;
  }

  // Spliting commands
  char **commands = calloc(AMOUNT_COMMANDS, sizeof(char *));
  int command_counter = 0;
  char *command = strtok(line, "<3");
  while (command)
  {
    if(command_counter >= AMOUNT_COMMANDS){
      printf(COLOR_RED"You can not type more than %d commands!\n"COLOR_RESET, AMOUNT_COMMANDS);
      error = true;
      break;
    }
    printf("command = %s\n", command); // debug
    commands[command_counter] = strdup(command);
    printf("args[command_counter] = %s\n", commands[command_counter]); // debug
    command = strtok(NULL, "<3");
    command_counter++;
  }

  // Verifying error
  if(error) return NULL;

  // Spliting args
  char *arg = NULL;
  int args_counter;
  for(command_counter = 0; command_counter < AMOUNT_COMMANDS; command_counter++)
  {
    // break if command at this index do not exists
    if(!commands[command_counter]) break;

    // separating args
    arg = strtok(commands[command_counter], " ");
    args_counter=0;
    while (arg)
    {
      if(args_counter >= AMOUNT_ARGS){
        printf(COLOR_RED"You can not type more than %d args!\n"COLOR_RESET, AMOUNT_ARGS);
        error = true;
        break;
      }
      printf("arg = %s\n", arg);

      buffer[command_counter][args_counter] = strdup(arg);

      arg = strtok(NULL, " ");
      args_counter++;
    }
  }

  if(error) return NULL;

  return buffer;
}
