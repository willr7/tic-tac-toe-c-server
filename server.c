#include "tic_tac_toe.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void bind_socket(int *sockfd, struct addrinfo *servinfo) {
  struct addrinfo *p;
  for (p = servinfo; p != NULL; p->ai_next) {
    if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_flags)) == -1) {
      perror("server: socket");
      continue;
    }

    int yes = 1;

    if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(*sockfd);
      perror("server: bind");
    }

    break;
  }

  freeaddrinfo(servinfo);

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }
}

void sigchld_handler(int s) {
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int start_server(int *sock_fd, char incoming_address[INET6_ADDRSTRLEN],
                 char PORT[], int BACKLOG) {
  struct sockaddr_storage their_addr;
  struct addrinfo hints, *servinfo;
  struct sigaction sa;
  socklen_t sin_size;
  char s[INET6_ADDRSTRLEN];
  char rv;
  int yes = 1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  bind_socket(sock_fd, servinfo);

  if (listen(*sock_fd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("listening for connections \n");

  sin_size = sizeof(their_addr);
  int incoming_fd = accept(*sock_fd, (struct sockaddr *)&their_addr, &sin_size);

  inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof(s));

  strcpy(incoming_address, s);

  if (incoming_fd == -1) {
    perror("failed to connect");
  }
  return incoming_fd;
}
