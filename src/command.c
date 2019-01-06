#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "command.h"
#include "shell.h"
#include "cd.h"

extern int EXIT_CODE;

void
comm_handle(char *command, int argc, char *argv[]) {
    char *full_path = NULL;
    if(!comm_is_known(command, argc, argv)) {
        char *buff_path = NULL;
        if(access(command, F_OK) == -1) {
            char *path = getenv("PATH");
            char *path_cpy = malloc(strlen(path));
            strcpy(path_cpy, path);
            path = path_cpy;
            if(path == NULL) {
                fprintf(stderr, "Unable to get PATH variable.\n");
                EXIT_CODE = 1;
                return;
            }
            buff_path = malloc(255);
            char *tok = strtok(path, ":");
            while(tok != NULL) {
                sprintf(buff_path, "%s/%s", tok, command);
                if(access(buff_path, F_OK) != -1) {
                    full_path = buff_path;
                    break;
                }
                tok = strtok(NULL, ":");
            }
        }
        else {
            full_path = command;
        }
        if(full_path == NULL) {
            fprintf(stderr, "%s: Command not found.\n", command);
            comm_cleancomm(command, argc, argv);
            return;
        }
        if(access(full_path, X_OK) == -1) {
            fprintf(stderr, "%s: Not enough permissions to execute this file.\n", command);
            free(full_path);
            comm_cleancomm(command, argc, argv);
            return;
        }
        pid_t pid;
        char **args;
        int status;
        switch(pid = fork()) {
            case -1:
                fprintf(stderr, "There were problem while forking this process.\n");
                EXIT_CODE = 1;
                break;
            case 0:
                args = malloc(sizeof(char*)*(argc+2));
                args[0] = command;
                for(int i = 0; i < argc; i++) args[i+1] = argv[i];
                args[argc+1] = "\0";
                execv(full_path, args);
                fprintf(stderr, "Command did not execute.\n");
                EXIT_CODE = 127;
                free(args);
                comm_cleancomm(command, argc, argv);
                sh_cleanup();
                sh_exit();
            default:
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) {
                    EXIT_CODE = WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    EXIT_CODE = 128 + WTERMSIG(status);
                } else {
                    fprintf(stderr, "%s: Unexpected error.\n", command);
                    EXIT_CODE = 1;
                }
        }
        if(buff_path != NULL)
            free(buff_path);
    }

    comm_cleancomm(command, argc, argv);
}

int
comm_is_known(char *command, int argc, char*argv[]) {
    if(cd_iscd(command)) {
        cd_handle(argc, argv);
        return (1);
    }
    else if(sh_isexit(command)) {
        comm_cleancomm(command, argc, argv);
        EXIT_CODE = 0;
        sh_exit();
    }
    return (0);
}

void
comm_cleancomm(char *command, int argc, char *argv[]) {
    for(int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
    free(command);
}

void
comm_cleanup() {

}