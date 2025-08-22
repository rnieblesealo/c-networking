/*
 * Example converstion betwen IPv4 string to network host order value
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  const char *ipv4_str = "192.168.10.33";
  struct sockaddr_in ipv4_addrstruct;
  int ipv4_network;

  // String literal, cast ok?
  if (inet_pton(AF_INET, (char *)ipv4_str, &ipv4_network) < 0) {
    exit(EXIT_FAILURE);
  }

  printf("NETWORK HOST ORDER = %d\n", ipv4_network);
}
