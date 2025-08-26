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
  // Establish a connection to an HTTP server
  const char *HOST    = "www.ucf.edu";
  const char *SERVICE = "http";

  struct addrinfo  hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family   = AF_INET;

  int status = getaddrinfo(HOST, SERVICE, &hints, &res);
  if (status != 0)
  {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("Socket fail");
    exit(EXIT_FAILURE);
  }

  if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0)
  {
    perror("Connect fail");
    exit(EXIT_FAILURE);
  }

  puts("Connection OK");

  // Call getpeername on it
  struct sockaddr peer_addr;
  socklen_t       peer_addrlen = sizeof(peer_addr);

  if (getpeername(sockfd, &peer_addr, &peer_addrlen) != 0)
  {
    perror("Getpeername fail");
    exit(EXIT_FAILURE);
  }

  // Show IP and port
  struct sockaddr_in *peer_addr_in    = (struct sockaddr_in *)&peer_addr;
  void               *peer_addr_in_ip = &(peer_addr_in->sin_addr);

  char peer_ipstr[INET_ADDRSTRLEN];
  if (inet_ntop(
          peer_addr_in->sin_family, peer_addr_in_ip, peer_ipstr, sizeof(peer_ipstr)) ==
      NULL)
  {
    perror("Ntop error");
    exit(EXIT_FAILURE);
  }

  int peer_port = ntohs(peer_addr_in->sin_port);

  fprintf(stdout, "Peer IPv4: %s\n", peer_ipstr);
  fprintf(stdout, "Peer port: %d\n", peer_port);

  // Close
  close(sockfd);
  freeaddrinfo(res);
}
