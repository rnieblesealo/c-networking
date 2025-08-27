#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
  const char *HOSTNAME = "127.0.0.1";
  const char *SERVICE  = "3000";

  struct addrinfo  hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family   = AF_INET;

  int status;
  status = getaddrinfo(HOSTNAME, SERVICE, &hints, &res);
  if (status != 0)
  {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("Socket error");
    exit(EXIT_FAILURE);
  }

  // allow reuse of socket
  int yes = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  if (connect(sock, res->ai_addr, res->ai_addrlen) != 0)
  {
    perror("Connect failure");
    exit(EXIT_FAILURE);
  }

  int connected = 1;
  while (connected)
  {
    // make buf
    const size_t BUFLEN = 64;
    char         buf[BUFLEN];
    memset(&buf, 0, BUFLEN);

    // prompt input
    fprintf(stdout, "> ");
    scanf("%s", buf);

    // exit
    if (strcmp(buf, "q") == 0)
    {
      fprintf(stdout, "Exiting...\n");
      break;
    }

    // send
    fprintf(stdout, "Sending %s...\n", buf);
    int sendres = send(sock, buf, strlen(buf), 0);
    if (sendres < 0)
    {
      perror("Send failure");
      exit(EXIT_FAILURE);
    }
    else if (sendres == 0)
    {
      fprintf(stderr, "Will not send, server connection closed\n");
      break;
    }
  }

  close(sock);
  freeaddrinfo(res);
}
