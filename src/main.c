#include <helpers.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  bool foreground_execution = false;
  int commands_amount = 0;
  char ***buffer = init_buffer();

  while (true) {
    printf(COLOR_GREEN_BOLD "acsh > " COLOR_RESET);

    if (!read_shell_input(buffer, &foreground_execution, &commands_amount)) {
      continue;
    }

    if (is_cd_function(buffer[0][0])) {
      if (chdir(buffer[0][1]) != 0) {
        perror("chdir error: ");
        exit(EXIT_FAILURE);
      }

      set_buffer(buffer);
      continue;
    }

    pid_t pid = fork();

    if (pid == -1) {
      perror("fork error: ");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      if (foreground_execution) {
        exec_command(buffer[0][0], buffer[0]);
      }

      if (!foreground_execution) {
        exec_commands_on_new_session(buffer, commands_amount);
      }
    } else {
      set_buffer(buffer);

      if (foreground_execution) {
        wait(NULL);
      } else {
        waitpid(pid, NULL, WNOHANG);
      }
    }
  }

  end_buffer(buffer);
}
