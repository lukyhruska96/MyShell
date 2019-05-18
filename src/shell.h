#ifndef SHELL_H
#define SHELL_H

void
sh_init();

void
sh_cleanup();

int
sh_isexit(char * command);

void
sh_exit();

char *
sh_getprompt();

#endif // SHELL_H
