#include <helpers.h>

int main() {
    initialize_unix_envs(getpid());
    initialize_unix_signals();
    int commands_amount = 0;
    char ***buffer = init_buffer();

    while (true) {
        printf(COLOR_GREEN_BOLD "acsh > " COLOR_RESET);

        if (!read_shell_input(buffer, &commands_amount)) {
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
            finalize_unix_envs();
            end_buffer(buffer);
            exit(EXIT_SUCCESS);
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork error: ");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            if (is_foreground_execution()) {
                exec_command(buffer[0][0], buffer[0]);
            }

            else {
                exec_commands_on_new_session(buffer, commands_amount);
            }
        } else {
            set_buffer(buffer);
            if (is_foreground_execution()) {
                register_foreground_process(pid);
                wait(NULL);
            } else {
                register_background_process(pid);
                waitpid(pid, NULL, WNOHANG);
            }
        }
    }
    exit(EXIT_FAILURE);
}