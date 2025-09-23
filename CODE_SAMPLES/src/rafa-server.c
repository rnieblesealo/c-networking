/*
 * usage: ./rafa-server <hostname> <port>
 * inet6 tcp
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 9
#define MAXMSGLEN 1024

void *get_in_addr(struct sockaddr *addr)
{
  // If INET4
  if (addr->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)addr)->sin_addr);
  }

  // If INET6
  return &(((struct sockaddr_in6 *)addr)->sin6_addr);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    puts("usage: ./rafa-server <hostname> <port>");
    exit(EXIT_FAILURE);
  }

  const char *hostname = argv[1];
  const char *port     = argv[2];

  struct addrinfo  hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof hints);

  // hints.ai_family   = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE;

  int gai_status;
  if ((gai_status = getaddrinfo(hostname, port, &hints, &servinfo)) == -1)
  {
    fprintf(stderr, "server: getaddrinfo: %s\n", gai_strerror(gai_status));
    exit(EXIT_FAILURE);
  }

  // Bind to first OK
  struct addrinfo *curr;
  int              sockfd;
  for (curr = servinfo; curr != NULL; curr = curr->ai_next)
  {
    if ((sockfd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1)
    {
      perror("server: socket");
      continue;
    }

    // Make socket reusable
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      perror("server: setsockopt");
      exit((EXIT_FAILURE));
    }

    if (bind(sockfd, curr->ai_addr, curr->ai_addrlen) == -1)
    {
      perror("server: bind");
      continue;
    }

    break;
  }

  if (curr == NULL)
  {
    fprintf(stderr, "server: could not bind\n");
    exit(EXIT_FAILURE);
  }

  // Listen
  if (listen(sockfd, BACKLOG) == -1)
  {
    perror("server: listen");
    exit(EXIT_FAILURE);
  }

  char ipstr[INET6_ADDRSTRLEN];
  printf("Listening on %s:%s...\n",
         inet_ntop(curr->ai_family,
                   get_in_addr((struct sockaddr *)curr->ai_addr),
                   ipstr,
                   sizeof ipstr),
         port);

  // Done wit dis
  freeaddrinfo(servinfo);

  // Accept incoming connection
  struct sockaddr_storage their_addr;
  socklen_t               their_addrlen = sizeof their_addr;
  int                     connsockfd;
  if ((connsockfd = accept(sockfd, (struct sockaddr *)&their_addr, &their_addrlen)) == -1)
  {
    perror("server: accept");
    exit(EXIT_FAILURE);
  }

  // Get message!
  char msgbuf[MAXMSGLEN];
  while (1)
  {
    memset(&msgbuf, 0, sizeof msgbuf);

    int nbytes;
    if ((nbytes = recv(connsockfd, msgbuf, sizeof msgbuf - 1, 0)) == -1)
    {
      perror("server: recv");
      exit(EXIT_FAILURE);
    }

    // TODO: Figure out why tf this bullshit dont work
    // TODO: Trim newline at end

    msgbuf[nbytes] = '\0';

    printf("Client message: %s", msgbuf);
  }

  puts("Exiting...");

  close(connsockfd);
  close(sockfd);

  exit(EXIT_SUCCESS);
}
