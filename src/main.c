#include <helpers.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PID 32768

void signal_handler(int signum);
void sigusr1_handler(int signum);

bool foreground_execution = false;
pid_t foreground_pid = 0;

pid_t background_pid[100];
int b_size = 0;

int main() {
    int commands_amount = 0;
    char ***buffer = init_buffer();

    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGUSR1, sigusr1_handler);

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
        if (is_exit_function(buffer[0][0])) {
            for (int i = 0; i < b_size; i++) {
                killpg(background_pid[i], SIGTERM);
            }
            end_buffer(buffer);
            exit(EXIT_SUCCESS);
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
                background_pid[b_size] = pid;
                b_size++;
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

        for (int i = 0; i < b_size; i++) {
            int pid_count = 0;
            for (pid_t pid = 1; pid <= MAX_PID; pid++) {
                if (getpgid(pid) == background_pid[i])
                    pid_count++;
            }
            printf("group: %d - process: %d\n", background_pid[i], pid_count);
        }

        printf(COLOR_RED "ERROR: You can not terminate the program via Ctrl-%c signal.\n" COLOR_RESET, signalChar);
        printf(COLOR_GREEN_BOLD "acsh > " COLOR_RESET);
        fflush(stdout);  // force buffer
    }
}

void sigusr1_handler(int signum) {
    bool is_background = false;
    bool is_protected = false;
    pid_t pgid = getpgid(getpid());
    int pid_count = 0;

    for (int i = 0; i < b_size; i++) {
        if (background_pid[i] == pgid)
            is_background = true;
    }

    // not very efficient approach
    if (is_background) {
        for (pid_t pid = 1; pid <= MAX_PID; pid++) {
            if (getpgid(pid) == pgid)
                pid_count++;
        }
        printf("group: %d - process: %d\n", pgid, pid_count);
        if (pid_count == 1)
            is_protected = true;
    }

    if (is_background && !is_protected)
        killpg(pgid, SIGTERM);
}