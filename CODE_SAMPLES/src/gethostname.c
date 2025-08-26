#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  const size_t HOSTNAME_BUF_LEN = 64;
  char         hostname_buf[HOSTNAME_BUF_LEN];

  if (gethostname(hostname_buf, HOSTNAME_BUF_LEN) != 0)
  {
    perror("Gethostname fail");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "My hostname: %s\n", hostname_buf);
}
