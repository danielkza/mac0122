#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "board.h"

static unsigned int get_bit(unsigned char x,
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

static board_t *board_init_new(int size,
                               board_t *copy_from)
{
    board_t *board;
    int data_size;
    unsigned char *data;

    if(size <= 0)
        return NULL;

    board = malloc(sizeof(*board));
    if(board == NULL)
        return NULL;

    if(copy_from != NULL && copy_from->size != size)
        return NULL;

    data_size = (size*size) / CHAR_BIT;

    /* Account for truncation: we need to round upwards */
    if((size * size) % CHAR_BIT != 0)
        data_size++;

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

static int board_get_pos_internal__(board_t *board,
                                    int x,
                                    int y)
{
    int pos_idx, pos_byte, pos_bit;

    pos_idx = (y * board->size) + x;
    pos_byte = pos_idx / CHAR_BIT;
    pos_bit = pos_idx % CHAR_BIT;

    return (int)get_bit(board->data[pos_byte], pos_bit);
}


int board_get_pos(board_t *board,
                  int x,
                  int y)
{
    if(board == NULL)
        return 0;

    if(x < 0 || x >= board->size || y < 0 || y >= board->size)
        return 0;

    return board_get_pos_internal__(board, x, y);
}

static void board_set_pos_internal__(board_t* board,
                                     int x,
                                     int y,
                                     int state)
{
    int pos_idx, pos_byte, pos_bit;
    unsigned char data_byte;

    pos_idx = (y * board->size) + x;
    pos_byte = pos_idx / CHAR_BIT;
    pos_bit = pos_idx % CHAR_BIT;

    data_byte = board->data[pos_byte];

    board->data[pos_byte] = (state != 0) ? set_bit(data_byte, pos_bit)
                                         : unset_bit(data_byte, pos_bit);
}

void board_set_pos(board_t *board,
                   int x,
                   int y,
                   int state)
{
    if(board == NULL)
        return;

    if(x < 0 || x >= board->size || y < 0 || y >= board->size)
        return;

    board_set_pos_internal__(board, x, y, state);
}

void board_rotate(board_t *src,
                  board_t *dest,
                  int direction)
{
    int x, y;

    if(src == NULL || dest == NULL || src->size != dest->size)
        return;

    /* The direction will always be converted to one of 4 possible cases:
     - 90 degree clockwise rotation (1)
     - 180 degree rotation (2)
     - 90 degree counter-clockwise rotation (3)
     - 360 degree rotation, no-op (0)
    */

    /* If direction is negative the modulus operator will return the negative
     result closest to 0, not the 'canonical' positive result. The next lines
     accounts for this
    */

    direction = (direction % 4);
    if(direction == 0)
    	return;
    else if(direction < 0)
    	direction += 4;


    for(y = 0; y < src->size; y++) {
        for(x = 0; x < src->size; x++) {
        	int x_new = 0, y_new = 0;
            int prev_state = board_get_pos_internal__(src, x, y);

            switch(direction)  {
                case 1:
                	x_new = (src->size - 1) - y;
                	y_new = x;
                	break;
                case 2:
                	x_new = (src->size - 1) - x;
                	y_new = (src->size - 1) - y;
                	break;
                case 3:
                    x_new = y;
                    y_new = (src->size - 1) - x;
        	}

        	board_set_pos_internal__(dest, x_new, y_new, prev_state);
        }
    }
}

void board_free(board_t *board)
{
    if(board != NULL) {
        free(board->data);
        free(board);
    }
}

