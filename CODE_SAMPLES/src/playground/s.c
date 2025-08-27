#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  const char *SERVICE = "3000";

  struct addrinfo  hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family   = AF_INET;

  int status;
  status = getaddrinfo(NULL, SERVICE, &hints, &res);
  if (status != 0)
  {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  int mysock = socket(PF_INET, SOCK_STREAM, 0);
  if (mysock < 0)
  {
    perror("Socket error");
    exit(EXIT_FAILURE);
  }

  // allow reuse of socket
  int yes = 1;
  setsockopt(mysock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  if (bind(mysock, res->ai_addr, res->ai_addrlen) < 0)
  {
    perror("Bind error");
    exit(EXIT_FAILURE);
  }

  if (listen(mysock, 1) < 0)
  {
    perror("Listen error");
    exit(EXIT_FAILURE);
  }

  struct sockaddr their_addr;
  socklen_t       their_addrlen = sizeof(their_addr);
  int             connsock      = accept(mysock, &their_addr, &their_addrlen);
  if (connsock < 0)
  {
    perror("Accept error");
    exit(EXIT_FAILURE);
  }

  int cyes = 1;
  setsockopt(connsock, SOL_SOCKET, SO_REUSEADDR, &cyes, sizeof(cyes));

  const size_t BUFLEN = 64;
  int          open   = 1;
  while (open)
  {
    char acceptbuf[BUFLEN];
    memset(&acceptbuf, 0, BUFLEN);

    // FIXME: Why does this hang here?
    if (recv(connsock, acceptbuf, BUFLEN, 0) < 0)
    {
      perror("Recv error");
      exit(EXIT_FAILURE);
    }

    printf("%s", acceptbuf);
  }

  close(connsock);
  close(mysock);
  freeaddrinfo(res);
}
