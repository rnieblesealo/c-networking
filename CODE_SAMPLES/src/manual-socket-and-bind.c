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
  const char *HOSTNAME = "127.0.0.1";
  const char *PORT     = "3000";

  int                sockfd;
  struct sockaddr_in my_addr; // Will manually fill this in

  // Make a socket
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("socket() failure");
    exit(EXIT_FAILURE);
  }

  // Bind it to our address, MANUALLY
  // For this we will need to manually pack address
  // This is how it was done in 1987
  // D:

  my_addr.sin_family = AF_INET;
  my_addr.sin_port =
      htons(atoi(PORT)); // Convert port to number then turn to network byte order
  my_addr.sin_addr.s_addr =
      inet_addr(HOSTNAME); // Need inet_addr to convert IP string to address struct
  memset(
      my_addr.sin_zero,
      '\0',
      sizeof(
          my_addr
              .sin_zero)); // What is this doing? Answer: Padding to ensure sockaddr_in matches generic sockaddr struct; in this case, we're saying there's no padding

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0)
  {
    perror("Bind fail");
    exit(EXIT_FAILURE);
  }

  printf("Bind OK!\n");
}
