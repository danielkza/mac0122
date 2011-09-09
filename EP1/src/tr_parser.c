#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <limits.h>
#include <stdarg.h>

#include "xmalloc.h"
#include "strutils.h"

#include "utils.h"
#include "char_vector.h"
#include "char_classes.h"
#include "tr_funcs.h"

#include "tr_parser.h"

// ========================================================================= //

static const char* tr_parser_find_next_token(const char *str,
											 const char* token);
static int tr_parser_try_parse_repeat(const char **str,
									  unsigned int *repeat_count,
									  tr_parser_error_t* error_out);
static char_class_t tr_parser_try_parse_class(const char **str,
										      tr_parser_error_t* error_out);
static int tr_parser_try_parse_equiv(const char **str,
									 tr_parser_error_t* error_out);
static int tr_parser_parse_octal_literal(const char **str);
static int tr_parser_parse_one_char(const char **str,
									tr_parser_error_t* error_out);

// ========================================================================= //


void tr_parser_error(tr_parser_error_t* error_out, const char* err_pos,
					 const char* err_fmt, ...)
{
	va_list ap;
	va_start(ap, err_fmt);

	if(error_out != NULL) {
		size_t msg_len;
			
		error_out->err = 1;
			
		msg_len = vsnprintf(NULL, 0, err_fmt, ap);
		if(msg_len != 0) {
			char* msg = (char*)xmalloc(msg_len + 1);
			
			vsnprintf(msg, msg_len + 1, err_fmt, ap);
			error_out->msg = msg;
		}

		if(err_pos != NULL
		   && error_out->input != NULL
		   && err_pos >= error_out->input)
		{
			error_out->err_pos = err_pos - error_out->input;
		}
	}

	va_end(ap);
}

int tr_parser_error_check(tr_parser_error_t* error_out)
{
	return error_out != NULL && error_out->err != 0;
}

void tr_parser_error_reset(tr_parser_error_t* error_out, const char *input)
{
	if(error_out != NULL) {
		error_out->err = 0;

		if(error_out->msg != NULL)
			free(error_out->msg);

		error_out->msg = NULL;
		error_out->input = input;
		error_out->err_pos = 0;
	}
}

// ========================================================================= //

static const char* tr_parser_find_next_token(const char* str,
											 const char* token)
{
	const char* token_pos;

	if(token == NULL || *token == '\0')
		return NULL;

	token_pos = str;
	while(token_pos != NULL) {
		token_pos = strstr(token_pos, token);
		
		if(token_pos != NULL) {
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


static int tr_parser_try_parse_repeat(const char **str,
									  unsigned int *repeat_count,
									  tr_parser_error_t* error_out)
{
	const char *repeat_start, *repeat_marker, *repeat_end;
	char *repeat_count_str;

	int c;

	if(str == NULL || *str == NULL)
		return INVALID_CHAR;

	// check for a valid repeat structure

	repeat_marker = tr_parser_find_next_token(*str, "*");
	if(repeat_marker == NULL)
		return INVALID_CHAR;

	repeat_end = tr_parser_find_next_token(repeat_marker + 1, "]");
	if(repeat_end == NULL)
		return INVALID_CHAR;
		
	repeat_start = *str;

	c = tr_parser_parse_one_char(&repeat_start, error_out);
	if(c == INVALID_CHAR) {
		tr_parser_error(error_out, repeat_start,
			            "repetition with no character");
		return INVALID_CHAR;
	}

	// copy 0 or more characters that lie between the asterisk and the closing
	// bracket.

	repeat_count_str = tr_strndup(repeat_marker + 1,
								  repeat_end - (repeat_marker + 1));
	if(repeat_count_str == NULL) {
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
							"invalid number of repetitions `%.*s`",
							(int)(repeat_end - repeat_count_str),
							repeat_count_str);

			*repeat_count = 0;
			return INVALID_CHAR;
		}
	}

	*str = repeat_end + 1;
	return c;
}

static char_class_t tr_parser_try_parse_class(const char **str,
										      tr_parser_error_t* error_out)
{
	const char* class_name_end;
	char* class_name;
	ptrdiff_t class_name_len;

	char_class_t char_class;

	if(str == NULL || *str == NULL)
		return CC_INVALID;	

	class_name_end = tr_parser_find_next_token(*str, ":]");
	if(class_name_end == NULL)
		return CC_INVALID;

	class_name_len = (class_name_end - *str);
	if(class_name_len == 0) {
		tr_parser_error(error_out, *str, "missing character class name");
		return CC_INVALID;
	}
		
	class_name = tr_strndup(*str, class_name_len);
	if(class_name == NULL) {
		tr_fatal_error("out of memory");
		return CC_INVALID;
	}
	
	char_class = char_class_get(class_name);				
	if(char_class == CC_INVALID) {
		tr_parser_error(error_out, *str, "invalid character class `%s`",
			            class_name);
				
		free(class_name);
		return CC_INVALID;
	}
	
	free(class_name);

	*str = class_name_end + 2;
	return char_class;
}

static int tr_parser_try_parse_equiv(const char **str,
									 tr_parser_error_t* error_out)
{
	const char *equiv_start, *equiv_end, *equiv_char_end;
	int c;


	if(str == NULL || *str == NULL)
		return INVALID_CHAR;	

	equiv_start = *str;

	equiv_end = tr_parser_find_next_token(equiv_start, "=]");
	if(equiv_end == NULL)
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
						"`%.*s` does not represent a single character for an equivalence class",
						(int)(equiv_end - equiv_start), equiv_start);
		return INVALID_CHAR;
	}

	*str = equiv_end + 2;
	return c;
}

/* Interprets an espace sequence following a backslash into the correct
 * character.
 *
 * The passed string must start right *after* the backslash.
 *
 * If out is not NULL, after the function returns its pointee will be set to
 * the address of the character directly following the last matched character.
 */

static int tr_parser_parse_octal_literal(const char **str)
{
	const char *s;
	char *copy, *copy_end;
	size_t len = 0;
	unsigned int value;
	
	if(str == NULL || *str == NULL)
		return INVALID_CHAR;

	// Manually count the number of octal numerals.
	// That is needed because strtoul skips whitespace, which we must not do.				
	for(s = *str; *s != '\0' && *s >= '0' && *s <= '7'; s++)
		len++;

	if(len == 0)
		return INVALID_CHAR;

	copy = tr_strndup(*str, len);
	if(copy == NULL)
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
static int tr_parser_parse_one_char(const char **str,
									tr_parser_error_t* error_out)
{
	const char char_escape_table[][2] = {
		{'a', '\a'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'},
		{'r', '\r'}, {'t', '\t'}, {'v', '\v'}, {'\\', '\\'}
	};
	
	int c;

	if(str == NULL || *str == NULL)
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

// ========================================================================= //

char_vector_t* tr_parser_parse(const char *str, size_t target_length,
							   tr_parser_error_t* error_out)
{
	char_vector_t *vec;
	const char *str_pos;
	
	int c;

	int indef_repeat_char = INVALID_CHAR;
	size_t indef_repeat_index = 0;

	if(str == NULL || *str == '\0')
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
	if(vec == NULL)
		return NULL;

	tr_parser_error_reset(error_out, str);

	for(str_pos = str; (c = *str_pos) != '\0'; ) {
		const char *str_pos_tmp = NULL;
			
		// try to parse one of the multiple meanings a open bracket may have
		// first
		if (c == '[') {
			str_pos_tmp = str_pos + 1;
			c = *str_pos_tmp;

			// try to parse an equivalence class
			if(c == '=') {
				str_pos_tmp++;
				
				c = tr_parser_try_parse_equiv(&str_pos_tmp, error_out);
				if(c != INVALID_CHAR) {
					tr_char_equiv_expand(c, vec);

					str_pos = str_pos_tmp;
					continue;
				} else if (tr_parser_error_check(error_out)) {
					break;
				}
			// try a character class
			} else if(c == ':') {
				char_class_t char_class;
				
				str_pos_tmp++;
				char_class = tr_parser_try_parse_class(&str_pos_tmp,
					                                   error_out);

				if(char_class != CC_INVALID) {
					tr_char_class_expand(char_class, vec);

					str_pos = str_pos_tmp;
					continue;
				} else if (tr_parser_error_check(error_out)) {
					break;
				}
			// try a character repeat
			} else {
				size_t repeat_count;
				c = tr_parser_try_parse_repeat(&str_pos_tmp, &repeat_count,
					                           error_out);

				if(c != INVALID_CHAR) {
					if(repeat_count) {
						tr_char_repeat_expand(c, repeat_count, vec);

					} else if (!target_length) {
						tr_parser_error(error_out, str_pos_tmp, 
										"repetitions of indefinite length are "
										"not valid in set1");
						break;

					} else if(indef_repeat_char != INVALID_CHAR) {
						tr_parser_error(error_out, str_pos_tmp, 
										"only one repetition of indefinite "
										"length is allowed on a set");
						break;

					} else {
						indef_repeat_char = c;
						indef_repeat_index = vec->len;
					}

					str_pos = str_pos_tmp;
					continue;
				} else if (tr_parser_error_check(error_out)) {
					break;
				}
			}
		}

		// no special meaning from brackets found. From now we may have a 
		// simple character, that may or may not be followed by a ranged
		// indicator and the range end.
		c = tr_parser_parse_one_char(&str_pos, error_out);
		if(c == INVALID_CHAR)
			break;

		// check for the range indicator, and only take action if it's not
		// followed by the end of the string.
		if(*str_pos == '-' && *(str_pos+1) != '\0') {
			int start = c, end;

			str_pos_tmp = str_pos + 1;
			
			end = tr_parser_parse_one_char(&str_pos_tmp, error_out);
			if(end != INVALID_CHAR) {
				if(end < start) {
					char *start_printable = tr_char_printable_repr(start),
						 *end_printable   = tr_char_printable_repr(end);
				
					tr_parser_error(error_out, str_pos,
									"range end does not collate after range "
									"start: `%s-%s`",
									start_printable, end_printable);

					free(start_printable);
					free(end_printable);

					break;
				}

				tr_char_range_expand(start, end, vec);

				str_pos = str_pos_tmp;
				continue;
			} else if(tr_parser_error_check(error_out)) {
				break;
			}
		}

		// no range matched, we can append the char to the set now.
		char_vector_append_char(vec, c);
	}

	// We broke out of the loop due to an error, free the allocated vector.
	if(tr_parser_error_check(error_out)) {
		char_vector_free(vec);
		return NULL;
	}

	// If there's an indefinite repetition, it will expand to fill the whole
	// target length.
	if(indef_repeat_char != INVALID_CHAR ) {
		tr_char_indef_repeat_expand(indef_repeat_char, target_length,
									indef_repeat_index, vec);

	// Otherwise, we should repeat the last character of the set until it
	// reaches the target length.
	} else if(target_length && vec->len && vec->len < target_length) {
		char last_char = vec->vector[vec->len - 1];
		size_t i;

		for(i = vec->len; i < target_length; i++) {
			vec->vector[i] = last_char;
		}
	}
	
	return vec;
}