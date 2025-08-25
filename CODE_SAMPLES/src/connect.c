#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief Original standalone example; the actual one works with the other "listen" target!
 */
void OriginalExample()
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

int main(int argc, char *argv[])
{
  // Define address we will connect to
  const char *HOSTNAME = "127.0.0.1";
  const char *PORT     = "3000";

  // Resolve it
  struct addrinfo  hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family   = AF_INET;
  // hints.ai_flags    = AI_PASSIVE; // TODO: Try with this instead

  int status;
  if ((status = getaddrinfo(HOSTNAME, PORT, &hints, &res)) != 0)
  {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  // Attempt to connect to it
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("Socket failure");
    exit(EXIT_FAILURE);
  }

  if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0)
  {
    perror("Connect failure");
    exit(EXIT_FAILURE);
  };

  printf("Connection successful!\n");

  // Send a mesage
  const size_t MAX_MSG_LEN = 64;
  char         msg[MAX_MSG_LEN];

  sprintf(msg, "Hello!");

  int sent_bytes;
  if ((sent_bytes = send(sockfd, msg, strlen(msg), 0)) < 0)
  {
    perror("Send failure");
    exit(EXIT_FAILURE);
  }

  printf("Sent message %s\n", msg);

  freeaddrinfo(res);
}
