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

#define PORT "6060"
#define BACKLOG 10

void sigchld_handler(int s) {
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

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

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int start_server(int *sock_fd, char incoming_address[INET6_ADDRSTRLEN]) {
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

int main() {
  int sock_fd, incoming_fd;
  char incoming_address[INET6_ADDRSTRLEN];

  char locs[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  bool win = 0;
  bool turn = 0;

  char player;
  char player_move[2];
  memset(&player_move, 0, sizeof(player_move));

  char board[200] = "Type a number to choose a square\n\n"
                    " 0 | 1 | 2 \n"
                    "---|---|---\n"
                    " 3 | 4 | 5 \n"
                    "---|---|---\n"
                    " 6 | 7 | 8 \n\n";

  incoming_fd = start_server(&sock_fd, incoming_address);

  printf("server: got new connection from %s\n", incoming_address);

  while (!win) {
    player = 'x' * !turn + 'o' * turn;

    char turn_message[20];
    snprintf(turn_message, sizeof(turn_message), "Player %c move: \n", player);
    strcat(board, turn_message);
    if (send(incoming_fd, board, sizeof(board), 0) == -1) {
      perror("send");
    }

    if (recv(incoming_fd, &player_move, sizeof(player_move), 0) == -1) {
      perror("recv");
    }

    int player_move_loc = atoi(player_move);
    bool m = (locs[player_move_loc] == ' ');
    turn = ((1 - turn) * m) + (turn * (1 - m));
    locs[player_move_loc] = !m * locs[player_move_loc] + m * player;

    win = checkwin(locs, turn);

    memset(&board, 0, sizeof(board));
    print_board(locs, board, sizeof(board));
  }

  char win_message[20];
  snprintf(win_message, sizeof(win_message), "player %c wins!\n", player);
  strcat(board, win_message);

  if (send(incoming_fd, board, sizeof(board), 0) == -1) {
    perror("send");
  }

  close(incoming_fd);
  close(sock_fd);
  exit(0);

  return 0;
}
