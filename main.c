#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 100

int main(void) {
    char input[MAXLINE];
    char *token;
    char *args[MAXLINE];
    char delim[2] = " ";
    int i, len, cd;
    
    while (1) {
        pid_t p = fork();

        if (p < 0) {
            perror("fork failed");
            exit(1);
        }

        else if (p == 0) {
            token = "";

            for (i = 0; i < MAXLINE; i++)
                args[i] = NULL;

            printf("myshell > ");
            fgets(input, MAXLINE, stdin);

            len = strlen(input);

            input[len - 1] = '\0';

            token = strtok(input, delim);

            args[0] = token;

            for (i = 1; (args[i] = strtok(NULL, delim)) != NULL; i++)
                ;

            args[i] = NULL;

            if (!strcmp(token, "cd")) {
                cd = chdir(args[1]);

                if (cd < 0)
                   printf("Invalid directory\n");
            }

            if (!strcmp(input, "exit"))
                exit(1);
            
            else
                execvp(token, args);
        }

        else {
            int status;
            wait(&status);

            if (status)
                return 0;
        }
    }

    return 0;
}