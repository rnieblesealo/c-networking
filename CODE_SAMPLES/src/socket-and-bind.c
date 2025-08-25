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
  const char *HOSTNAME = "localhost";
  const char *PORT     = "3000"; // DNS lookup port, see /etc/services

  // Load address struct via lookup

  struct addrinfo  hints;
  struct addrinfo *res; // This will DMA, make sure to free

  memset(&hints, 0, sizeof(hints));

  hints.ai_socktype = SOCK_STREAM; // TCP socket
  hints.ai_family   = AF_INET;     // v4 or v6

  int status;
  if ((status = getaddrinfo(HOSTNAME, PORT, &hints, &res)) != 0)
  {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  // This assumes loaded address is AF_INET
  struct sockaddr_in *ip          = (struct sockaddr_in *)res->ai_addr;
  void               *ip_addr_val = &(ip->sin_addr);

  char ipstr[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, ip_addr_val, ipstr, sizeof(ipstr)) == NULL)
  {
    perror("Failure to get IPv4 string");
    exit(EXIT_FAILURE);
  }

  printf("Resolved IP %s, will use in connection...\n", ipstr);

  // Make socket

  int sockfd = socket(PF_INET, AF_INET, 0);
  if (sockfd < 0)
  {
    perror("socket() failure");
    exit(EXIT_FAILURE);
  }

  printf("Created socket with descriptor %d\n", sockfd);

  // Bind
  if (bind(sockfd, res->ai_addr, res->ai_addr->sa_len) != 0)
  {
    perror("Bind error");
    exit(EXIT_FAILURE);
  }

  printf("Bind OK!\n");
}
