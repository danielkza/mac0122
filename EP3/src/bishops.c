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

	table = bishops_solve(board_size, bishop_num);
	if(display_boards) {
		/*board_hash_table_print(table) */;
	}

	equiv_count = bishops_count_equivalent(table, display_boards != 0);

	printf("%d tabuleiros, %d nao equivalentes\n", table->used, equiv_count);

	return 0;

}
