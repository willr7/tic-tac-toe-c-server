#include "tic_tac_toe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int evaluate_board(char *locs, char player, int current_eval) {
  // calculate the best move given a string of locations and a player
  // this function is recursive, so start with base case
  char winner = checkwin(locs, player);
  char opponent = player ^ ('x' ^ 'o');

  int possible_moves[9];
  memset(possible_moves, 0, sizeof(possible_moves));
  int num_possible_moves;
  int best_move_eval = current_eval;

  num_possible_moves = get_possible_moves(possible_moves, locs);

  if (winner == 'x') {
    return 1;
  }
  if (winner == 'o') {
    return -1;
  }
  if (num_possible_moves == 0) {
    return 0;
  }

  char new_locs[9];

  // iterate through possible moves and find evaluations for all of the moves
  for (int i = 0; i < num_possible_moves; i++) {

    // reset new locs and copy old locs to new locs
    memset(new_locs, 0, sizeof(new_locs));
    for (int j = 0; j < 9; j++) {
      new_locs[j] = locs[j];
    }

    // try new move
    new_locs[possible_moves[i]] = player;

    // if the player is x, find the move that returns 1
    if (player == 'x') {
      // calculate the evaluation of making some possible move
      int potential_board_eval =
          evaluate_board(new_locs, opponent, current_eval);

      // if the evaluation of that move is greater than the current evaluation,
      // save the value
      if (best_move_eval < potential_board_eval) {
        best_move_eval = potential_board_eval;
      }
      // if the player is o, find the move that returns 0
    } else {
      // calculate the evaluation of making some possible move
      int potential_board_eval =
          evaluate_board(new_locs, opponent, current_eval);

      // if the evaluation of that move is greater than the current evaluation,
      // save the value
      if (best_move_eval > potential_board_eval) {
        best_move_eval = potential_board_eval;
      }
    }
  }
  return best_move_eval;
}

int find_best_move(char *locs, char player) {
  int possible_moves[9];
  memset(possible_moves, 0, sizeof(possible_moves));
  int num_possible_moves;

  num_possible_moves = get_possible_moves(possible_moves, locs);

  int best_eval;
  int new_eval;
  int best_move;
  char new_locs[9];
  char opponent = player ^ ('o' ^ 'x');

  if (player == 'x') {
    best_eval = -100;
  }
  if (player == 'o') {
    best_eval = 100;
  }

  best_move = possible_moves[0];

  // iterate through possible moves and find evaluations for all of the moves
  for (int i = 0; i < num_possible_moves; i++) {

    // reset new locs and copy old locs to new locs
    memset(new_locs, 0, sizeof(new_locs));
    for (int j = 0; j < 9; j++) {
      new_locs[j] = locs[j];
    }

    // try new move
    new_locs[possible_moves[i]] = player;
    new_eval = evaluate_board(new_locs, opponent, best_eval);
    if (player == 'x') {
      if (new_eval > best_eval) {
        best_eval = new_eval;
        best_move = possible_moves[i];
      }
    }
    if (player == 'o') {
      if (new_eval < best_eval) {
        best_eval = new_eval;
        best_move = possible_moves[i];
      }
    }
  }
  return best_move;
}

int main() {
  char locs[9] = {'x', ' ', 'o', ' ', 'x', 'o', 'o', 'o', ' '};
  char player = 'x';

  // int possible_moves[9];
  // memset(possible_moves, 0, sizeof(possible_moves));
  // int num_possible_moves;
  //
  // num_possible_moves = get_possible_moves(possible_moves, locs);
  //
  // for (int i = 0; i < 9; i++) {
  //   printf("possible move: %d\n", possible_moves[i]);
  // }

  // int winner = checkwin(locs, player);
  // printf("winner: %c\n", winner);

  char board[200];
  print_board(locs, board, sizeof(board));
  printf("%s", board);

  int board_evaluation = evaluate_board(locs, player, -1);
  printf("board evaluation: %d\n", board_evaluation);
  int best_move = find_best_move(locs, player);
  printf("best move: %d\n", best_move);
}
