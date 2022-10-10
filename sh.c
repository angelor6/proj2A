/*
Angelo Ramos
Mitchell Ramos
*/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"


int pid;

int sh( int argc, char **argv, char **envp )
{
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  char **args = calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist;
  char *token;


  int len;

  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
						  out with*/
     
  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  prompt[0] = ' '; prompt[1] = '\0';

  /* Put PATH into a linked list */
  pathlist = get_path();



  
  while ( go )
  {
      // prompt user
      printf("\n$");
      if (fgets(commandline, PROMPTMAX, stdin) != NULL) {

           /* get command line and process */

          len = (int)strlen(commandline);
          commandline[len - 1] = '\0';
          token = strtok(commandline," ");

          i = 0;
          
          // split user input into seperate strings in args[]
          // command = args[0]
          // arguments = args[1...n]
          while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL," ");
          }
          printf("\nCommand: %s", args[0]);
      }

    
    int numOfCmd = 10;

    char*listOfCmds[numOfCmd];
    int switchArg = 0;
    int flag = 0;

    listOfCmds[0] = "exit";
    listOfCmds[1] = "pwd";
    listOfCmds[2] = "which";
    listOfCmds[3] = "pid";
    listOfCmds[4] = "kill";
    listOfCmds[5] = "where";
    listOfCmds[6] = "list";
    listOfCmds[7] = "setenv";
    listOfCmds[8] = "printenv";
    listOfCmds[9] = "cd";

/* set up for which
   checks if arg[1] is a built in command
   if it is then the command (arg[0]) is now arg[1]*/
if (args[1] != NULL) {
  i = 0;
  while (i < numOfCmd){
    if (strcmp(args[0], "which") == 0 && strcmp(args[1],listOfCmds[i]) == 0){
        args[0] = args[1];
        break;
    }
    i++;
  }
}

    /* set up switch with commands
      and check for built in commands
    */
    i = 0;
    if (args[0] != NULL) {
      for(i = 0; i < numOfCmd; i++) {
        if (strcmp(args[0], listOfCmds[i]) == 0) {
          switchArg = i + 1;
          break;
        }
      }
    }

    flag = 0;
    switch(switchArg) {
      case 1:
        printf("\nGOODBYE\n");
        struct pathelement* p = pathlist;
        free(pathlist->element);
        while(p){
          struct pathelement* tmp = p;
          p = p->next;
          free(tmp);
        }

        free(prompt);
        free(commandline);
        free(args);
        free(owd);
        free(pwd);
        flag = 1;        
        exit(0);      
      case 2:
        getcwd(homedir, 10);
        printf("\n%s", homedir);
        flag = 1;
        break;
      case 3:
        i = 0;
        while (args[i] != NULL){
          which(args[i], pathlist);
          i++;
        }
        flag = 1;
        break;
      case 4:
        printf("\nPID: %d", getpid());
        flag = 1;
        break;
      case 5:
        if (args[2] != NULL) {
          memmove(args[1], args[1]+1, strlen(args[1])); // removes '-' from -(number)
          kill(atoi(args[2]), atoi(args[1]));
          perror("Failed");
          break;
        }
        else if (args[1] != NULL){
          kill(atoi(args[1]), 9);
          perror("failed");
          break;
        }
        break;
      case 6:
        i = 0;
        while (args[i] != NULL) {
          where(args[i], pathlist);
          i++;
        }
        flag = 1;
        break;
      case 7:
        DIR *dir;
        struct dirent *sd;

        if (args[1] == NULL) {
          dir = opendir(".");
          list(dir);
        }
        else if (args[2] != NULL) {
          for (int j = 1; j < i; j++) {
            char* path = args[j];
            dir = opendir(path);
            printf("%s:\n", path);
            list(dir);
          }
        }

        else {
          printf("Wrong number of arguments");
        }
        flag = 1;
        break;
      case 8:
        extern char **environ;

        if (args[1] == NULL) {
          while (*environ) {
            printf("%s\n", *environ++);
          }
        }
        else if (args[2] == NULL) {
          setenv(args[1], "", 0);
        }
        else if(args[3] == NULL) {
          setenv(args[1], args[2], 1); // 3rd param is non-zero which means var will be overwritten
        }
        else {
          printf("Too many arguments");
        }
        flag = 1;
        break;
      case 9:
        if(strcmp(args[0], "printenv") == 0) {
        extern char **environ; // points to an array of strings called the environment

        if (args[1] == NULL) {
          while(*environ) {
            printf("%s\n", *environ++);
          }
        }

        if (args[2] == NULL) {
          char* var = getenv(args[1]);

          if(var) {
            printf ("Var found : %s\n", var);
          }
          else {
            printf("Var not found");
          }
        }
        
        else {
          printf("Too many arguments");
        }
        printf("\n");
      }
      flag = 1;
      break;
      case 10:
        if (args[1] == NULL) {
          chdir(getenv("HOME"));
        }
        else if (strcmp(args[1], "-") == 0) {
          chdir("..");
        }

        else if (args[3] != NULL) {
          printf("Too many arguments");
        }

        else if (args[2] == NULL) {
          int validArgInt = chdir(args[1]);  // on success, results in 0. on error, results in -1
          if (validArgInt == 0) {
            chdir(args[1]);
          }
          else {
            perror("chdir failed");
          }
        }
        flag = 1;
        break;
      default:
        break;
    }


    
    /* do fork(), execve() and waitpid() */
    pid = fork();
    if(pid == 0) {
      execve(argv[0],args,NULL);
      exit(0);
    } else {
      waitpid(pid,NULL, 0);
      printf("\nchild exit");
    }

      /* else */
      if (flag == 0){
        fprintf(stderr, "%s: Command not found.\n", args[0]);

      }

      // clear arguments
      i = 0;
      while (args[i] != NULL){
        args[i] = NULL;
          i++;
        }
    }
  return 0;
} /* sh() */

// search for argument in pathlist and print locations
char *which(char *command, struct pathelement *pathlist )
{
    int flag = 0;
    while (pathlist){
      char *ret = strstr(pathlist->element, command);
      if(ret){
        printf("\n[%s]", pathlist->element);
        flag = 1;
      }
      pathlist = pathlist->next;
    }
    if (flag == 0) {
      return NULL;
    }
   /* loop through pathlist until finding command and return it.  Return
   NULL when not found. */

} /* which() */

// search for argument in pathlist and print locations
char *where(char *command, struct pathelement *pathlist )
{
  
  while (pathlist) {
    char *ret = strstr(pathlist->element, command);
    if (ret){
       printf("\n[%s]", pathlist->element);
    }
    pathlist = pathlist->next;
  }

  /* similarly loop through finding all locations of command */
} /* where() */

void list (DIR *dir)
{
  struct dirent *sd;

  if (dir == NULL) {
    printf("Could not open current directory");
  }

  while ((sd = readdir(dir)) != NULL) {
    printf("%s\n", sd->d_name);
  }

  free(dir);
} /* list() */

