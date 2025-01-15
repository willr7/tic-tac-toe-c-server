#include <stdbool.h>
#include <stdio.h>

#ifndef TIC_TAC_TOE_H_
#define TIC_TAC_TOE_H_

void print_board(char locs[9], char *board_buffer, size_t board_buffer_size);
char checkwin(char locs[9], bool turn);

#endif // TIC_TAC_TOE_H_
