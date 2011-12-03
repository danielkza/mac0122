#ifndef BOARD_H
#define BOARD_H

#include <stddef.h>

typedef struct {
    int size;
    unsigned char *data;
} board_t;

typedef unsigned char board_coord_t;

board_t *board_create(int size);

board_t *board_clone(board_t *board);

void board_free(board_t *board);

int board_get_pos(board_t *board,
                  board_coord_t x,
                  board_coord_t y);

void board_set_pos(board_t *board,
                  board_coord_t x,
                  board_coord_t y,
                  int state);

void board_rotate(board_t *src,
                  board_t *dest);


void board_flip(board_t *src,
                board_t *dest)

#endif
