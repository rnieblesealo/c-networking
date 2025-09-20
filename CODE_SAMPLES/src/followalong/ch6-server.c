#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "3490"
#define BACKLOG 9

/**
 * @brief This function reaps child processes that have exited without blocking to prevent zombie processes and protects the parent process' errno.
 * @note The function signature expected by sa_handler requires s (signal number) but it's unused here
 * @note SIGCHLD is a signal sent to a parent process when one of its child processes exits; the kernel delivers it
 */
void sigchld_handler(int s)
{
  (void)s; // Suppress unused var warning

  // Save errno since waitpid might overwrite it
  int saved_errno = errno;

  // Reap all child processes that have exited with no blocking
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  // Restore errno
  errno = saved_errno;
}

/**
 * @brief Get the IPv4 or IPv6 of an address structure as a generic pointer
 */
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{

  /* ========== Resolve Hostname ========================================= */

  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE; // This should all resolve to our own host

  struct addrinfo *servinfo; // Resolved self hostname
  int              rv;
  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo) != 0))
  {
    fprintf(stderr, "GAI ERROR: %s\n", gai_strerror(rv));
  }

  /* ========== Make & Bind Server Socket ================================ */

  int sockfd; // Listen socket

  // Loop through results and bind the first one that works
  struct addrinfo *p;
  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    // Try to socket
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("server: socket");
      continue;
    }

    // If OK, make reusable
    // A failure here means something went really wrong!
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }

    // Then try to bind
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  // If we went through all results...
  if (p == NULL)
  {
    fprintf(stderr, "server: failed to bind\n");
    exit(EXIT_FAILURE);
  }

  /* ========== Listening ================================================ */

  // Begin listening...
  if (listen(sockfd, BACKLOG) == -1)
  {
    fprintf(stderr, "listen\n");
    exit(EXIT_FAILURE);
  }

  // Show listening message
  char my_ipstr[INET6_ADDRSTRLEN];
  memset(my_ipstr, 0, sizeof(my_ipstr));

  inet_ntop(servinfo->ai_family,
            get_in_addr((struct sockaddr *)&servinfo->ai_addr),
            my_ipstr,
            sizeof(my_ipstr));

  printf("Listening on %s:%s\n", my_ipstr, PORT);

  // Don't need this anymore
  freeaddrinfo(servinfo);

  /* ========== Child Process Reaping / Signal Handling ================== */

  // Setup signal handler function
  struct sigaction sa;
  sa.sa_handler =
      sigchld_handler; // NOTE: sa_handler takes a function pointer with one int as an argument

  // NOTE: sa_mask = Set of signals to ignore while signal handler is executing
  // NOTE: sigemptyset = Calling causes no signals to block while sigchld_handler is running

  // This effectively empties the sa_mask, meaning no signals will block!
  sigemptyset(&sa.sa_mask);

  // NOTE: SA_RESTART = Auto-restart other syscalls interrupted by signal
  sa.sa_flags = SA_RESTART;

  // Register the signal handler
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  /* ========== Main Loop ================================================ */

  char                    ipstr[INET6_ADDRSTRLEN];
  int                     connsockfd; // Handler socket
  socklen_t               their_addrlen;
  struct sockaddr_storage their_addr;

  while (1)
  {
    // Try to accept new connection
    their_addrlen = sizeof(their_addr);
    connsockfd    = accept(sockfd, (struct sockaddr *)&their_addr, &their_addrlen);
    if (connsockfd == -1)
    {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr),
              ipstr,
              sizeof(ipstr));

    printf("server: got connection from %s\n", ipstr);

    // If child process (fork returns 0 to child)
    if (fork() == 0)
    {
      close(sockfd); // Close listening socket

      const char *response = "Response\n";
      if (send(connsockfd, response, strlen(response), 0) == -1)
      {
        perror("send");
      }

      close(connsockfd);
      exit(EXIT_SUCCESS);
    }

    close(connsockfd);
  }

  exit(EXIT_SUCCESS);
}
