#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "built-in.h"

#define MAXLINE 100

void main_loop(void);
int get_tokens(char *input, char *delim, char *args[]);
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

    system("clear");                /* Clear the screen on launch */
    
    while (1) {                         /* Infinite loop */
        input = readline("myshell > ");
        add_history(input);

        len = strlen(input);

        if (input[len - 1] == '\n')
            input[len - 1] = '\0';                  /* Replace the newline with a nul terminator */

        i = get_tokens(input, " ", args);   /* Gets all tokens from input */

        if (args[0] == NULL)    /* Continue if no tokens entered */
            continue;

        if (!strcmp(args[i-1], "&") && i > 1) {      /* Sets the background flag to true and removes the '&' token */
            background_process = 1;
            i--;
        }

        args[i] = NULL;     /* Replaces whatever the failed strtok returned with NULL */

        if (!strcmp(args[0], "cd")) {   /* Checks if command is cd */
            cd(args[1]);
            
            continue;
        }

        if (!strcmp(args[0], "export")) {
            export_var(args[1]);
            continue;
        }

        if (!strcmp(args[0], "exit")) {     /* Checks if command is exit */
            system("clear");
            exit(0);
        }
        
        else 
            fork_child(args);               /* Forks the parent process to handle the command */

        if (!background_process)            /* Waits for process to finish unless it is a background process */
            wait(NULL);
        
        else {
            background_process = 0;         /* Resets the flag otherwise */
        }
    }
}

int get_tokens(char *input, char *delim, char *args[]) {
    int i;

    for (i = 0; i < MAXLINE; i++)       /* Initialises the token array with NULL */
            args[i] = NULL;

    args[0] = strtok(input, delim);     /* Gets the first token */

    for (i = 1; (args[i] = strtok(NULL, delim)) != NULL; i++) {   /* Proceeds to grab all tokens from input */
        if (args[i][0] == '$' && args[i][1] != '\0') {
            args[i]++;
            args[i] = getenv(args[i]);
        }
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