#ifndef	CD_H
#define	CD_H

int
cd_iscd(char *command);

void
cd_handle(int argc, char *argv[]);

void
cd_cleanup();

int
cd_init();

#endif // CD_H