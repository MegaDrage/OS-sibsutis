
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  pid_t parent_pid;
  parent_pid = fork();
  fork();
  fork();
  while (1) {
    fprintf(stdout, "PROCESS: %i\n", (int)getpid());
  }
  return 0;
}
