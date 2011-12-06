#ifndef BISHOP_SOLVER_H
#define BISHOP_SOLVER_H

#include <stddef.h>
#include <stdint.h>

#include "board.h"
#include "board_hash_table.h"

void bishops_solve_partial(board_t *board,
                           board_hash_table_t *table,
                           uint8_t start_x,
                           uint8_t start_y,
                           int remaining);

board_hash_table_t *bishops_solve(uint8_t board_size,
                   unsigned int num_bishops);

int bishops_count_equivalent(board_hash_table_t *table, int print);

#endif
