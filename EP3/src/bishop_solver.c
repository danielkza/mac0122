#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "board_hash_table.h"

#include "bishop_solver.h"

#define BOARD_HASH_TABLE_INITIAL_SIZE 1<<20

void bishops_solve_partial(board_t *board,
                           board_hash_table_t *table,
                           uint8_t start_x,
                           uint8_t start_y,
                           int remaining)
{
    uint8_t x, y;

    board_add_bishop(board, start_x, start_y);

    if(remaining == 1) {
        board_hash_table_insert(table, board);
    } else {
        x = start_x;
        y = start_y;

        for(;;) {
            if(++x >= board->size) {
                x = 0;
                if(++y >= board->size)
                    break;
            }

            if(board_get_pos(board, x, y) == 0) {
                bishops_solve_partial(board, table, x, y, remaining-1);
            }
        }
    }

    board_pop_bishop(board);
}

int bishops_count_equivalent(board_hash_table_t *table, int print)
{
    unsigned int count, i;
    if(table == NULL)
        return 0;

    count = 0;
    for(i = 0; i < table->size; i++) {
    #if 1
        board_list_t *list, *cmp;

        for(list = table->data[i]; list != NULL; list = list->next) {
            int found_equiv = 0;

            for(cmp = list->next; cmp != NULL; cmp = cmp->next) {
                if(list->hash == cmp->hash && list->hash2 == cmp->hash2
                   && board_equivalent(list->item, cmp->item))
                {
                    found_equiv = 1;
                    break;
                }
            }

            if(!found_equiv) {
                count++;
            }

            /* board_print(list->item); */
        }
    #else
        if(table->data[i] != NULL)
            count++;
    #endif
    }

    return count;
}

board_hash_table_t *bishops_solve(uint8_t board_size, unsigned int num_bishops)
{
    board_t *board;
    board_hash_table_t *table;
    uint8_t x, y;

    board = board_create(board_size, num_bishops);
    table = board_hash_table_create(BOARD_HASH_TABLE_INITIAL_SIZE);

    x = 0; y = 0;
    for(;;) {
        printf("progress: %d/%d\n", (y * board_size) +x, board_size * board_size);
        bishops_solve_partial(board, table, x, y, num_bishops);

        if(++x >= board->size) {
            x = 0;
            if(++y >= board->size)
                break;
        }
    }

    board_free(board);

    return table;
}
