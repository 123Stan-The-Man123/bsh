#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "built-in.h"

#define MAXLINE 1000

void main_loop(void);
int get_tokens(char *input, char *delim, char *args[], int *reset);
void fork_child(char *args[]);

int main(void) {

    using_history();

    main_loop();    /* Enter the main event loop */

    return 0;
}

void main_loop(void) {
    char *input;            /* String to store input */
    char *args[MAXLINE];            /* Array of strings to store all arguments */
    int i, len;
    int background_process = 0;     /* Flag to check if the process is to be run in the background */
    char credentials[MAXLINE] = "@";
    int reset_val = 0;
    int *reset = &reset_val;

    system("clear");                /* Clear the screen on launch */
    
    while (1) {                     /* Infinite loop */

        /* Gets the username and the current working directory to display as the prompt */
        credentials[1] = '\0';
        strcat(credentials, getlogin());
        strcat(credentials, "-->");
        strcat(credentials, getcwd(NULL, 0));
        strcat(credentials, " > ");

        /* Display the prompt and add the user's input to history */
        input = readline(credentials);
        add_history(input);

        len = strlen(input);

        if (input[len - 1] == '\n')
            input[len - 1] = '\0';                  /* Replace the newline with a nul terminator */

        i = get_tokens(input, " ", args, reset);   /* Gets all tokens from input */

        if (i == -1) {      /* Continue if get_tokens failed*/
            continue;
        }

        if (args[0] == NULL)    /* Continue if no tokens entered */
            continue;

        if (!strcmp(args[i-1], "&") && i > 1) {      /* Sets the background flag to true and removes the '&' token */
            background_process = 1;
            i--;
        }

        args[i] = NULL;     /* Replaces whatever the failed strtok returned with NULL */
        
        if (detect_builtin(args))      /* Detects if the command is a built-in command */
            continue;

        else 
            fork_child(args);               /* Forks the parent process to handle the command */

        if (!background_process)            /* Waits for process to finish unless it is a background process */
            wait(NULL);
        
        else {
            background_process = 0;         /* Resets the flag otherwise */
        }

        if (*reset) {      /* Resets stdin and stdout if the flag is set */
            *reset = 0;
            printf("\n");
            freopen("/dev/tty", "r", stdin);
            freopen("/dev/tty", "w", stdout);
        }
    }

    free(input);    /* Free the input during program termination */
}

int get_tokens(char *input, char *delim, char *args[], int *reset) {
    int i;
    int redirect_output_flag = 0;
    int redirect_input_flag = 0;

    for (i = 0; i < MAXLINE; i++)       /* Initialises the token array with NULL */
            args[i] = NULL;

    args[0] = strtok(input, delim);     /* Gets the first token */

    for (i = 1; (args[i] = strtok(NULL, delim)) != NULL; i++) {   /* Proceeds to grab all tokens from input */
        if (args[i][0] == '$' && args[i][1] != '\0') {       /* Checks if the token is an environment variable */
            args[i]++;
            args[i] = getenv(args[i]);

            if (args[i] == NULL) {                     /* Returns an error if the variable is not set */
                printf("Variable not found\n");
                return -1;
            }
        }

        else if (!strcmp(args[i], "<")) {
            redirect_input_flag = 1;
            continue;
        }

        else if (!strcmp(args[i], ">")) {              /* Checks if the token is a redirection operator */
            redirect_output_flag = 1;
            continue;
        }

        if (redirect_input_flag) {
            redirect_input_flag = 0;
            *reset = 1;

            if (!fopen(args[i], "r")) {            /* Checks for file */
                printf("File not found\n");
                return -1;
            }
            
            if (freopen(args[i], "r", stdin) == NULL) {    /* Error check for freopen */
                perror("freopen failed");
                return -1;
            }

            args[i-1] = args[i] = NULL;         /* Resets part of the token array */

            i -= 2;
        }

        else if (redirect_output_flag) {        /* Checks for redirection */
            redirect_output_flag = 0;
            *reset = 1;

            if (freopen(args[i], "w", stdout) == NULL) {    /* Error check for freopen */
                perror("freopen failed");
                return -1;
            }

            args[i-1] = args[i] = NULL;         /* Resets part of the token array */

            i -= 2;
        }
    }

    if (redirect_input_flag || redirect_output_flag) {                        /* Error check for redirection */
        printf("error: missing file name\n");
        return -1;
    }
    
    return i;   /* Returns the position of the last token */
}

void fork_child(char *args[]) {
    pid_t p = fork();               /* Creates a forked process */

    if (p < 0) {                    /* Checks for a failed fork */
        perror("fork failed");
        exit(1);
    }

    else if (p == 0) 
        if (execvp(args[0], args) < 0) {                        /* Executes the command with tokens and returns an error if the call failed */
            printf("execution failed: invalid command\n");
            exit(1);
        }
}