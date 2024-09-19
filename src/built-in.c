#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "built-in.h"

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

void export_var(char *variable) {
    int len = strlen(variable);

    if (variable[0] == '\"' && variable[len-2] == '\"') {
        variable++;
        variable[len-2] = '\0';
    }

    putenv(variable);
    return ;
}