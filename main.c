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
    char delim[2] = " ";

    pid_t p = fork();

    if (p < 0) {
        perror("fork failed");
        exit(1);
    }

    else if (p == 0) {
        while (1) {
            printf("myshell > ");
            fgets(input, MAXLINE, stdin);

            input[strlen(input) - 1] = '\0';

            token = strtok(input, delim);

            if (!strcmp(token, "cd")) {
                token = strtok(NULL, delim);
                chdir(token);
            }

            else if (!strcmp(input, "exit"))
                break;
            
            else
                system(input);
        }
    }

    else {
        int status;
        wait(&status);
    }

    return 0;
}