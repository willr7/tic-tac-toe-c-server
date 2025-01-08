#include "tic_tac_toe.h"
#include <stdbool.h>
#include <stdio.h>

void print_board(char locs[9], char *board_buffer, size_t board_buffer_size) {
  snprintf(board_buffer, board_buffer_size,
           " %c | %c | %c \n"
           "---|---|---\n"
           " %c | %c | %c \n"
           "---|---|---\n"
           " %c | %c | %c \n\n",
           locs[0], locs[1], locs[2], locs[3], locs[4], locs[5], locs[6],
           locs[7], locs[8]);
}

void move(char locs[9], bool *turn) {
  char player = 'x' * *turn + 'o' * !(*turn);
  printf("Turn: %c \n", player);

  int player_move;
  printf("Move: ");
  scanf("%d", &player_move);

  bool m = (locs[player_move] == ' ');
  *turn = ((1 - *turn) * m) + (*turn * (1 - m));
  locs[player_move] = !m * locs[player_move] + m * player;
}

bool checkwin(char locs[9], bool turn) {
  char player = 'x' * turn + 'o' * !(turn);
  return (
      // horizontals
      (locs[0] == player) * (locs[1] == player) * (locs[2] == player) +
      (locs[3] == player) * (locs[4] == player) * (locs[5] == player) +
      (locs[6] == player) * (locs[7] == player) * (locs[8] == player) +
      // verticals
      (locs[0] == player) * (locs[3] == player) * (locs[6] == player) +
      (locs[1] == player) * (locs[4] == player) * (locs[7] == player) +
      (locs[2] == player) * (locs[5] == player) * (locs[8] == player) +
      // diagonals
      (locs[0] == player) * (locs[4] == player) * (locs[8] == player) +
      (locs[2] == player) * (locs[4] == player) * (locs[6] == player));
}
