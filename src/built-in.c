#include <stdio.h>
#include <readline/history.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "built-in.h"

int detect_builtin(char *args[]) {      /* Compares the input to the available built-in commands*/
    if (!strcmp(args[0], "cd")) {
        cd(args[1]);
        
        return 1;
    }

    if (!strcmp(args[0], "history")) {
        history();
        return 1;
    }

    if (!strcmp(args[0], "export")) {
        if (args[1] != NULL)
            export_var(args[1]);
            
        return 1;
    }

    if (!strcmp(args[0], "exit")) {
        system("clear");
        exit(0);
    }

    return 0;   /* Returns 0 if no built-in command was found */
}

void cd(char *path) {
    int cd;

    if (path == NULL) {         /* Defaults path to /home if empty */
            chdir(getenv("HOME"));
            return ;            /* Leave function early */
        }

        cd = chdir(path);       /* Collects the status code and potentially changes directory */

        if (cd < 0)             /* Returns error message if an invalid path was given */
            printf("Invalid directory\n");
}

void history(void) {
    for (int i = 0; i < history_length; i++) {      /* Iterates through the history */
        history_set_pos(i);                         
        HIST_ENTRY *current = current_history();    
        printf("%4d %s\n", i, current->line);       /* Fetch and print the line at that point */
    }

    return ;
}

void export_var(char *variable) {
    putenv(variable);           /* Sets the environment variable */
    return ;
}