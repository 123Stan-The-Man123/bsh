#include <stdio.h>
#include <readline/history.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "built-in.h"

static void cd(char const *path);
static void history(void);
static void export_var(char const *variable);

bool detect_builtin(char const *args[static 1]) {
    if (!strcmp(args[0], "cd")) {
        if (args[1] && args[2])
            puts("cd: too many arguments");
        else
            cd(args[1]);
        return true;
    }

    if (!strcmp(args[0], "history")) {
        if (args[1])
            puts("history: too many arguments");
        else
            history();
        return true;
    }

    if (!strcmp(args[0], "export")) {
        if (!args[1])
            puts("export: missing argument");
        for (int i = 1; args[i]; ++i)
            export_var(args[i]);
        return true;
    }

    if (!strcmp(args[0], "exit")) {
        if (!args[1]) {
            system("clear");
            exit(0);
        }
        return true;
    }

    return false;
}

void cd(char const *path) {
    static char temp[MAXLINE];
    static char prev_path[MAXLINE];

    if (!path)
        path = getenv("HOME");
    if (strcmp("~", path) == 0)
        path = getenv("HOME");
    if (strcmp("-", path) == 0) {
        strcpy(temp, prev_path);
        strcpy(prev_path, getcwd(NULL, 0));
        if (!chdir(temp)) {
            printf("%s\n", temp);
            return ;
        }
    }

    strcpy(prev_path, getcwd(NULL, 0));
    
    if (!chdir(path))
        return;
    switch (errno) {
        case EACCES:
            puts("cd: permission denied");
            break;
        case EIO:
            puts("cd: IO error");
            break;
        case ELOOP:
            puts("cd: looping symbolic links");
            break;
        case ENAMETOOLONG:
        case ENOENT:
        case ENOTDIR:
            puts("cd: invalid directory");
    }
}

void history(void) {
    for (int i = 0; i < history_length; i++) {
        history_set_pos(i);
        HIST_ENTRY *current = current_history();
        printf("%4d %s\n", i, current->line);
    }
}

void export_var(char const *variable) {
    char * p = strdup(variable);
    if (putenv(p)) {
        free(p);
        if (errno == ENOMEM)
            exit(1);
        puts("export: missing '='"); // errno == EINVAL
    }
}
