#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "char_classes.h"
#include "char_vector.h"
#include "token_stack.h"

#include "tr_parser.h"

enum tr_parser_state_t {
	READING,
	BRACKET_OPEN,
	BRACKET_CLOSED,
	CLASS_OPEN,
	CLASS_READING,
	CLASS_CLOSED,
	EQUIV_OPEN,
	EQUIV_READING,
	EQUIV_CLOSE,
	RANGE_STARTED,
}

#define TOKEN_FIRST(type) case type:
#define TOKEN(type) break; case type:
#define TOKEN_LAST(type) break

char_vector* tr_parser_parse(token_stack** tokens, int string1_length)
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
		case READING:
			switch(type) {
			case BRACKET_OPEN:
				state = PARSER_STATE_BRACKET_OPEN;
			break;
			case HYPHEN
				if(!prev)
					PARSER_ERROR("Range start without previous character");
				else
					state = PARSER_STATE_RANGE_STARTED;
			TOKEN_T(TYPE_CHAR)
				char_vector_append_char(char_lis, token->value);
			
				PARSER_ERROR("Invalid token");
			}

		case PARSER_STATE_BRACKET_OPEN:
			if(type == TOKEN_CLASS_COLON)
				state = PARSER_STATE_CLASS_OPEN;
			else if(type == TOKEN_CLASS_EQUALS)
				state = PARSER_STATE_EQUIV_OPEN

	}






token_t* tr_parser_next_token(const char **str) {
	char c;
	const char *str_pos;
	token_t *token;
	
	if(!str || !(*str) || **str == '\0')
		return NULL;

	token = (token_t*)malloc(sizeof(token_t));
	if(!token)
		return NULL;

	str_pos = *str;
	c = *str_pos;

	// this is not in the switch because it is the only sequence that may 
	// advance the position more than a single character
	if(c == '\\') {
		const char *new_pos = NULL;
		str_pos++;

		// The string ends in a backslash
		if(*str_pos == '\0') {
			token->type = TOKEN_CHAR;
			token->value = '\\';
		} else {
			c = tr_parser_unescape(str_pos, &new_pos);
			
			// didn't match an escape sequence, just use the next character
			// unchanged
			if(str_pos == new_pos)
				c = *(str_pos++);
			else
				str_pos = new_pos;
			
			token->type = TOKEN_CHAR;
			token->value = c;
		}
	} else {
		// match single-character tokens
		switch(c) {
		case '-': token->type = TOKEN_HYPHEN;        break;
		case '[': token->type = TOKEN_BRACKET_OPEN;  break;
		case ']': token->type = TOKEN_BRACKET_CLOSE; break;
		case '=': token->type = TOKEN_EQUALS;        break;
		case ':': token->type = TOKEN_COLON;         break;
		case '*': token->type = TOKEN_ASTERISK;      break;
		default:
			if(isdigit(c))
				token->type = TOKEN_NUMBER;
			else
				token->type = TOKEN_CHAR;
		}

		token->value = c;
		str_pos++;
	}

	// update the position counter
	*str = str_pos;
	return token;
}

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

	// no match found
	*out = in;
	return 0;
}