#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "char_classes.h"
#include "tr_parser.h"

/* Interprets an espace sequence following a backslash into the correct character.
 *
 * The passed string must start right *after* the backslash.
 *
 * If out is not NULL, the address pointed by it will contain the address of the
 * character direct following the last matched character after the function returns;
 */

char tr_parser_parse_char_list(const char* in, size_t buf_initial_size) {
	char *buf = NULL;
	size_t buf_size = 0, buf_len = 0;

	const char *pos, *new_pos;
	char c;

	if(!in || *in == '\0')
		return;
	
	if(buf_initial_size <= 0)
		buf_initial_size = TR_PARSER_CHAR_LIST_BUF_INITIAL_SIZE;

	for(pos = in; *pos != '\0'; pos++) {
		char c, next;


		if(buf_size <= buf_len) {
			buf_size = buf_size ? buf_size * 2
				                : buf_initial_size;

			buf = (char*)realloc(buf, buf_size * sizeof(char));
		}

		c = *pos;
		if(c == '\0')
			break;

		next = *(pos+1);

		if(c == '\\') {
			pos++;

			// try to parse backslash sequence, otherwise just use the next character unchanged
			c = tr_parser_parse_backslash_seq(pos, new_pos);
			if(pos == new_pos)
				c = *pos;

			buf[buf_len++] = c;
			pos++;
		} else if (strncmp(pos, "[:", 2) == 0) {
			// try to capture a character class first
			const char *class_start; int class_len;
			if(next == ':') {
				class_start = 





char tr_parser_parse_backslash_seq(const char *in, const char **out) {
	const char char_table[][2] = {
		{'a', '\a'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'},
		{'r', '\r'}, {'t', '\t'}, {'v', '\v'}, {'\\', '\\'}
	};
	
	char *pos, c = *in;
	char octal_val[OCTAL_LITERAL_MAX_LENGTH+1];
	int i, octal_num;

	// if the string ended, just return a backslash
	if(c == '\0') {
		*out = in;
		return '\\'; 
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
		octal_num = strtol(octal_val, &pos, 8);
		if(octal_val == pos)
			// strtol failed
			break;
		else if(octal_num > UCHAR_MAX)
			// literal too large, try with one fewer character
			*(pos-1) = '\0';
		else {
			// all good
			*out = in + (pos - octal_val);
			return octal_num;
		}
	}
	while(octal_val[0] != '\0');

	// no match found
	*out = in;
	return 0;
}