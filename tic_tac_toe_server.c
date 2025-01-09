#include "server.h"
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

  incoming_fd = start_server(&sock_fd, incoming_address, PORT, BACKLOG);

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
