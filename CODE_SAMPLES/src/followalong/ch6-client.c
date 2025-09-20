#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "3490"
#define MAXDATASIZE 100

/**
 * @brief Get the IPv4 or IPv6 of an address structure as a generic pointer
 */
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{

  if (argc != 2)
  {
    fprintf(stderr, "usage: client hostname\n");
    exit(EXIT_FAILURE);
  }

  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  struct addrinfo *servinfo;
  int              gai_status;
  if ((gai_status = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
    exit(EXIT_FAILURE);
  }

  int  sockfd;
  char ipstr[INET6_ADDRSTRLEN];

  // Resolve first
  struct addrinfo *p;
  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("client: socket");
      continue;
    }

    inet_ntop(
        p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof(ipstr));

    printf("client: attempting connection to %s\n", ipstr);

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      perror("clienet: connect");
      close(sockfd);
      continue;
    }

    break;
  }

  if (p == NULL)
  {
    fprintf(stderr, "client: failed to connect\n");
    exit(EXIT_FAILURE);
  }

  // Connection OK at this point

  inet_ntop(
      p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof(ipstr));

  printf("client: connected to %s\n", ipstr);

  freeaddrinfo(servinfo);

  char buf[MAXDATASIZE];
  int  nbytes;
  if ((nbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
  {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  buf[nbytes] = '\0';

  printf("client: received '%s'\n", buf);

  close(sockfd);

  exit(EXIT_SUCCESS);
}
