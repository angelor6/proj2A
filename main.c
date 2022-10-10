#include "sh.h"
#include <signal.h>
#include <stdio.h>
#include <termios.h>


void sig_handler(int signal); 
void disable_veof();
int main( int argc, char **argv, char **envp )
{
  /* put signal set up stuff here */
  disable_veof();
  signal(SIGINT, sig_handler);
  signal(SIGTSTP, sig_handler);
  signal(SIGTERM, sig_handler);
  
  while (1){
    return sh(argc, argv, envp);

  }

  return sh(argc, argv, envp);
}

void sig_handler(int signal)
{
  printf("\n");
  fflush(stdout);
  /* define your signal handler */
}

// handles EOF by setting it as the same as ctrlc
void disable_veof(void) {
    struct termios t;
    int r;

    r = tcgetattr(STDIN_FILENO, &t);
    if(r)
        exit(EXIT_FAILURE);
    t.c_cc[VEOF] = _POSIX_VDISABLE;
    r = tcsetattr(STDIN_FILENO, TCSANOW, &t);
    if(r)
        exit(EXIT_FAILURE);
}

