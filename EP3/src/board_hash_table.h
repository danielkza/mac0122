#ifndef BOARD_HASH_TABLE_H
#define BOARD_HASH_TABLE_H

#include <stdint.h>

#include "board.h"

typedef struct board_list_t {
    board_t *item;
    uint64_t hash, hash2;
    struct board_list_t *next;
} board_list_t;

typedef struct  {
    uint32_t size;
    uint32_t used;

    board_list_t **data;
} board_hash_table_t;

board_hash_table_t *board_hash_table_create(uint32_t size);

board_t *board_hash_table_search(board_hash_table_t *table,
                                 board_t *board);

void board_hash_table_expand(board_hash_table_t *table,
                             uint32_t size);

board_t *board_hash_table_insert(board_hash_table_t *table,
                                 board_t *board);

void board_hash_table_print(board_hash_table_t *table);

void board_hash_table_free(board_hash_table_t *table);

#endif
