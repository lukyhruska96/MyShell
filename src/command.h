#ifndef COMMAND_H
#define COMMAND_H

int
comm_is_known(char * command, int argc, char * argv[]);

void
comm_handle(char * command, int argc, char * argv[], int * pd,
    char ** redirections);

void
comm_cleancomm(char * command, int argc, char * argv[]);

void
comm_cleanup();

#endif // COMMAND_H
