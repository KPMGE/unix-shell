#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../include/helpers.h"

void exec_commands_on_new_session(char ***commands, size_t amount_commads) {
  setsid();

  for (int i = 0; i < amount_commads; i++) {
    pid_t pid = fork();

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
