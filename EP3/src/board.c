#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "board.h"

static unsigned int board_size_to_bytes(int n)
{
    unsigned int data_size = (n * n) / CHAR_BIT;

    /* Account for truncation: we need to round upwards */
    if((n * n) % CHAR_BIT != 0)
        data_size++;

    return data_size;
}

static board_t *board_init_new(int size,
                               board_t *copy_from)
{
    board_t *board;
    unsigned int data_size;
    unsigned char *data;

    if(size <= 0 || size > UCHAR_MAX+1)
        return NULL;

    board = malloc(sizeof(*board));
    if(board == NULL)
        return NULL;

    if(copy_from != NULL && copy_from->size != size)
        return NULL;

    data_size = board_size_to_bytes(size);

    if(copy_from != NULL) {
        data = malloc(data_size * sizeof(*data));
        if(data != NULL)
            memcpy(data, copy_from->data, data_size);
    } else {
        data = calloc(data_size, sizeof(*data));
    }

    if(data == NULL) {
        free(board);
        return NULL;
    }

    board->size = size;
    board->data = data;

    return board;
}

board_t *board_create(int size)
{
    return board_init_new(size, NULL);
}

board_t *board_clone(board_t *board)
{
    if(board == NULL)
        return NULL;

    return board_init_new(board->size, board);
}

static int get_bit(unsigned char x,
                            int n)
{
    return (x >> n) & 1;
}

static unsigned char set_bit(unsigned char x,
                             int n)
{
    return x | (1 << n);
}

static unsigned char unset_bit(unsigned char x,
                               int n)
{
    return x & ~(1 << n);
}


static int board_get_pos_internal(board_t *board,
                                  unsigned char x,
                                  unsigned char y)
{
    unsigned int pos_idx, pos_byte, pos_bit;

    pos_idx = (y * board->size) + x;
    pos_byte = pos_idx / CHAR_BIT;
    pos_bit = pos_idx % CHAR_BIT;

    return get_bit(board->data[pos_byte], pos_bit);
}


int board_get_pos(board_t *board,
                  unsigned char x,
                  unsigned char y)
{
    if(board == NULL)
        return 0;

    if(x < 0 || x >= board->size || y < 0 || y >= board->size)
        return 0;

    return board_get_pos_internal(board, x, y);
}

static void board_set_pos_internal(board_t* board,
                                   unsigned char x,
                                   unsigned char y,
                                   int state)
{
    unsigned int pos_idx, pos_byte, pos_bit;
    unsigned char data_byte;

    pos_idx = (y * board->size) + x;
    pos_byte = pos_idx / CHAR_BIT;
    pos_bit = pos_idx % CHAR_BIT;

    data_byte = board->data[pos_byte];

    board->data[pos_byte] = (state != 0) ? set_bit(data_byte, pos_bit)
                                         : unset_bit(data_byte, pos_bit);
}

void board_set_pos(board_t *board,
                   unsigned char x,
                   unsigned char y,
                   int state)
{
    if(board == NULL)
        return;

    if(x < 0 || x >= board->size || y < 0 || y >= board->size)
        return;

    board_set_pos_internal(board, x, y, state);
}

void board_rotate(board_t *src,
                  board_t *dest)
{
    unsigned char x, y;
    unsigned int pos_byte, cur_byte, data_size;

    if(src == NULL || dest == NULL || src->size != dest->size)
        return;

    data_size = board_size_to_bytes(src->size);

    y = 0;
    x = 0;
    for(pos_byte = 0; pos_byte < data_size; pos_byte++) {
        unsigned char cur_byte = src->data[pos_byte];

        for(i = 0; i < CHAR_BIT; i++) {
            int x_new = (src->size - 1) - y,
                y_new = x;

            if(++x >= src->size) {
                x = 0;
                if(++y >= src->size)
                    return;
            }

            board_set_pos_internal(dest, x_new, y_new, cur_byte & (1 << i));
        }
    }
}

void board_flip(board_t *src,
                board_t *dest)
{

    unsigned char x, y;
    unsigned int pos_byte, cur_byte, data_size;

    if(src == NULL || dest == NULL || src->size != dest->size)
        return;

    data_size = board_size_to_bytes(src->size);

    y = 0;
    x = 0;
    for(pos_byte = 0; pos_byte < data_size; pos_byte++) {
        unsigned char cur_byte = src->data[pos_byte];

        for(i = 0; i < CHAR_BIT; i++) {
            int x_new = (src->size - 1) - x,
                y_new = y;

            if(++x >= src->size) {
                x = 0;
                if(++y >= src->size)
                    return;
            }

            board_set_pos_internal(dest, x_new, y_new, cur_byte & (1 << i));
        }
    }
}

int board_compare(board_t *board_1, board_t *board_2)
{
    if(board_1 == NULL || board_2 == NULL)
        return 0;

    if(board_1->size != board_2->size)
        return 0;

    return memcmp(board_1->data, board_2->data, board_size_to_bytes(board_1->size));

}

unsigned long board_hash(board_t *board, board_pos_t *lookup_table)
{
    int x, y;
    int pos_byte, cur_byte, data_size;
    unsigned long hash;

    if(board == NULL || lookup_table == NULL)
        return 0;

    /* The distance between occupied positions and the pseudo-center of the
     * board will not change for any of the transformations that matter to us,
     * rotation and flipping. We'll use them to generate a hash that is
     * guaranteed to be equal for each equivalent board.
     */

    data_size = board_size_to_bytes(board->size);

    y = 0;
    x = 0;

    hash = 0;
    for(pos_byte = 0; pos_byte < data_size; pos_byte++) {
        unsigned char cur_byte = board->data[pos_byte];

        unsigned char dist_x, dist_y;
        unsigned int temp;

        for(i = 0; i < CHAR_BIT; i++) {
            if(++x >= board->size) {
                x = 0;
                if(++y >= board->size)
                    goto done;
            }

            if(!(cur_byte & (1 << i)))
                continue;

            dist_x = x % board->size;
            dist_y = y % board->y;

            hash += (dist_x * 92881) ^ dist_y;

    }
done:








}

void board_free(board_t *board)
{
    if(board != NULL) {
        free(board->data);
        free(board);
    }
}

