#ifndef COMMAND_H
#define COMMAND_H

int
comm_handle_internal(char * command, int argc, char * argv[]);

void
comm_handle(char * command, int argc, char * argv[], int * pd,
    char ** redirections);

void
comm_cleancomm(char * command, int argc, char * argv[]);

void
comm_cleanup();

#endif // COMMAND_H
