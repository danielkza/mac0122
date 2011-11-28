#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"

int main()
{
	board_t *board = board_create(4),
	        *alt = board_create(4);
	if(board != NULL) {
		int x, y;

		board_set_pos(board, 0, 1, 1);
		board_set_pos(board, 1, 1, 1);
		board_set_pos(board, 2, 1, 0);
		board_set_pos(board, 3, 1, 1);

		for(y = 0; y < 4; y++) {
			for(x = 0; x < 4; x++) {
				printf("%d ", board_get_pos(board, x, y));
			}

			printf("\n");
		}
		printf("\n");

		board_rotate(board, alt, -1);

		for(y = 0; y < 4; y++) {
			for(x = 0; x < 4; x++) {
				printf("%d ", board_get_pos(alt, x, y));
			}

			printf("\n");
		}
		printf("\n");


		board_rotate(board, alt, -2);

		for(y = 0; y < 4; y++) {
			for(x = 0; x < 4; x++) {
				printf("%d ", board_get_pos(alt, x, y));
			}

			printf("\n");
		}
		printf("\n");

		board_rotate(board, alt, 5);

		for(y = 0; y < 4; y++) {
			for(x = 0; x < 4; x++) {
				printf("%d ", board_get_pos(alt, x, y));
			}

			printf("\n");
		}
		printf("\n");

		board_free(board);
	}

	return 0;
}
