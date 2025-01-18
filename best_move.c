#include <stdio.h>
#include <stdlib.h>

int get_possible_moves(int *possible_moves, char *locs) {
  // changes possible moves in place and returns the number of possible moves
  int curr = 0;
  for (int i = 0; i < 9; i++) {
    if (locs[i] == ' ') {
      possible_moves[curr] = i;
      curr += 1;
    }
  }

  return curr;
}

void get_best_move(int *best_moves, char *locs, char player) {
  // calculate the best move given a string of locations and a player
  int possible_moves[9];
  int num_possible_moves;

  num_possible_moves = get_possible_moves(possible_moves, locs);

  for (int i = 0; i < num_possible_moves; i++) {
    printf("%d\n", possible_moves[i]);
  }
}

int main() {
  char locs[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  char player = 'x';
  int best_moves[9];
  get_best_move(best_moves, locs, player);
}
