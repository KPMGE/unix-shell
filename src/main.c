#include <helpers.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void signal_handler(int signum);

bool foreground_execution = false;
pid_t foreground_pid = 0;

int main() {
    set_background_groups(1);
    int commands_amount = 0;
    char ***buffer = init_buffer();

    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTSTP, signal_handler);

    while (true) {
        printf(COLOR_GREEN_BOLD "acsh > " COLOR_RESET);
        printf("%d ", get_background_groups());

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
                foreground_pid = pid;
                wait(NULL);
            } else {
                set_background_groups(get_background_groups() + 1);
                waitpid(pid, NULL, WNOHANG);
            }
        }
    }

    end_buffer(buffer);
}

void signal_handler(int signum) {
    if (foreground_execution) {
        kill(foreground_pid, SIGTERM);
        foreground_execution = false;
        printf("\n");
    } else {
        char signalChar = '?';
        if (signum == SIGINT)
            signalChar = 'C';
        if (signum == SIGQUIT)
            signalChar = '\\';
        if (signum == SIGTSTP)
            signalChar = 'Z';

        printf(COLOR_RED "ERROR: You can not terminate the program via Ctrl-%c signal.\n" COLOR_RESET, signalChar);
        printf(COLOR_GREEN_BOLD "acsh > " COLOR_RESET);
        fflush(stdout);  // force buffer
    }
}