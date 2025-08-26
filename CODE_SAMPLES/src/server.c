/**
 * @brief Basic stream (TCP) socket server
 */

#define PORT "3490" // Where users will reach us
#define BACKLOG 10  // Conn. queue length

#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>

void sigchild_handler(int s)
{
  int saved_errno =
      errno; // waitpid() might override real errno; we cache the real one for later restore

  while (waitpid(-1, NULL, WNOHANG) > 0)
  {
    errno = saved_errno;
  }

  // TODO: Resume here...
}
