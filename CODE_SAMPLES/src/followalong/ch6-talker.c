#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT "3490"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "usage: talker hostname message\n");
    exit(EXIT_FAILURE);
  }

  struct addrinfo hints;

  memset(&hints, 0, sizeof hints);

  hints.ai_family   = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;

  struct addrinfo *servinfo;
  int              gai_status = getaddrinfo(argv[1], PORT, &hints, &servinfo);
  if (gai_status != 0)
  {
    fprintf(stderr, "talker: getaddrinfo: %s\n", gai_strerror(gai_status));
    exit(EXIT_FAILURE);
  }

  // Make a socket with first OK result

  struct addrinfo *curr;
  int              sockfd;
  for (curr = servinfo; curr != NULL; curr = curr->ai_next)
  {
    if ((sockfd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1)
    {
      perror("talker: socket");
      continue;
    }

    break;
  }

  if (curr == NULL)
  {
    fprintf(stderr, "talker: could not socket\n");
    exit(EXIT_FAILURE);
  }

  // Send the message
  int nbytes;
  if ((nbytes = sendto(
           sockfd, argv[2], strlen(argv[2]), 0, curr->ai_addr, curr->ai_addrlen)) == -1)
  {
    perror("talker: sendto");
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(servinfo);

  printf("talker: sent %d bytes to %s\n", nbytes, argv[1]);

  close(sockfd);
}
