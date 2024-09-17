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
    int i, len, cd, status;
    
    while (1) {
        token = "";

        for (i = 0; i < MAXLINE; i++)
            args[i] = NULL;

        printf("myshell > ");
        fflush(stdout);
        fgets(input, MAXLINE, stdin);

        len = strlen(input);

        input[len - 1] = '\0';

        token = strtok(input, delim);

        if (token == NULL)
            continue;

        args[0] = token;

        for (i = 1; (args[i] = strtok(NULL, delim)) != NULL; i++)
            ;

        args[i] = NULL;

        if (!strcmp(token, "cd")) {
            if (args[1] == NULL) {
                chdir("/home");
                continue;
            }
            cd = chdir(args[1]);

            if (cd < 0)
                printf("Invalid directory\n");
            
            continue;
        }

        if (!strcmp(token, "exit")) {
            system("clear");
            exit(0);
        }
        
        else {
            pid_t p = fork();

            if (p < 0) {
                perror("fork failed");
                exit(1);
            }

            else if (p == 0) 
                 if (execvp(token, args) < 0) {
                    printf("execution failed: invalid command\n");
                    exit(1);
                }
        }

        wait(&status);
    }

    return 0;
}