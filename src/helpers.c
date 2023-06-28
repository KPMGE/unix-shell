#include <errno.h>
#include <helpers.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Prototypes========//
// Public==========//

char ***init_buffer();
void r_strip(char *string);
void end_buffer(char ***buffer);
void end_commands(char **commands);
void exec_commands_on_new_session(char ***buffer, size_t amount_commads);
char ***read_shell_input(char ***buffer, bool *foreground_exec, int *commands_amount);

// Private==============================================================//
// static char **split_commands(char *line, int *counter, char **commands);
static char *validate_line(char *line, int char_amount, bool *foreground_exec, char ***buffer);
static char **split_commands2(char *line, int *counter, char **commands);
static int split_args(char **commands, char ***buffer);

//===========================================//
void exec_command(char *command, char **args) {
    execvp(command, args);
    perror("exec error: ");
    exit(EXIT_FAILURE);
}

// ======================================================================= //
void exec_commands_on_new_session(char ***buffer, size_t amount_commads) {
    if (setsid() == -1) {
        perror("setsid error: ");
        exit(EXIT_FAILURE);
    }
    pid_t pgid = getpid();
    for (size_t i = 1; i < amount_commads; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork error: ");
            exit(-1);
        }

        if (pid == 0) {
            setpgid(0, pgid);
            exec_command(buffer[i][0], buffer[i]);
        }
    }

    exec_command(buffer[0][0], buffer[0]);
}

// ================== //
char ***init_buffer() {
    char ***buffer = calloc(AMOUNT_COMMANDS, sizeof(char **));
    for (int x = 0; x < AMOUNT_COMMANDS; x++) {
        buffer[x] = calloc(AMOUNT_ARGS, sizeof(char *));
    }

    return buffer;
}

// ============================================ //
char ***read_shell_input(char ***buffer, bool *foreground_exec, int *commands_amount) {
    size_t size = 0;
    ssize_t char_amount;
    char *line = NULL;

    char_amount = getline(&line, &size, stdin);

    // Handling exit conditions and formatting
    if (!validate_line(line, char_amount, foreground_exec, buffer))
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

//================================================================//
char **split_commands2(char *line, int *counter, char **commands) {
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

//=================================================================//
// static char **split_commands(char *line, int *counter, char **commands) {
//   bool error = false;
//   int command_counter = 0;
//   char *command = strtok(line, "<3");
//   while (command) {
//     if (command_counter >= AMOUNT_COMMANDS) {
//       printf(COLOR_RED "You can not type more than %d commands!\n" COLOR_RESET,
//              AMOUNT_COMMANDS);
//       error = true;
//       break;
//     }
//     // printf("command = %s\n", command); // debug
//     commands[command_counter] = strdup(command);
//     // printf("args[command_counter] = %s\n", commands[command_counter]); //
//     // debug
//     command = strtok(NULL, "<3");
//     command_counter++;
//   }

//   *counter = command_counter;

//   if (error)
//     return NULL;
//   return commands;
// }

//==============================================//
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

//=================================//
void end_commands(char **commands) {
    if (commands) {
        for (int x = 0; x < AMOUNT_COMMANDS; x++) {
            if (commands[x]) {
                free(commands[x]);
            }
        }
        free(commands);
    }
}

//============================//
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

//==============================//
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

//========================//
void r_strip(char *string) {
    while (string[strlen(string) - 1] == ' ' ||
           string[strlen(string) - 1] == '\n') {
        string[strlen(string) - 1] = '\0';
    }
}

//=======================================================================//
static char *validate_line(char *line, int char_amount, bool *foreground_exec, char ***buffer) {
    // If char is '\n'
    if (char_amount == 1) {
        free(line);
        return NULL;
    }

    // Removing whitespaces from right side
    r_strip(line);

    // exiting
    // if (!strcmp(line, "exit")) {
    //    free(line);
    //    end_buffer(buffer);
    //    exit(EXIT_SUCCESS);
    //}

    // Setting foreground flag
    if (line[strlen(line) - 1] == '%') {
        *foreground_exec = true;
    } else {
        *foreground_exec = false;
    }

    return line;
}

bool is_cd_function(char *str) {
    return !strcmp(str, "cd");
}

bool is_exit_function(char *str) {
    return !strcmp(str, "exit");
}