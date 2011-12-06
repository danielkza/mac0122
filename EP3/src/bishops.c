#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>

#include "board.h"
#include "bishop_solver.h"

void print_help()
{
	printf("Uso: ep3 n m [iv]\n");
}

int main(int argc, char **argv)
{
	unsigned int board_size, bishop_num;
	int display_boards = 0, display_stats = 0;
	board_hash_table_t *table;
	int i, equiv_count;

	if(argc < 3) {
		print_help();
		return 1;
	}

	if(!(board_size = atoi(argv[1]))
	   || !(bishop_num = atoi(argv[2]))
	   || board_size < bishop_num)
	{
		print_help();
		return 1;
	}

	for(i = 3; i < argc; i++) {
		if(strchr(argv[i], 'i'))
			display_boards = 1;
		if(strchr(argv[i], 'v'))
			display_stats = 1;
	}

    #if 0
	{
		board_t *board = board_create(4, 2);
		int i;
		uint64_t hash, hash2;

		board_add_bishop(board, 1, 0);
		board_add_bishop(board, 1, 1);
		board_print(board);

		hash = board_hash(board, &hash2);
		printf("h: %llu, h2: %llu\n", hash, hash2);

		board_pop_bishop(board);
		board_pop_bishop(board);
		board_add_bishop(board, 1, 0);
		board_add_bishop(board, 1, 2);
		board_print(board);

		hash = board_hash(board, &hash2);
		printf("h: %llu, h2: %llu\n", hash, hash2);


		return 0;
	}
	#endif

	table = bishops_solve(board_size, bishop_num);
	if(display_boards) {
		/*board_hash_table_print(table) */;
	}

	printf("counting uniques\n");

	equiv_count = bishops_count_equivalent(table, display_boards != 0);

	printf("%d tabuleiros, %d unicos\n", table->used, equiv_count);

	return 0;

}
