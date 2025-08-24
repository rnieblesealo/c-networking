#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

/**
 * @brief Traverse an addrinfo's nexts printing their IPs as we go
 * Adapted from Beej's example!
 */
void PrintResolvedIPs(struct addrinfo *first)
{
  struct addrinfo *curr;
  for (curr = first; curr != NULL; curr = curr->ai_next)
  {
    void *addr;
    char *ipver;

    struct sockaddr_in  *ipv4;
    struct sockaddr_in6 *ipv6;

    // Get pointer to address, checking type
    if (curr->ai_family == AF_INET)
    {
      // Cast generic sockaddr to IPv4-specific sockaddr
      ipv4 = (struct sockaddr_in *)curr->ai_addr;

      // Get pointer to address struct
      addr = &(ipv4->sin_addr);

      // Set IP version
      ipver = "IPv4";

      // Do NTOP to get printable string
      char ipv4str[INET_ADDRSTRLEN];
      if (inet_ntop(curr->ai_family, addr, ipv4str, sizeof(ipv4str)) < 0)
      {
        fprintf(stderr, "inet_ntop error\n");
        exit(2);
      }

      printf("%s: %s\n", ipver, ipv4str);
    }
    else
    {
      ipv6  = (struct sockaddr_in6 *)curr->ai_addr;
      addr  = &(ipv6->sin6_addr);
      ipver = "IPv6";

      char ipv6str[INET6_ADDRSTRLEN];
      if (inet_ntop(curr->ai_family, addr, ipv6str, sizeof(ipv6str)) < 0)
      {
        fprintf(stderr, "inet_ntop error\n");
        exit(2);
      }

      printf("%s: %s\n", ipver, ipv6str);
    }
  }
}

/**
 * @brief Example I wrote myself
 */
void MyExample()
{
  const char *HOSTNAME = "www.apple.com";
  const char *PORT     = "domain"; // IANA name or port value

  struct addrinfo  hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints)); // Clean up struct

  // Fill in some facts we already know:

  hints.ai_family   = AF_INET6;    // Unspecified family; can be either v4 or v6
  hints.ai_socktype = SOCK_STREAM; // TCP socket

  // Resolve the hostname!
  if (getaddrinfo(HOSTNAME, PORT, &hints, &res) < 0)
  {
    fprintf(stderr, "Failure resolving hostname\n");
    exit(EXIT_FAILURE);
  }

  PrintResolvedIPs(res);

  freeaddrinfo(res);
}

/**
 * @brief Example I wrote more closely to Beej's in the book
 * Key difference is resolution of our own IP instead of an arbitrary one with AI_PASSIVE
 */
void BeejExample()
{
  const char *PORT = "3490";

  struct addrinfo  hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));

  // Set flags
  hints.ai_family   = AF_UNSPEC;   // Unspecified, can be v4 or v6
  hints.ai_socktype = SOCK_STREAM; // TCP socket
  hints.ai_flags =
      AI_PASSIVE; // Fill in my IP for me,  TODO: What does this mean? Answer: It resolves our own address! We can, alternatively to this, use NULL as first param

  // Make syscall
  int status;
  if ((status = getaddrinfo(NULL, (char *)PORT, &hints, &res)) != 0)
  {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  PrintResolvedIPs(res);

  freeaddrinfo(res);

  // TODO: If this looks up our own IP why isn't it the loopback?
  // Ans: It does give the loopback; you just handled printables incorrectly the first time
  // TODO: Why does this work offline? (I think system has a DNS table preinstalled, but not sure...)
  // Ans: It doesn't; if you try any DNS other than localhost if offline it'll fail
}

/**
 * @brief More robust example with user-defined hostname param
 */
void BeejExample2(int argc, char *argv[])
{
  // First arg in argv is program name
  // Our args start from the second one and on
  if (argc != 2)
  {
    fprintf(stderr, "usage: getaddrinfo [hostname]\n");
    exit(1);
  }

  struct addrinfo  hints;
  struct addrinfo *res;
  char             ipstr[INET6_ADDRSTRLEN];

  // Set hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_UNSPEC;   // Any IP type
  hints.ai_socktype = SOCK_STREAM; // TCP

  // Make syscall
  // argv[1] = Hostname we want to resolve
  // NULL port = assumes DNS lookup one
  int status;
  if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0)
  {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
  }

  PrintResolvedIPs(res);

  freeaddrinfo(res);
}

int main(int argc, char *argv[]) { BeejExample2(argc, argv); }
