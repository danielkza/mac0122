#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <limits.h>
#include <assert.h>
#include <stdarg.h>
#include <wchar.h>


#include "utils.h"
#include "char_utils.h"
#include "char_classes.h"
#include "char_vector.h"
#include "token_stack.h"

#include "tr_parser.h"

VERIFY(sizeof(int) > sizeof(char));
static const int INVALID_CHAR = CHAR_MAX+1;

void tr_fatal_error(const char* err_fmt, ...) {
	va_list ap;
	va_start(ap, err_fmt);

	if(vfprintf(stderr, err_fmt, ap) < 0) {
		fputs(err_fmt, stderr);
	}

	va_end(ap);
}

int tr_parser_error_check(tr_parser_error_t* error_out) {
	if(error_out) {
		if(error_out->err_pos || error_out->msg)
			return 1;
	}

	return 0;
}


void tr_parser_error(tr_parser_error_t* error_out, const char* err_pos,
	                 const char* err_fmt, ...)
{
	va_list ap;
	va_start(ap, err_fmt);

	if(error_out) {
		size_t len = vsnprintf(NULL, 0, err_fmt, ap);
		if(len) {
			char* msg = (char*)malloc(len+1);
			if(msg) {
				vsnprintf(msg, len+1, err_fmt, ap);
				error_out->msg = msg;
			}
		}
		
		error_out->err_pos = (char*)err_pos;
	}

	va_end(ap);
}

void tr_parser_error_reset(tr_parser_error_t* error_out)
{
	if(error_out) {
		if(error_out->msg)
			free(error_out->msg);

		error_out->msg = NULL;
		error_out->err_pos = NULL;
		error_out->start = NULL;
	}
}

const char* tr_parser_find_next_token(const char *str, const char* token)
{
	const char* token_pos;

	if(!token || *token == '\0')
		return NULL;

	token_pos = str;
	while(token_pos) {
		token_pos = strstr(token_pos, token);
		
		if(token_pos) {
			const char* backslash_pos;
			int backslash_count = 0;

			// check for backslashes before the token possibly indicating it was
			// escaped, and therefore we need to search further.
			for(backslash_pos = token_pos - 1;
				backslash_pos >= str && *backslash_pos == '\\';
				backslash_pos--)
			{
				backslash_count++;
			}
		
			// even number of backslashes actually means multiple literal 
			// backslash characters, we're good.
			if(backslash_count % 2 == 0)
				break;
		}
	}

	return token_pos;
}


int tr_parser_try_parse_repeat(const char **str,
							   int last_read_char,
                               unsigned int *repeat_count,
							   tr_parser_error_t* error_out)
{
	const char *repeat_start, *repeat_marker, *repeat_end;
	char *repeat_count_str;
	unsigned int repeat_count = 0;

	int c;

	if(!str || !*str || last_read_char == INVALID_CHAR)
		return INVALID_CHAR;

	// check for a valid repeat structure

	repeat_marker = tr_parser_find_next_token(*str, "*");
	if(!repeat_marker)
		return INVALID_CHAR;

	repeat_end = tr_parser_find_next_token(repeat_marker + 1, "]");
	if(!repeat_end)
		return INVALID_CHAR;
		
	repeat_start = *str;

	c = tr_parser_parse_one_char(&repeat_start, error_out);
	if(c == INVALID_CHAR) {
		tr_parser_error(error_out, repeat_start, "repetition with no character");
		return INVALID_CHAR;
	}

	// copy 0 or more characters that lie between the asterisk and the closing
	// bracket.

	repeat_count_str = tr_strndup(repeat_marker + 1,
		                          repeat_end - (repeat_marker + 1));
	if(!repeat_count_str) {
		tr_fatal_error("out of memory");
		return INVALID_CHAR;
	}
	
	// no characters means an indefinite repetition, as does a repetition count
	// of zero.
	if (*repeat_count_str == '\0') {
		*repeat_count = 0;
	} else {
		char* repeat_count_str_end = NULL;

		*repeat_count = strtoul(repeat_count_str, &repeat_count_str_end, 0);
			
		// the repetition count should end right before the closing bracket
		if(repeat_count_str_end != repeat_end) {
			tr_parser_error(error_out, repeat_count_str,
				            "invalid number of repetitions `%*s`",
					        (int)(repeat_end - repeat_count_str),
							repeat_count_str);

			*repeat_count = 0;
			return INVALID_CHAR;
		}
	}

	*str = repeat_end + 1;
	return c;
}

wctype_t tr_parser_try_parse_class(const char **str,
	                               tr_parser_error_t* error_out)
{
	const char* str_pos;
	
	const char* class_name_end;
	char* class_name;
	ptrdiff_t class_name_len;

	wctype_t char_class;

	//

	if(!str || !*str || !out)
		return 0;	

	class_name_end = tr_parser_find_next_token(*str + 1, ":]");
	if(!class_name_end)
		return 0;

	class_name_len = (class_name_end - *str);
	if(class_name_len <= 0) {
		tr_parser_error(error_out, str_pos, "missing character class name");
		return 0;
	}
		
	class_name = tr_strndup(*str, class_name_len);
	if(!class_name) {
		tr_fatal_error("out of memory");
		return 0;
	}
	
	char_class = wctype(class_name);				
	if(!char_class) {
		tr_parser_error(error_out, str_pos, "invalid character class `%s`", class_name);
				
		free(class_name);
		return 0;
	}
	
	free(class_name);

	*str = class_name_end + 2;
	return char_class;
}

int tr_parser_try_parse_equiv(const char **str, tr_parser_error_t* error_out)
{
	const char *equiv_start, *equiv_end, *equiv_char_end;
	int c;

	//

	if(!str || !*str)
		return INVALID_CHAR;	

	equiv_start = *str + 1;

	equiv_end = tr_parser_find_next_token(equiv_start, "=]");
	if(!equiv_end)
		return INVALID_CHAR;

	equiv_char_end = equiv_start;

	c = tr_parser_parse_one_char(&equiv_char_end, error_out);
	if(c == INVALID_CHAR) {
		tr_parser_error(error_out, equiv_start, "missing equivalence class character");
		return INVALID_CHAR;
	}
	
	// there must be one and exactly one valid character in the equivalence
	// class
	if (equiv_char_end != equiv_end) {
		tr_parser_error(error_out, equiv_start,
				        "`%*s` does not represent a single character for an equivalence class",
						(int)(equiv_char_end - equiv_start), equiv_start);
		return INVALID_CHAR;
	}

	*str = equiv_end + 2;
	return c;
}

char_vector_t* tr_parser_parse(const char *str, size_t target_length,
	                           tr_parser_error_t* error_out) {
	char_vector_t *vec;
	const char *str_pos;
	
	int c, last_read_char = INVALID_CHAR;
	int indefinite_repeat_index = -1;

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

	for(str_pos = str; (c = *str_pos) != '\0'; ++str_pos) {
		// each parsing attempt 'continue's if sucessfull, so the final part of
		// the loop body only happens if no special meanings were parsed for
		// the current position.

		// a second temporary position counter for parsing attempts. It gets
		// written back to str_pos on each iteration of not null.
	
		const char *str_pos_tmp = NULL;
		
		// parse a range specificator only if we have a valid character kept
		// from the previous iterations	
		if (c == '-' && last_read_char != INVALID_CHAR) {
			char current;

			str_pos_tmp = str_pos+1;
			
			c = tr_parser_parse_one_char(&str_pos_tmp, error_out);
			if(c != INVALID_CHAR) {
				if(c < last_read_char) {
					tr_parser_error(error_out, str_pos, 
						            "range with end not collating after start: `%c-%c`",
									last_read_char, c);
					break;
				}

				for(current = last_read_char; current <= c; current++) 
					char_vector_append_char(vec, current);

				last_read_char = INVALID_CHAR;

				str_pos = str_pos_tmp;
				continue;
			
			} else if(tr_parser_error_check(error_out)) {
				break;
			}

		} else if (c == '[') {
			str_pos_tmp = str_pos + 1;
			c = *str_pos_tmp;

			if(c == '\0') {
				break;

			// equivalence class
			} else if(c == '=') {
				str_pos_tmp++;
				
				c = tr_parser_try_parse_equiv(&str_pos_tmp, error_out);
				if(c != INVALID_CHAR) {
					tr_char_equiv_expand(vec, c);

					str_pos = str_pos_tmp;
					continue;
				} else if (tr_parser_error_check(error_out)) {
					break;
				}

			} else if(c == ':') {
				wctype_t char_class = tr_parser_try_parse_class(&str_pos_tmp,
					                                            error_out);

				if(char_class) {
					tr_char_class_expand(char_class, vec);

					str_pos = str_pos_tmp;
					continue;
				} else if (tr_parser_error_check(error_out)) {
					break;
				}
			} else {
				size_t repeat_count;
				c = tr_parser_try_parse_repeat(&str_pos_tmp, last_read_char,
					                           &repeat_count, error_out);

				if(c != INVALID_CHAR) {
					if(repeat_count) {
						while(repeat_count--) {
							char_vector_append_char(vec, c);
						}
					} else {
						char_vector_append_char(vec, c);
						indefinite_repeat_index = vec->len - 1;
					}

					str_pos = str_pos_tmp;
					continue;
				} else if (tr_parser_error_check(error_out)) {
					break;
				}
			}
		}

		c = tr_parser_parse_one_char(&str_pos, error_out);
		if(c == INVALID_CHAR)
			break;

		char_vector_append_char(vec, c);
	}

	if(indefinite_repeat_index) {
		char_vector_expand(vec, 







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
			tr_parser_error(error_out, *str, "unterminated escape sequence"); 
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