#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "../include/helpers.h"

// Client================================//
int main() {
  bool foreground_execution = false;
  int commands_amount=0;
  char *** buffer = init_buffer();

  while(true){
    // Initializing prompt
    printf(COLOR_GREEN_BOLD"acsh > "COLOR_RESET);
    read_shell_input(buffer, &foreground_execution, &commands_amount);

    if (!foreground_execution) {
      exec_commands_on_new_session(buffer, commands_amount);
    } else {
      // NOTE: if executing on foreground, we know there's just one command
      pid_t pid = fork();

      if (pid == -1) {
        perror("fork error: ");
        exit(-1);
      }

      if (pid == 0) {
        execvp(buffer[0][0], buffer[0]);
        perror("exec error: ");
        exit(-1);
      } else {
        wait(NULL);
      }
    }
  }

  end_buffer(buffer);
}
