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
  const size_t BACKLOG_SIZE = 8;
  const char  *PORT         = "3000";

  // Resolve hostname (our own)
  struct addrinfo  hints; // IPv4 TCP address (remember will be our own hostname)
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_protocol = SOCK_STREAM;
  hints.ai_family   = AF_INET;
  hints.ai_flags    = AI_PASSIVE; // Autoresolve to our own hostname

  int status;
  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0)
  {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  // Hostname IP, port now in res (our own!)

  // Create socket
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("Failed creating socket");
    exit(EXIT_FAILURE);
  }

  // Bind socket to host
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0)
  {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  // Get IP string for printing purposes
  char                ipstr[INET_ADDRSTRLEN];
  struct sockaddr_in *ip     = (struct sockaddr_in *)res->ai_addr;
  void               *ip_val = &(ip->sin_addr);

  if (inet_ntop(AF_INET, ip_val, ipstr, sizeof(ipstr)) == NULL)
  {
    perror("ntop fail");
    exit(EXIT_FAILURE);
  }

  // Listen for connections
  if (listen(sockfd, BACKLOG_SIZE) != 0)
  {
    perror("Listen failure");
    exit(EXIT_FAILURE);
  }

  printf("Listening for connections on host %s, port %s...\n", ipstr, PORT);

  // Accept a new connection
  struct sockaddr_storage
      incoming_addr; // Will be written to here; why are we using generic instead of direct type?
  socklen_t incoming_addr_size = sizeof(
      incoming_addr); // Size of address struct; I think is passed as pointer to update based on actual size of incoming addr

  int incoming_sockfd =
      accept(sockfd, (struct sockaddr *)&incoming_addr, &incoming_addr_size);
  if (incoming_sockfd < 0)
  {
    perror("Accept failure");
    exit(EXIT_FAILURE);
  }

  puts("Connection established!");

  // Receive a message
  const size_t MSG_BUF_LEN = 64;
  char         msg_buf[MSG_BUF_LEN];
  int          received_bytes;
  if ((received_bytes = recv(incoming_sockfd, msg_buf, MSG_BUF_LEN, 0)) < 0)
  {
    perror("Receive failure");
    exit(EXIT_FAILURE);
  }

  printf("Received message %s\n", msg_buf);

  // Close client socket
  close(incoming_sockfd);

  // Refuse any more incoming conns.
  close(sockfd);

  // Free
  freeaddrinfo(res);
}
