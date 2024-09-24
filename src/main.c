#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <bsd/string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "built-in.h"

#define MAXPROMPT 201 // 200 characters + nul

static void main_loop(void);
static int get_tokens(char *input, bool *reset);
static void fork_child(char *args[], bool background);
static char *args[MAXLINE];

int main(void) {
    using_history();
    main_loop();
}

void main_loop(void) {
    system("clear");
    
    while (1) {
        // Create PS: "@{login}-->{cwd} > "
        char const * login = getlogin();
        char const * cwd = getcwd(NULL, 0);
        char prompt_string[MAXLINE];
        prompt_string[0] = '\0';
        if (login && cwd) {
            prompt_string[0] = '@';
            prompt_string[1] = '\0';
            strlcat(prompt_string, login, MAXPROMPT);
            strlcat(prompt_string, "-->", MAXPROMPT);
            strlcat(prompt_string, cwd, MAXPROMPT);
        }
        free((void *) cwd);
        strlcat(prompt_string, " > ", MAXPROMPT);


        // Display the prompt and add the user's input to history
        char *input = readline(prompt_string);
        if (!input) // EOF
            exit(0);
        add_history(input);

        // TODO
        /* // strip newline from end */
        /* int len = strlen(input); */
        /* if (input[len - 1] == '\n') */
        /*     input[len - 1] = '\0'; */

        bool reset_val = false;
        int i = get_tokens(input, &reset_val);
        if (i == -1)
            continue;

        // remove '&' token for background processes
        bool background_process = false;
        if (!strcmp(args[i - 1], "&") && i > 1) {
            background_process = true;
            args[--i] = NULL;
        }
        
        if (detect_builtin((char const **) args)) {
            if (background_process)
                puts("ignoring '&' for built-ins");
        }
        else
            fork_child(args, background_process);

        // reset redirections
        if (reset_val) {
            puts("");
            freopen("/dev/tty", "r", stdin);
            freopen("/dev/tty", "w", stdout);
        }
        free(input);
    }
}

int get_tokens(char *input, bool *reset) {
    char const *delim = " ";
    memset(args, 0, sizeof args);

    // 0 - normal, 1 - char found, 2 - redirected
    int redirect_output = 0;
    int redirect_input = 0;

    args[0] = strtok(input, delim);

    int i;
    for (i = 1; (args[i] = strtok(NULL, delim)); ++i) {
        if (args[i][0] == '$' && args[i][1] != '\0') {
            args[i] = getenv(args[i] + 1);
            if (args[i])
                continue;
            printf("Variable not found\n");
            return -1;
        }

        if (!strcmp(args[i], "<")) {
            if (redirect_input)
                perror("multiple input redirections");
            else
                redirect_input = 1;
            continue;
        }

        if (!strcmp(args[i], ">")) {
            if (redirect_output)
                perror("multiple output redirections");
            else
                redirect_output = 1;
            continue;
        }

        if (redirect_input == 1) {
            redirect_input = 2;

            if (!freopen(args[i], "r", stdin)) {
                perror("freopen failed");
                return -1;
            }
        }
        else if (redirect_output == 1) {
            redirect_output = 2;

            if (!freopen(args[i], "w", stdout)) {
                perror("freopen failed");
                return -1;
            }
        }
        else continue;

        // Remove redirection tokens from args
        args[i - 1] = NULL;
        i -= 2;
    }

    if (redirect_input == 1 || redirect_output == 1) {
        puts("error: missing file name");
        return -1;
    }

    if (redirect_input || redirect_output)
        *reset = true;
    
    return i;
}

void fork_child(char *args[], bool background) {
    pid_t p = fork();
    if (p < 0)
        goto forkerror;

    if (!background) {
        if (!p)
            goto exec;
        wait(NULL);
        return;
    }

    if (!p) {
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        p = fork();
        if (p < 0)
            goto forkerror;
        if (!p)
            goto exec;
        printf("spawned process: %d\n", p);
        exit(0);
    }
    int rc;
    wait(&rc);
    if (rc)
        exit(1);
    return;

    exec:
    // Executes the command with tokens and returns an error if the call failed
    execvp(args[0], args);
    puts("execution failed: invalid command");
    exit(1);

    forkerror:
    perror("fork failed:");
    exit(1);
}
