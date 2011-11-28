#ifndef BOARD_H
#define BOARD_H

#include <stddef.h>

typedef struct {
    int size;
    unsigned char *data;
} board_t;

board_t *board_create(int size);

board_t *board_clone(board_t *board);

void board_free(board_t *board);

int board_get_pos(board_t *board,
                  int x,
                  int y);

void board_set_pos(board_t *board,
                  int x,
                  int y,
                  int state);

void board_rotate(board_t *src,
                  board_t *dest,
                  int direction);

#endif
