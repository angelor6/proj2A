
#include "get_path.h"

extern int pid;
int sh( int argc, char **argv, char **envp);
char *which(char *command, struct pathelement *pathlist);
char *where(char *command, struct pathelement *pathlist);
void list ( DIR *dir );
void printenv(char **envp);

#define PROMPTMAX 32
#define MAXARGS 10
