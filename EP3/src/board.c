#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include "hash_function.h"

#include "board.h"

static board_t *board_init_new(uint8_t size,
                               uint16_t max_bishops,
                               const board_t *copy_from)
{
    board_t *board;
    uint32_t data_size, bishops_size;
    uint8_t *data;
    board_bishop_t *bishops;

    if(size == 0 || max_bishops == 0)
        return NULL;

    board = malloc(sizeof(*board));
    if(board == NULL)
        return NULL;

    if(copy_from != NULL
       && (copy_from->size != size || copy_from->max_bishops != max_bishops))
    {
        return NULL;
    }

    data_size = sizeof(*data) * (size * size);
    data = malloc(data_size);
    if(data == NULL) {
        free(board);
        return NULL;
    }

    bishops_size = sizeof(*bishops) * max_bishops;
    bishops = malloc(bishops_size);
    if(bishops == NULL) {
        free(board);
        free(data);
        return NULL;
    }

    if(copy_from != NULL) {
        memcpy(data, copy_from->data, data_size);
        memcpy(bishops, copy_from->bishops, bishops_size);

        board->cur_bishops = copy_from->cur_bishops;
    } else {
        memset(data, 0, data_size);
        board->cur_bishops = 0;
    }

    board->size = size;
    board->data = data;
    board->max_bishops = max_bishops;
    board->bishops = bishops;

    return board;
}

board_t *board_create(uint8_t size, uint16_t max_bishops)
{
    return board_init_new(size, max_bishops, NULL);
}

board_t *board_clone(const board_t *board)
{
    if(board == NULL)
        return NULL;

    return board_init_new(board->size, board->max_bishops, board);
}

void board_free(board_t *board)
{
    if(board != NULL) {
        free(board->data);
        free(board->bishops);
        free(board);
    }
}

void board_copy_data(board_t *dest,
                     const board_t *src)
{
    uint32_t data_size, bishops_size;
    if(src == NULL || dest == NULL)
        return;

    if(src->size != dest->size || src->max_bishops != dest->max_bishops)
        return;

    data_size = sizeof(*src->data) * src->size;
    bishops_size = sizeof(*src->bishops) * src->max_bishops;

    memcpy(dest->data, src->data, data_size);
    memcpy(dest->bishops, src->bishops, bishops_size);

    dest->cur_bishops = src->cur_bishops;
}

static int board_get_pos_internal(const board_t *board,
                                  uint8_t x,
                                  uint8_t y)
{
    return board->data[(y * board->size) + x];
}


int board_get_pos(const board_t *board,
                  uint8_t x,
                  uint8_t y)
{
    if(board == NULL)
        return 0;

    if(x >= board->size || y >= board->size)
        return 0;

    return board_get_pos_internal(board, x, y);
}

static void board_set_pos_internal(board_t* board,
                                   uint8_t x,
                                   uint8_t y,
                                   int state)
{

    board->data[(y * board->size) + x] = state;
}

void board_set_pos(board_t *board,
                   uint8_t x,
                   uint8_t y,
                   int state)
{
    if(board == NULL)
        return;

    if(x >= board->size || y >= board->size)
        return;

    board_set_pos_internal(board, x, y, state);
}

static void board_fix_diagonals(board_t* board,
                                uint8_t start_x,
                                uint8_t start_y,
                                int state)
{
    int i;
    int16_t x, y;

    typedef struct  {
        int x_end, x_delta;
        int y_end, y_delta;
    } direction;

    direction directions[] = {
        { 0, +1,  0, +1},
        { 0, +1, -1, -1},
        {-1, -1,  0, +1},
        {-1, -1, -1, -1}
    };
    const int direction_count = sizeof(directions) / sizeof(directions[0]);

    /* Only constants can be used for initializares in C89 :( */
    directions[0].x_end = directions[0].y_end = board->size;
    directions[1].x_end = directions[2].y_end = board->size;

    if(board == NULL)
        return;

    board_set_pos_internal(board, start_x, start_y, board_get_pos_internal(board, start_x, start_y) + state);

    for(i = 0; i < direction_count; i++) {
        direction d = directions[i];

        for(x = start_x + d.x_delta, y = start_y + d.y_delta;
            x != d.x_end && y != d.y_end;
            x += d.x_delta, y += d.y_delta)
        {
            board_set_pos_internal(board, x, y, board_get_pos_internal(board, x, y) + state);
        }
    }
}

static void board_rotate_internal(board_bishop_t *bishops,
                                  uint16_t bishop_num,
                                  uint8_t board_size)
{
    uint8_t x, y;
    uint16_t i;
    for(i = 0; i < bishop_num; i++) {
        x = bishops[i].x;
        y = bishops[i].y;

        bishops[i].x = (board_size - 1) - y,
        bishops[i].y = x;
    }
}

void board_rotate(board_t *board)
{
    uint16_t i;

    if(board == NULL || board->cur_bishops == 0)
        return;

    board_rotate_internal(board->bishops, board->cur_bishops, board->size);
    memset(board->data, 0, sizeof(*board->data) * (board->size * board->size));

    for(i = 0; i < board->cur_bishops; i++) {
        board_fix_diagonals(board, board->bishops[i].x, board->bishops[i].y, +1);
    }
}

static void board_flip_internal(board_bishop_t *bishops,
                                uint16_t bishop_num,
                                uint8_t board_size)
{
    uint8_t x, y;
    uint16_t i;
    for(i = 0; i < bishop_num; i++) {
        x = bishops[i].x;
        y = bishops[i].y;

        bishops[i].x = (board_size - 1) - x,
        bishops[i].y = y;
    }
}

void board_flip(board_t *board)
{
    uint16_t i;

    if(board == NULL || board->cur_bishops == 0)
        return;

    board_flip_internal(board->bishops, board->cur_bishops, board->size);
    memset(board->data, 0, sizeof(*board->data) * (board->size * board->size));

    for(i = 0; i < board->cur_bishops; i++) {
        board_fix_diagonals(board, board->bishops[i].x, board->bishops[i].y, +1);
    }
}

static int board_compare_internal(board_bishop_t *bishops_1,
                                  board_bishop_t *bishops_2,
                                  uint16_t bishop_num)
{
    uint16_t i, j;
    for(i = 0; i < bishop_num; i++) {
        uint8_t x = bishops_1[i].x,
                y = bishops_1[i].y;
        int found = 0;

        for(j = 0; j < bishop_num; j++) {
            if(bishops_2[j].x == x
               && bishops_2[j].y == y)
            {
                found = 1;
                break;
            }
        }

        if(!found)
            return 0;
    }

    return 1;
}

int board_compare(const board_t *board_1, const board_t *board_2)
{

    if(board_1 == NULL || board_2 == NULL)
        return 0;

    if(board_1 == board_2)
        return 1;

    if(board_1->size != board_2->size
       || board_1->max_bishops != board_2->max_bishops
       || board_1->cur_bishops != board_2->cur_bishops)
    {
        return 0;
    }

    return board_compare_internal(board_1->bishops, board_2->bishops, board_1->cur_bishops);
}

int board_equivalent(const board_t *board_1, const board_t *board_2)
{
    board_bishop_t *bishops;
    uint32_t bishops_size;
    int i;

    if(board_1 == NULL || board_2 == NULL)
        return 0;

    if(board_1 == board_2)
        return 1;

    if(board_1->size != board_2->size
       || board_1->max_bishops != board_2->max_bishops
       || board_1->cur_bishops != board_2->cur_bishops)
    {
        return 0;
    }

    bishops_size = sizeof(*bishops) * board_1->cur_bishops;
    bishops = malloc(bishops_size);
    if(bishops == NULL)
        return board_compare(board_1, board_2);

    memcpy(bishops, board_2->bishops, bishops_size);

    for(i = 1; i <= 8; i++) {
        if(board_compare_internal(board_1->bishops, bishops, board_1->cur_bishops)) {
            free(bishops);
            return 1;
        }

        board_rotate_internal(bishops, board_2->cur_bishops, board_2->size);
        if(i == 4)
            board_flip_internal(bishops, board_2->cur_bishops, board_2->size);
    }

    board_flip_internal(bishops, board_2->cur_bishops, board_2->size);

    /* return to the original */
    free(bishops);
    return 0;
}

uint64_t board_hash(board_t *board, uint64_t *hash2_out)
{
    board_bishop_t *bishops;
    uint32_t bishops_size;
    uint64_t hash = 0, hash2 = 0;
    int i;

    if(board == NULL)
        return 0;

    bishops = board->bishops;
    bishops_size = sizeof(*bishops) * board->cur_bishops;

    for(i = 1; i <= 8; i++) {
        int j, k;
        for(j = 0; j < board->cur_bishops; j++) {
            for(k = j+1; k < board->cur_bishops; k++) {
                uint8_t x1 = board->bishops[j].x,
                        y1 = board->bishops[j].y,
                        x2 = board->bishops[k].x,
                        y2 = board->bishops[k].y;

                uint32_t words[2], h1 = 0, h2 = 0;
                words[0] = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
                words[1] = ((x1 + x2) + (y1 + y2)),

                hashword2(words, 2, &h1, &h2);

                hash += h1;
                hash2 += h2;
            }
        }

        board_rotate_internal(bishops, board->cur_bishops, board->size);

        if(i == 4) {
            board_flip_internal(bishops, board->cur_bishops, board->size);
        }
    }
    /* return to the original */
    board_flip_internal(bishops, board->cur_bishops, board->size);

    if(hash2_out != NULL)
        *hash2_out = hash2;

    return hash;
}


void board_add_bishop(board_t *board, uint8_t x, uint8_t y)
{
    if(board == NULL)
        return;

    if(x >= board->size || y >= board->size || board->cur_bishops >= board->max_bishops)
        return;

    board->bishops[board->cur_bishops].x = x;
    board->bishops[board->cur_bishops].y = y;
    board->cur_bishops++;

    board_fix_diagonals(board, x, y, +1);
}

void board_pop_bishop(board_t *board)
{
    uint8_t x, y;

    if(board == NULL || board->cur_bishops == 0)
        return;

    x = board->bishops[board->cur_bishops - 1].x;
    y = board->bishops[board->cur_bishops - 1].y;
    board->cur_bishops--;

    board_fix_diagonals(board, x, y, -1);
}

void board_print(const board_t *board)
{
    uint8_t x, y;
    int i;
    board_t *board_print;

    if(board == NULL)
        return;

    board_print = board_create(board->size, 1);
    if(board_print == NULL)
        return;

    for(i = 0; i < board->cur_bishops; i++) {
        board_set_pos_internal(board_print,
                               board->bishops[i].x,
                               board->bishops[i].y,
                               1);
    }

    for(y = 0; y < board->size; y++) {
        for(x = 0; x < board->size; x++) {
            printf("%c ", board_get_pos_internal(board_print, x, y) ? 'X' : '_');
        }
        printf("\n");
    }
    printf("\n");

    board_free(board_print);
}

