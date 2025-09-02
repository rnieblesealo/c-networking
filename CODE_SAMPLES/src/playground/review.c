#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Handles a failure
 */
void Fail(const char *restrict message)
{
  perror(message);
  exit(EXIT_FAILURE);
}

// Do all in IPv6

void Server()
{
  // Resolve my address (IPv6 TCP listening on all interfaces)

  const char *HOST = "localhost"; // My localhost
  const char *PORT = "3000";

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET6;
  hints.ai_protocol = SOCK_STREAM;

  struct addrinfo *res;
  int              status = getaddrinfo(HOST, PORT, &hints, &res);
  if (status != 0)
  {
    // NOTE: Not using Fail() here since we're using GAI error instead of perror
    fprintf(stderr, "GAI error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in6 *my_addr    = (struct sockaddr_in6 *)res->ai_addr;
  void                *my_addr_ip = &my_addr->sin6_addr;
  char                 my_addr_ipstr[INET6_ADDRSTRLEN];
  in_port_t            my_addr_port = ntohs(my_addr->sin6_port);

  if (inet_ntop(my_addr->sin6_family, my_addr_ip, my_addr_ipstr, sizeof(my_addr_ipstr)) ==
      NULL)
  {
    Fail("Ntop error");
  }

  fprintf(stdout, "Resolved IP %s, port %d\n", my_addr_ipstr, my_addr_port);

  // Open a socket
  int sockfd = socket(PF_INET6, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    Fail("Socket error");
  }

  fprintf(stdout, "Opened socket with descriptor %d\n", sockfd);

  // Bind my address to the socket
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0)
  {
    Fail("Bind error");
  }

  fprintf(stdout, "Bind OK!\n");

  // Listen
  const size_t BACKLOG_SIZE = 1;
  if (listen(sockfd, BACKLOG_SIZE) != 0)
  {
    Fail("Listen error");
  }

  fprintf(stdout, "Listening on %s@%d...\n", my_addr_ipstr, my_addr_port);

  // Accept
  struct sockaddr their_addr;
  socklen_t       their_addrlen =
      sizeof(their_addr); // This will update to actual size so we pass as pointer
  int connsockfd = accept(sockfd, &their_addr, &their_addrlen);
  if (connsockfd < 0)
  {
    Fail("Accept error");
  }

  // Receive
  const size_t RECV_BUFLEN = 128;
  char         recv_buf[RECV_BUFLEN];

  memset(&recv_buf, 0, sizeof(recv_buf)); // Ensure buffer is clean!

  if (recv(connsockfd, recv_buf, RECV_BUFLEN, 0) < 0)
  {
    Fail("Receive error");
  }

  fprintf(stdout, "Received: %s\n", recv_buf);

  // Send back
  const char *RESPONSE = "Acknowledged!";
  fprintf(stdout, "Sending: %s\n", RESPONSE);

  if (send(connsockfd, RESPONSE, strlen(RESPONSE), 0) < 0)
  {
    Fail("Send error");
  }

  fprintf(stdout, "Sent!\n");

  // Close
  close(connsockfd);
  close(sockfd);
  freeaddrinfo(res);
}
void Client()
{
  // Resolve dest address
  // Send
  // Listen
  // Receive
  // Close
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    fprintf(stdout, "usage: /.review [server | client]\n");
    exit(EXIT_SUCCESS);
  }

  if (strcmp(argv[1], "server") == 0)
  {
    Server();
    exit(EXIT_SUCCESS);
  }
  else if (strcmp(argv[1], "client") == 0)
  {
    fprintf(stderr, "Client not implemented...\n");
    exit(EXIT_SUCCESS);
  }
  else
  {
    fprintf(stdout, "usage: /.review [server | client]\n");
    exit(EXIT_SUCCESS);
  }
}
