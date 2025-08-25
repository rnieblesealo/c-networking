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
  const char *HOSTNAME = "www.example.com";
  const char *PORT     = "80"; // HTTP port

  // Load up remote address
  struct addrinfo  hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family   = AF_INET;

  int status;
  if ((status = getaddrinfo(HOSTNAME, PORT, &hints, &res)) != 0)
  {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  // Make TCP socket
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("Socket failure");
    exit(EXIT_FAILURE);
  }

  // Connect to remote host
  if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0)
  {
    perror("Connect failure");
    exit(EXIT_FAILURE);
  }

  printf("Connection successful!\n");

  freeaddrinfo(res);
}
