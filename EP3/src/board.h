#ifndef BOARD_H
#define BOARD_H

#include <stddef.h>
#include <stdint.h>

typedef struct board_pos_t {
    uint8_t x, y;
} board_bishop_t;

typedef struct {
    uint8_t size;
    uint16_t max_bishops;
    uint16_t cur_bishops;

    board_bishop_t *bishops;
    unsigned char *data;
} board_t;


board_t *board_create(uint8_t size, uint16_t max_bishops);

board_t *board_clone(const board_t *board);

void board_free(board_t *board);

void board_copy_data(board_t *dest,
                     const board_t *src);

int board_get_pos(const board_t *board,
                  uint8_t x,
                  uint8_t y);

void board_set_pos(board_t *board,
                  uint8_t x,
                  uint8_t y,
                  int state);

int board_compare(const board_t *board_1, const board_t *board_2);

void board_rotate(board_t *board);

void board_flip(board_t *board);

int board_compare(const board_t *board_1, const board_t *board_2);

int board_equivalent(const board_t *board_1, const board_t *board_2);

uint64_t board_hash(board_t *board, uint64_t *hash2_out);

void board_add_bishop(board_t *board, uint8_t x, uint8_t y);

void board_pop_bishop(board_t *board);

void board_print(const board_t *board);

#endif
