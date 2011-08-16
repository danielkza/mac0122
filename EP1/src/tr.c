#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token_stack.h"
#include "tr_parser.h"

int main(int argc, char* argv[])
{
	const char* test = "\\440abc[abs]--***:::101023";

	token_stack_t* tokens = tr_parser_tokenize(test);
	token_t* token;

	while((token = token_stack_pop(&tokens)) != NULL) {
		printf("token:\n"
			   "	type: %d\n"
			   "	value: %c\n", token->type, token->value);
	}

	system("pause");
}