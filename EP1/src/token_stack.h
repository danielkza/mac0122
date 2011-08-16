#ifndef TR_TOKEN_STACK_H
#define TR_TOKEN_STACK_H

#include "char_vector.h"
#include "char_classes.h"

typedef enum {
	TOKEN_CHAR,
	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,
	TOKEN_EQUALS,
	TOKEN_HYPHEN,
	TOKEN_COLON,
	TOKEN_ASTERISK,
	TOKEN_NUMBER
} token_type_t;

typedef struct {
	token_type_t type;
	char value;
} token_t;

typedef struct token_stack_t {
	token_t* token;
	struct token_stack_t* next;
} token_stack_t;

int token_stack_push(token_stack_t** head, token_t* token);
token_t* token_stack_pop(token_stack_t** head);


#endif // #ifndef TR_TOKEN_STACK_H