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

int main(int argc, char *argv[]) {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  struct sigaction sa;
  socklen_t sin_size;
  char s[INET6_ADDRSTRLEN];
  char rv;
  int yes = 1;
  char player_move[2];
  char win = 0;
  char turn = 'x';
  char player;
  char turn_message[20];
  char opponent_move[5];
  char locs[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  int player_move_loc;
  char player_response[2];

  char *server_address;
  printf("Connecting to server at: %s:%s\n", argv[1], PORT);

  memset(&player_move, 0, sizeof(player_move));
  char board[200] = "Type a number to choose a square\n\n"
                    " 0 | 1 | 2 \n"
                    "---|---|---\n"
                    " 3 | 4 | 5 \n"
                    "---|---|---\n"
                    " 6 | 7 | 8 \n\n";

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
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

  if (recv(sockfd, &player, sizeof(player), 0) == -1) {
    perror("failed to receive player");
  }

  printf("Waiting for another player...\n");
  char new_player_message[50];

  if (recv(sockfd, new_player_message, sizeof(new_player_message), 0) == -1) {
    perror("failed to receive new player message");
  }

  printf("%s", new_player_message);

  snprintf(turn_message, sizeof(turn_message), "player: %c\n", player);
  strcat(board, turn_message);

  while (!win) {
    printf("%s", board);

    if (turn == player) {

      printf("Player %c move: \n", player);
      scanf("%s", player_move);

      player_move_loc = atoi(player_move);
      bool m = (locs[player_move_loc] == ' ');
      locs[player_move_loc] = !m * locs[player_move_loc] + m * player;

      if (send(sockfd, locs, sizeof(locs), 0) == -1) {
        perror("send locs in game loop");
      }
    } else {
      printf("waiting for opponent move...\n");
      if (recv(sockfd, &locs, sizeof(locs), 0) == -1) {
        perror("error receiving new turn");
      }
      printf("opponent move: \n");
    }
    win = checkwin(locs, turn);
    turn = turn ^ ('x' ^ 'o');
    memset(&board, 0, sizeof(board));
    print_board(locs, board, sizeof(board));
  }

  char win_message[20];
  snprintf(win_message, sizeof(win_message), "player %c wins!\n", win);
  strcat(board, win_message);
  printf("%s", board);

  close(sockfd);
  return 0;
}
