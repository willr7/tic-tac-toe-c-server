#include "tic_tac_toe.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "6060"
#define BACKLOG 10

int main() {
  int sockfd;
  struct sockaddr_storage their_addr;
  struct addrinfo hints, *servinfo, *p;
  struct sigaction sa;
  socklen_t sin_size;
  char s[INET6_ADDRSTRLEN];
  char rv;
  int yes = 1;
  char response[2];

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  while (1) {
    char buffer[200];
    if (recv(sockfd, buffer, sizeof(buffer), 0) == -1) {
      perror("failed to receive message");
    }

    printf("%s", buffer);

    scanf("%s", response);

    if (send(sockfd, response, sizeof(char), 0) == -1) {
      perror("failed to send message");
    }
  }

  close(sockfd);
}
