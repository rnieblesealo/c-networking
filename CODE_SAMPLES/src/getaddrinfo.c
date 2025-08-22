#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_P_ADDR_SIZE 64 // Buffer size to store printable address be it v4 or v6

int main()
{

  const char *HOSTNAME = "www.example.com";
  const char *PORT     = "3490";

  struct addrinfo  hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints)); // Clean up struct

  // Fill in some facts we already know:

  hints.ai_family   = AF_UNSPEC;   // Unspecified family; can be either v4 or v6
  hints.ai_socktype = SOCK_STREAM; // TCP socket

  // Resolve the hostname!
  if (getaddrinfo(HOSTNAME, PORT, &hints, &res) < 0)
  {
    fprintf(stderr, "Failure resolving hostname\n");
    exit(EXIT_FAILURE);
  }

  // Print the first result's IPv4
  char res_ip4[MAX_P_ADDR_SIZE];
  if (inet_ntop(
          res->ai_addr->sa_family, res->ai_addr->sa_data, res_ip4, res->ai_addr->sa_len) <
      0)
  {
    fprintf(stderr, "Failure converting resolved IPv4 to printable\n");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "First resolved IPv4: %s", res_ip4);

  exit(EXIT_SUCCESS);
}
