#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "shell.h"
#include "cd.h"

extern int EXIT_CODE;

char *PATH = NULL;

void
comm_handle(char *command, int argc, char *argv[]) {
    if(!comm_is_known(command, argc, argv)) {
        if(PATH == NULL) {
            PATH = getenv("PATH");
            if(PATH == NULL) {
                fprintf(stderr, "Unable to get PATH variable.\n");
                EXIT_CODE = 1;
                return;
            }
        }
        
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