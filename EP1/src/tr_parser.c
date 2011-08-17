#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "char_classes.h"
#include "char_vector.h"
#include "token_stack.h"

#include "tr_parser.h"

enum tr_parser_state_t {
	STATE_READING,
	STATE_BRACKET_OPEN,
	STATE_BRACKET_CLOSED,
	STATE_CLASS_OPEN,
	STATE_CLASS_READING,
	STATE_CLASS_CLOSED,
	STATE_EQUIV_OPEN,
	STATE_EQUIV_READING,
	STATE_EQUIV_CLOSE,
	STATE_RANGE_STARTED,
};

char_vector_t* tr_parser_parse(token_stack** tokens, int string1_length)
{
	tr_parser_state_t state = PARSER_STATE_READING;
	token_t *token;
	
	char_vector* char_list = char_vector_new(16);
	char current_class[16] = "";

	if(!char_list)
		return NULL;

	token_t *prev = NULL, *token = NULL;
	while(1) {
		token_type_t type;

		if(token)
			prev = token;

		token =  token_stack_pop(tokens);
		if(!token)
			break;

		type = token->type;

		switch(state) {
		case 0:
			break;
		}
	}
}

enum tokenizer_state_t {
	STATE_READING,
	STATE_BRACKET_OPENED,
	STATE_WAITING_BRACKET_CLOSE,
	STATE_WAITING_CLASS_CLOSE,
	STATE_WAITING_EQUIV_CLOSE
};

#define PUSH_TOKEN(type, value) token_stack_push(stack, token_new(type, value))

void tr_parser_next_token(const char **str, tokenizer_state_t state, token_stack_t** stack) {
	char c;
	const char *str_pos = *str;
		
	if(!str || !(*str) || **str == '\0' || !stack || !(*stack))
		return NULL;

	c = *str_pos;

	if(c == '\\') {
		c = (*(++str_pos) == '\0') ? '\\' : tr_parser_unescape(str_pos, &str_pos);

		// A bracket was previously opened, and we found an ordinary character.
		// Therefore, a repetition was started
		if(state == STATE_BRACKET_OPENED) {
			PUSH_TOKEN(TOKEN_BRACKET_OPEN, 0));
			
			state = STATE_WAITING_BRACKET_CLOSE;
		}

		PUSH_TOKEN(TOKEN_CHAR, c);
		
		// The position is already right, dont increment it.
		return;
	} else if (c == '[') {
		state = STATE_BRACKET_OPENED;
	
	} else if (c == ':' && state == STATE_BRACKET_OPENED) {
		PUSH_TOKEN(TOKEN_CLASS_START, 0);

		state = STATE_WAITING_CLASS_CLOS;E
	
	} else if (c == '=' && state == STATE_BRACKET_OPENED) {
		PUSH_TOKEN(TOKEN_EQUIV_START, 0);

		state = STATE_WAITING_EQUIV_CLOSE;
	
	} else if (c == '*' && state == STATE_WAITING_BRACKET_CLOSE) {
		PUSH_TOKEN(TOKEN_REPEAT_MARK, 0);
	
	} else if (c == '-') {
		PUSH_TOKEN(TOKEN_RANGE_MARK, 0);

	} else if (c == ']') {
		if(state == STATE_WAITING_CLASS_CLOSE)
			PUSH_TOKEN(TOKEN_CLASS_END, 0);
	
		else if(state == STATE_WAITING_EQUIV_CLOSE)
			PUSH_TOKEN(TOKEN_EQUIV_END, 0);
		
		else if(state == STATE_WAITING_BRACKET_CLOSE)
			PUSH_TOKEN(TOKEN_BRACKET_CLOSE, 0);

	} else {
		if(state == STATE_BRACKET_OPENED)
			PUSH_TOKEN(TOKEN_BRACKET_OPEN, 0);
		
		if(isdigit(c))
			PUSH_TOKEN(TOKEN_NUMERAL, c);
	}

	// update the position counter
	*str = (str_pos + 1);
	return;
}

#undef PUSH_TOKEN

token_stack_t* tr_parser_tokenize(const char *str)
{
	const char *str_pos;
	char c = 0;

	token_t *token = NULL;
	token_stack_t *token_stack = NULL;
		
	if(!str || *str == '\0')
		return NULL;
	
	str_pos = str;
	while((token = tr_parser_next_token(&str_pos)) != NULL)
		token_stack_push(&token_stack, token);

	return token_stack;
}


/* Interprets an espace sequence following a backslash into the correct
 * character.
 *
 * The passed string must start right *after* the backslash.
 *
 * If out is not NULL, after the function returns its pointee will be set to
 * the address of the character directly following the last matched character.
 */

char tr_parser_unescape(const char *in, const char **out)
{
	const char char_table[][2] = {
		{'a', '\a'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'},
		{'r', '\r'}, {'t', '\t'}, {'v', '\v'}, {'\\', '\\'}
	};
	
	char c = *in;
	char octal_val[OCTAL_LITERAL_MAX_LENGTH+1];
	int i, octal_num;

	if(c == '\0') {
		*out = in;
		return 0;
	}

	// try special characters from table first
	for(i = 0; i < ARRAY_SIZE(char_table); ++i) {
		if(c == char_table[i][0]) {
			*out = in + 1;
			return char_table[i][1];
		}
	}

	// try to parse an octal literal
	strncpy(octal_val, in, OCTAL_LITERAL_MAX_LENGTH);
	octal_val[OCTAL_LITERAL_MAX_LENGTH] = '\0';

	do {
		char *octal_val_end = octal_val;
		octal_num = strtol(octal_val, &octal_val_end, 8);
		
		if(octal_val == octal_val_end)
			// strtol failed
			break;
		else if(octal_num > UCHAR_MAX)
			// literal too large, try with one fewer character
			*(octal_val_end-1) = '\0';
		else {
			// all good
			*out = in + (octal_val_end - octal_val);
			return octal_num;
		}
	} while(octal_val[0] != '\0');

	// no match found,, return the next char unchanged
	*out = in + 1;
	return c;
}