#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <limits.h>
#include <assert.h>

#include "utils.h"
#include "char_classes.h"
#include "char_vector.h"
#include "token_stack.h"

#include "tr_parser.h"

/*
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

*/

VERIFY(sizeof(int) > sizeof(char));
static const int INVALID_CHAR = CHAR_MAX+1;

void tr_parser_error(const char* err_msg, const char* err_pos,
	                 tr_parser_error_t* error_out)
{
	if(error_out) {
		error_out->msg = strdup(err_msg);
		error_out->err_pos = err_pos;
	}
}

int tr_parser_parse_bracket_seq(const char **str, char_vector_t* out,
	                            tr_parser_error_t* error_out)
{
	char c;
	
	if(!str || !*str || !out)
		return 0;

	c = **str;
	if(c == '\0') {
		tr_parser_error("unclosed bracket", (*str)-1, error_out);
		return 0;

	} else if(c == ':') {
		wctype_t char_class = 0;
		char *class_name;
		const char* class_name_end;
		size_t class_name_len;
				
		*str++;

		class_name_end = strchr(*str, ':');
		if(!class_name_end || *(class_name_end+1) != ']') {
			tr_parser_error("unclosed character class", *str, error_out);
			return 0;
		}

		class_name_len = (class_name_end - str_pos);
		if(!class_name_len)
			TR_PARSER_ERROR_S("empty character class declaration");
				
		class_name = tr_strndup(class_name, class_name_len));
		if(!class_name)
			TR_PARSER_ERROR_S("failed to allocate memory for class name");

		char_class = wctype(class_name);
		if(!char_class)
			TR_PARSER_ERROR_S("unknown character class");

		free(class_name);
		tr_char_class_expand(char_class, vec);
			
	} else if(c == '=') {
		const char* equiv_end;

		str_pos += 2;

		if(*str_pos == '\0')
			TR_PARSER_ERROR_S("unclosed equivalence class");

		c = tr_parser_get_one_char(&str_pos, error_out);
		if(c == INVALID_CHAR)
			goto end;

		do {
			equiv_end = strchr(str_pos, '=');
		} while(equiv_end && *(equiv_end-1) == '\\');

		if(!equiv_end || *(equiv_end+1) != ']')
			TR_PARSER_ERROR_S("unclosed equivalence class");

		char_vector_append_char_equiv(char_vector, c);
			
	} else {
		const char * repeat_count_start = 0;
		unsigned int repeat_count = 0;

		c = tr_parser_get_one_char(&str_pos, error_out);

		if(c == INVALID_CHAR)
			TR_PARSER_ERROR_S("invalid char specified for repetition");
				
		if(*(str_pos+2) == '*') {
			for(str_pos += 3; *str_pos != ']'; ++str_pos) {
				if(*str_pos == '\0')
					TR_PARSER_ERROR_S("unclosed character repetition");
				else if(!isdigit(*str_pos))
					TR_PARSER_ERROR_S("invalid numeral");
				else if(!repeat_count_start)
					repeat_count_start = str_pos;
			}
		}

		repeat_count = strtoul(repeat_count_start, &repeat_count_start, 0);





char_vector_t* tr_parser_parse(const char *str, size_t target_length,
	                           tr_parser_error_t* error_out) {
	char_vector_t* vec;
	const char *str_pos;
	const char *indefinite_repeat_pos = NULL;
	
	char c;
	int last_read_char = INVALID_CHAR;

	if(!str || *str == '\0')
		return NULL;

	/* If this is the second string, the length of the first string is exactly
	 * the length we should reach.
	 *
	 * Otherwise, a good estimate of how long the output will be is the size of
	 * the input in this case, because most characters map to themselves,
	 * escapes will be at most 4 chars in length, and other constructs that
	 * expand to multiple chars will mostly (over)compensate for it.
	 */
	vec = char_vector_new(target_length > 0 ? target_length : strlen(str));
	if(!vec)
		return NULL;

#define TR_PARSER_ERROR_S(msg) \
	tr_parser_error(msg, str_pos, error_out); \
	goto end;

	// NOTE: TR_PARSER_ERROR breaks out of the loop to return an error message.
	for(str_pos = str; (c = *str_pos) != '\0'; ++str_pos) {
		if (c == '-') {
			char current;

			str_pos++;
			
			if(last_read_char == INVALID_CHAR)
				TR_PARSER_ERROR_S("range specification without start character");
			
			c = tr_parser_parse_one_char(&str_pos, error_out);
			if(c == INVALID_CHAR)
				TR_PARSER_ERROR_S("unterminated range specification");
			else if(c < last_read_char)
				TR_PARSER_ERROR_S("range end does not collate after range start");

			for(current = last_read_char; current <= c; current++) 
				char_vector_append_char(vec, current);

			last_read_char = INVALID_CHAR;
			str_pos++;

		} else if (c == '[') {
			

					









		}
	}

end:
	if(vec)
		free(vec);

	return;
}

/* Interprets an espace sequence following a backslash into the correct
 * character.
 *
 * The passed string must start right *after* the backslash.
 *
 * If out is not NULL, after the function returns its pointee will be set to
 * the address of the character directly following the last matched character.
 */

int tr_parser_parse_octal_literal(const char **str)
{
	const char *s;
	char *copy, *copy_end;
	size_t len = 0;
	unsigned int value;
	
	if(!str || !(*str))
		return INVALID_CHAR;

	// Manually count the number of octal literals found.
	// That is needed because strtoul skips whitespace, which we must not do.				
	for(s = *str; *s != '\0' && *s >= '0' && *s <= '7'; s++)
		len++;

	if(!len)
		return INVALID_CHAR;

	copy = tr_strndup(*str, len);
	if(!copy)
		return INVALID_CHAR;

	while(len > 0) {
		copy[len] = '\0';
		value = (unsigned int)strtoul(copy, &copy_end, 8);
		
		// strtol failed
		if(copy == copy_end) {
			value = INVALID_CHAR;
			break;
		// literal too large, try with one fewer character
		} else if(value > UCHAR_MAX) {
			len--;
		// all good
		} else {
			*str += (copy_end - copy);
			break;
		}
	}

	free(copy);
	return value;
}

int tr_parser_parse_one_char(const char **str, tr_parser_error_t* error_out)
{
	const char char_escape_table[][2] = {
		{'a', '\a'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'},
		{'r', '\r'}, {'t', '\t'}, {'v', '\v'}, {'\\', '\\'}
	};
	
	int c;

	if(!str || !*str)
		return INVALID_CHAR;

	c = **str;
	if(c == '\0') {
		// string ended
		return INVALID_CHAR;

	} else if (c == '\\') {
		/* parse an escape sequence, that might be:
		 * \octal, where 'octal' is a sequence of octal numerals from 1 to 3
		 * digits in length
		 * \character, where 'character is one of the POSIX escape characters
		 * \c, where 'c' is any other character. It translate to 'c' itself.
		 */

		size_t i;
		
		// skip the backslash
		(*str)++;
		c = **str;

		// a backslash as the last character is not valid
		if(c == '\0') {
			tr_parser_error("unterminated escape sequence", *str, error_out); 
			return INVALID_CHAR;
		}

		// try special characters from the POSIX table first
		for(i = 0; i < ARRAY_SIZE(char_escape_table); ++i) {
			if(c == char_escape_table[i][0]) {
				(*str)++;
				return char_escape_table[i][1];
			}
		}

		c = tr_parser_parse_octal_literal(str);
		if(c != INVALID_CHAR)
			return c;	
	}

	// no special escape meanings found: just return the character after the 
	// backslash as-is.	
	
	c = **str;
	(*str)++;
	return c;
}