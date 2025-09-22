#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT "3490"
#define BUFLEN 128

/**
 * @brief Get the IPv4 or IPv6 of an address structure as a generic pointer
 * @returns Generic pointer to either IPv4 or IPv6
 */
void *get_in_addr(struct sockaddr *sa)
{
  // NOTE: This is kind of neat design!
  // They have a generic type (sockaddr) for both v4 and v6 which is passable around stuff
  // We can morph that generic into specific type via simply checking a member var and casting accordingly
  // Good C design :)

  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
  // Resolve own host

  struct addrinfo hints;

  memset(&hints, 0, sizeof hints);

  hints.ai_family   = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM; // UDP socket!
  hints.ai_flags    = AI_PASSIVE; // Listen on all my network interfaces

  int              gai_status;
  struct addrinfo *servinfo;
  if ((gai_status = getaddrinfo(NULL, PORT, &hints, &servinfo)) == -1)
  {
    fprintf(stderr, "listener: getaddrinfo: %s\n", gai_strerror(gai_status));
    exit(EXIT_FAILURE);
  }

  // Bind to first result we can

  int              sockfd;
  struct addrinfo *curr;
  for (curr = servinfo; curr != NULL; curr = curr->ai_next)
  {
    if ((sockfd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1)
    {
      perror("listener: socket");
      continue;
    }

    if (bind(sockfd, curr->ai_addr, curr->ai_addrlen) == -1)
    {
      perror("listener: bind");
      continue;
    }

    break;
  }

  if (curr == NULL)
  {
    fprintf(stderr, "listener: failed to bind\n");
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(servinfo);

  // Resolve their host and begin receiving from it 

  printf("listener: waiting to recvfrom...\n");

  struct sockaddr_storage their_addr; // TODO: Why use this type instead of sockaddr?
  socklen_t               their_addrlen = sizeof their_addr;
  int                     nbytes;
  char                    buf[BUFLEN];

  memset(&buf, 0, sizeof buf);

  if ((nbytes = recvfrom(
           sockfd, buf, BUFLEN - 1, 0, (struct sockaddr *)&their_addr, &their_addrlen)) ==
      -1)
  {
    perror("listener: recvfrom");
    exit(EXIT_FAILURE);
  }

  char ipstr[INET6_ADDRSTRLEN];
  printf("listener: got packet from %s\n",
         inet_ntop(their_addr.ss_family,
                   get_in_addr((struct sockaddr *)&their_addr),
                   ipstr,
                   sizeof ipstr));

  printf("listener: packet is %d bytes long\n", nbytes);
  buf[nbytes] = '\0';
  printf("listener: packet contains '%s'\n", buf);

  close(sockfd);

  return 0;
}
