/*
 * Invoca la llamada al sistema getppid().
 */
#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
  int pid = fork();

  if (pid == 0) {
    printf(1,"Child process pid: %d\n", getpid());
    printf(1,"Parent process pid: %d\n", getppid());
  } else {
    wait();
  }

  exit();
}