#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <search.h>
#include <string.h>

#include "utils.h"
#include "char_vector.h"
#include "tr_parser.h"

#include "char_utils.h"


size_t tr_strnlen(const char *str, size_t len)
{
	const char *str_end = (const char *)memchr(str, '\0', len);
	if(str_end)
		len = (str_end - str);

	return len;
}

char * tr_strnchr(const char *str, int ch, size_t len)
{
	assert(ch >= CHAR_MIN && ch <= CHAR_MAX);

	return (char *)memchr(str, (char)ch, len);
}

char * tr_strndup(const char *str, size_t len)
{
	char *out = NULL;
	if(str) {
		len = strnlen(str, len);

		out = (char*)malloc(sizeof(char) * (len + 1));
		if(out) {
			memcpy(out, str, len);
			out[len] = '\0';
		}
	}

	return out;
}

char * tr_strnchrdup(const char *str, int ch, size_t len)
{
	char *out = NULL;
	const char* str_end = tr_strnchr(str, ch, len);

	if(str_end) {
		out = (char*)malloc(sizeof(char) * (len + 1));
		if(out) {
			// no need for further checks, we can just use memcpy
			memcpy(out, str, len);
			out[len] = '\0';
		}
	}

	return out;
}

int charcmpcoll(char a, char b)
{
	char aa[2] = {0, 0}, bb[2] = {0, 0};
	aa[0] = a;
	bb[0] = b;

	return strcoll(aa, bb);
}

int compare_char_coll_sort(const void* a, const void* b)
{
	if(a && b) {
		return compare_char_coll(*((char*)a), *((char*)b));
	}

	return 0;
}

char_vector_t* create_collation_table() {
	char_vector_t* vec = char_vector_new(UCHAR_MAX+1);
	int c;

	if(vec == NULL)
		return NULL;

	for(c = 1; c <= UCHAR_MAX; ++c)
		char_vector_append_char(vec, c);

	qsort(vec->vector, UCHAR_MAX, sizeof(char), 

}




int tr_char_range_expand(char_vector_t* vec, int start, int end,
	                     tr_parser_error_t* error_out)
{
	size_t i;
	char* start_pos = NULL, end_pos = NULL;
	const char* collation_table;
	
	if(!vec)
		return 0;

	collation_table = get_collation_table();
	if(!collation_table)
		return 0;

	start_pos = memchr(collation_table, start, sizeof(collation_table));
	end_pos = memchr(collation_table, end, sizeof(collation_table));

	if(!start_pos || !end_pos) {
		tr_parser_error(error_out, NULL, "range start or end not found on collation", NULL,
		                error_out);
		return 0;
	} else if (end_pos < start_pos) {
		tr_parser_error("range start does not collate before range end", NULL,
		                error_out);
		return 0;
	}

	do {
		char_vector_append_char(vec, *(start_pos++));
		i++;
	} while(start_pos <= end_pos);

	return i;
}





	


