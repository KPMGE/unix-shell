#include <helpers.h>

static char **split_commands2(char *line, int *counter, char **commands);
static int split_args(char **commands, char ***buffer);
static char *validate_line(char *line, int char_amount);
static void end_commands(char **commands);
static void r_strip(char *string);
static void signal_handler(int signum);
static void sigusr1_handler(int signum);

// Global environment variables
bool foreground_execution;
pid_t foreground_pid;
pid_t background_pid[MAX_BACKGROUND_PROCESS];
int b_size;

void initialize_unix_envs() {
    foreground_execution = false;
    foreground_pid = 0;
    b_size = 0;
}

void initialize_unix_signals() {
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGUSR1, sigusr1_handler);
}

void exec_command(char *command, char **args) {
    execvp(command, args);
    perror("exec error: ");
    exit(EXIT_FAILURE);
}

void exec_commands_on_new_session(char ***buffer, size_t amount_commads) {
    if (setsid() == -1) {
        perror("setsid error: ");
        exit(EXIT_FAILURE);
    }
    initialize_unix_signals();
    pid_t pgid = getpid();
    for (size_t i = 1; i < amount_commads; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork error: ");
            exit(-1);
        }

        if (pid == 0) {
            // set group id to parent process id (fork in main)
            setpgid(0, pgid);
            exec_command(buffer[i][0], buffer[i]);
        }
    }

    exec_command(buffer[0][0], buffer[0]);
}

char ***read_shell_input(char ***buffer, int *commands_amount) {
    size_t size = 0;
    ssize_t char_amount;
    char *line = NULL;

    char_amount = getline(&line, &size, stdin);

    // formatting
    if (!validate_line(line, char_amount))
        return NULL;

    // Spliting commands
    char **commands = calloc(AMOUNT_COMMANDS, sizeof(char *));
    if (!split_commands2(line, commands_amount, commands)) {
        end_commands(commands);
        return NULL;
    }

    // Spliting args
    if (!split_args(commands, buffer)) {
        end_commands(commands);
        return NULL;
    }

    free(line);
    end_commands(commands);

    return buffer;
}

char ***init_buffer() {
    char ***buffer = calloc(AMOUNT_COMMANDS, sizeof(char **));
    for (int x = 0; x < AMOUNT_COMMANDS; x++) {
        buffer[x] = calloc(AMOUNT_ARGS, sizeof(char *));
    }

    return buffer;
}

void set_buffer(char ***buffer) {
    if (!buffer) {
        perror("Could not set unexisting buffer!\n");
    }

    for (int x = 0; x < AMOUNT_COMMANDS; x++) {
        for (int y = 0; y < AMOUNT_ARGS; y++) {
            if (buffer[x][y]) {
                free(buffer[x][y]);
                buffer[x][y] = NULL;
            }
        }
    }
}

void end_buffer(char ***buffer) {
    if (!buffer) {
        perror("Could not set unexisting buffer!\n");
    }

    for (int x = 0; x < AMOUNT_COMMANDS; x++) {
        for (int y = 0; y < AMOUNT_ARGS; y++) {
            if (buffer[x][y]) {
                free(buffer[x][y]);
            }
        }

        if (buffer[x]) {
            free(buffer[x]);
        }
    }

    free(buffer);
}

bool is_cd_function(char *str) {
    return !strcmp(str, "cd");
}

bool is_exit_function(char *str) {
    return !strcmp(str, "exit");
}

static char **split_commands2(char *line, int *counter, char **commands) {
    char token[250];
    int k = 0, commands_count = 0;
    memset(token, 0, 250 * sizeof(char));

    for (size_t i = 0; i < strlen(line) - 1; i++) {
        if (line[i] == '<' && line[i + 1] == '3') {
            token[k] = '\0';
            commands[commands_count++] = strdup(token);

            k = 0;
            i++;
            memset(token, 0, 250 * sizeof(char));

            continue;
        }

        token[k++] = line[i];
        token[k] = line[i + 1];
    }

    token[k + 1] = '\0';
    commands[commands_count++] = strdup(token);

    if (commands_count >= AMOUNT_COMMANDS) {
        printf(COLOR_RED "You can not type more than %d commands!\n" COLOR_RESET, AMOUNT_COMMANDS);
        return NULL;
    }

    *counter = commands_count;

    return commands;
}

static int split_args(char **commands, char ***buffer) {
    bool error = false;
    char *arg = NULL;
    int args_counter;
    for (int x = 0; x < AMOUNT_COMMANDS; x++) {
        // break if command at this index do not exists
        if (!commands[x])
            break;

        // separating args
        arg = strtok(commands[x], " ");
        args_counter = 0;
        while (arg) {
            if (args_counter >= AMOUNT_ARGS) {
                printf(COLOR_RED "You can not type more than %d args!\n" COLOR_RESET,
                       AMOUNT_ARGS);
                error = true;
                break;
            }

            if (!strcmp(arg, "%")) {
                buffer[x][args_counter] = NULL;
                break;
            }

            buffer[x][args_counter] = strdup(arg);

            arg = strtok(NULL, " ");
            args_counter++;
        }
    }

    if (error)
        return 0;
    return 1;
}

static char *validate_line(char *line, int char_amount) {
    // If char is '\n'
    if (char_amount == 1) {
        free(line);
        return NULL;
    }

    // Removing whitespaces from right side
    r_strip(line);

    // Setting foreground flag
    if (line[strlen(line) - 1] == '%') {
        foreground_execution = true;
    } else {
        foreground_execution = false;
    }

    return line;
}

static void end_commands(char **commands) {
    if (commands) {
        for (int x = 0; x < AMOUNT_COMMANDS; x++) {
            if (commands[x]) {
                free(commands[x]);
            }
        }
        free(commands);
    }
}

static void r_strip(char *string) {
    while (string[strlen(string) - 1] == ' ' ||
           string[strlen(string) - 1] == '\n') {
        string[strlen(string) - 1] = '\0';
    }
}

static void signal_handler(int signum) {
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

        // for (int i = 0; i < b_size; i++) {
        // int pid_count = 0;
        //  for (pid_t pid = 1; pid <= MAX_PID; pid++) {
        //        if (getpgid(pid) == background_pid[i])
        //            pid_count++;
        //    }
        //    printf("group: %d - process: %d\n", background_pid[i], pid_count);
        //}

        printf(COLOR_RED "ERROR: You can not terminate the program via Ctrl-%c signal.\n" COLOR_RESET, signalChar);
        printf(COLOR_GREEN_BOLD "acsh > " COLOR_RESET);
        fflush(stdout);  // force buffer
    }
}

static void sigusr1_handler(int signum) {
    bool is_background = false;
    bool is_protected = false;
    pid_t pgid = getpgid(getpid());
    int pid_count = signum - signum;

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
        if (pid_count == 1)
            is_protected = true;
    }

    // if (is_background && !is_protected)
    //     killpg(pgid, SIGTERM);
}