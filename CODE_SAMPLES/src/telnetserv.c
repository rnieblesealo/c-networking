/**
 * @brief Simple server to handle Telnet messages
 * Uses IPv6! Careful...
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 1
#define RECVBUFLEN 64
#define HOSTNAME "localhost"
#define SERVNAME "3000"

int main()
{
  // Resolve IPv6 hostname
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));

  hints.ai_socktype = SOCK_STREAM; // TCP stream socket
  hints.ai_family   = AF_INET6;    // IPv6
  hints.ai_flags    = AI_PASSIVE;  // Fill in IP for me

  struct addrinfo *res;
  int              status = getaddrinfo(HOSTNAME, SERVNAME, &hints, &res);
  if (status != 0)
  {
    fprintf(stderr, "GAI ERROR: %s\n", gai_strerror(status));
    goto freeaddress;
  }

  // Create socket, make reusable
  int sockfd = socket(PF_INET6, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("INET6 SOCKET FAIL");
    goto freeaddress;
  }

  int yes = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  // Bind the socket
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0)
  {
    perror("INET6 BIND FAIL");
    goto closemysocket;
  }

  // Listen on socket
  if (listen(sockfd, BACKLOG) != 0)
  {
    perror("INET6 LISTEN FAIL");
    goto closemysocket;
  }

  // Accept the connection
  struct sockaddr theiraddr;
  socklen_t       theiraddrlen = sizeof(theiraddr);

  // Create socket for new connection; make reusable
  int connsockfd = accept(sockfd, &theiraddr, &theiraddrlen);
  if (connsockfd < 0)
  {
    perror("INET6 ACCEPT FAIL");
    goto closemysocket;
  }

  yes = 1;
  setsockopt(connsockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  // Receive loop
  int  conned = 1;
  char recvbuf[RECVBUFLEN];
  while (conned)
  {
    memset(recvbuf, 0, sizeof(recvbuf));
    int recvbytes = recv(connsockfd, recvbuf, sizeof(recvbuf), 0);

    if (recvbytes < 0)
    {
      perror("INET6 RECEIVE FAIL");
      goto closetheirsocket;
    }

    else if (recvbytes == 0)
    {
      fprintf(stdout, "Connection closed... RIP\n");
      conned = 0;
    }

    else
    {
      // Sanitize newline
      int c;
      for (c = 0; recvbuf[c] != '\n'; ++c)
        ;
      recvbuf[c - 1] = '\0'; // Replace newline for null terminator

      fprintf(stdout, "> %s\n", recvbuf);
    }
  }

closetheirsocket:
  fprintf(stdout, "Closing connection socket\n");
  close(connsockfd);
closemysocket:
  fprintf(stdout, "Closing listener socket\n");
  close(sockfd);
freeaddress:
  fprintf(stdout, "Freeing address info\n");
  freeaddrinfo(res);
  fprintf(stdout, "Cleanup OK \n");

  exit(EXIT_SUCCESS);
}
