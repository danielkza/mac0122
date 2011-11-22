#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "board.h"

static unsigned int get_bit(unsigned char x, int n)
{
	return (x >> n) & 1;
}

static unsigned char set_bit(unsigned char x, int n)
{
	return x | (1 << n);
}

static unsigned char unset_bit(unsigned char x, int n)
{
	return x & ~(1 << n);
}

static board_t *board_init_new(int size, board_t *copy_from)
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

board_t *board_create(int size) {
	return board_init_new(size, NULL);
}

board_t *board_clone(board_t *board) {
	if(board == NULL)
		return NULL;

	return board_init_new(board->size, board);
}

int board_get_pos(board_t *board, int x, int y)
{
	int pos_idx, pos_byte, pos_bit;

	if(board == NULL || x < 0 || y < 0)
		return 0;

	if(x >= board->size || y >= board->size)
		return 0;

	pos_idx = (y * board->size) + x;
	pos_byte = pos_idx / CHAR_BIT;
	pos_bit = pos_idx % CHAR_BIT;

	return (int)get_bit(board->data[pos_byte], pos_bit);
}

void board_set_pos(board_t *board, int x, int y, int state)
{
	int pos_idx, pos_byte, pos_bit;
	unsigned char data_byte;

	if(board == NULL || x < 0 || y < 0)
		return;

	if(x >= board->size || y >= board->size)
		return;

	pos_idx = (y * board->size) + x;
	pos_byte = pos_idx / CHAR_BIT;
	pos_bit = pos_idx % CHAR_BIT;

	data_byte = board->data[pos_byte];

	board->data[pos_byte] = (state != 0) ? set_bit(data_byte, pos_bit)
	                                     : unset_bit(data_byte, pos_bit);
}

void board_free(board_t *board)
{
	if(board != NULL) {
		free(board->data);
		free(board);
	}
}

