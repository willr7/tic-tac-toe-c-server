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

char checkwin(char locs[9], bool turn) {
  char winner;

  char x_won = (locs[0] == 'x') * (locs[1] == 'x') * (locs[2] == 'x') +
               (locs[3] == 'x') * (locs[4] == 'x') * (locs[5] == 'x') +
               (locs[6] == 'x') * (locs[7] == 'x') * (locs[8] == 'x') +
               // verticals
               (locs[0] == 'x') * (locs[3] == 'x') * (locs[6] == 'x') +
               (locs[1] == 'x') * (locs[4] == 'x') * (locs[7] == 'x') +
               (locs[2] == 'x') * (locs[5] == 'x') * (locs[8] == 'x') +
               // diagonals
               (locs[0] == 'x') * (locs[4] == 'x') * (locs[8] == 'x') +
               (locs[2] == 'x') * (locs[4] == 'x') * (locs[6] == 'x');

  bool o_won = (locs[0] == 'o') * (locs[1] == 'o') * (locs[2] == 'o') +
               (locs[3] == 'o') * (locs[4] == 'o') * (locs[5] == 'o') +
               (locs[6] == 'o') * (locs[7] == 'o') * (locs[8] == 'o') +
               // verticals
               (locs[0] == 'o') * (locs[3] == 'o') * (locs[6] == 'o') +
               (locs[1] == 'o') * (locs[4] == 'o') * (locs[7] == 'o') +
               (locs[2] == 'o') * (locs[5] == 'o') * (locs[8] == 'o') +
               // diagonals
               (locs[0] == 'o') * (locs[4] == 'o') * (locs[8] == 'o') +
               (locs[2] == 'o') * (locs[4] == 'o') * (locs[6] == 'o');

  winner = x_won * 'x' + o_won * 'o' + (!x_won & !o_won) * 'n';

  return winner;
}
