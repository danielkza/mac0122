#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>

#include "xmalloc.h"
#include "strutils.h"

#include "utils.h"
#include "char_vector.h"
#include "tr_parser.h"
#include "char_classes.h"

#include "tr_funcs.h"

int tr_char_class_expand(char_class_t char_class, char_vector_t* out)
{
	unsigned int ch;
	size_t i = 0;

	if(char_class == CC_INVALID || out == NULL)
		return 0;
	
	for(ch = 0; ch <= UCHAR_MAX; ch++) {
		if(char_class_check(ch, char_class)) {
			char_vector_append_char(out, ch);
			i++;
		}
	}

	return i;
}

int tr_char_equiv_expand(char ch, char_vector_t* out)
{
	if(out == NULL)
		return 0;

	char_vector_append_char(out, ch);

	return 1;
}

int tr_char_range_expand(unsigned char start, unsigned char end,
	                     char_vector_t* out) {
	size_t i;

	if(end < start || out == NULL)
		return 0;

	if(!char_vector_expand(out, out->len + (end - start + 1)))
		return 0;

	for(i = 0; i <= (unsigned)end - start; ++i) {
		out->vector[out->len++] = (start + i);
	}

	return i;
}

int tr_char_repeat_expand(char ch, size_t count, char_vector_t* out) {
	if(!count || out == NULL)
		return 0;

	if(!char_vector_expand(out, out->len + count))
		return 0;

	memset(out->vector + out->len, ch, count);
	out->len += count;

	return count;
}

int tr_char_indef_repeat_expand(char ch, size_t target_len, size_t start_index,
	                            char_vector_t* out)
{
	size_t i, count;

	if(!target_len || out == NULL || out->len >= target_len)
		return 0;

	if(!char_vector_expand(out, target_len))
		return 0;

	count = target_len - out->len;

	// push back the chars that wlll come after the repeat
	for(i = start_index; i < out->len; i++) {
		out->vector[i + count] = out->vector[i];
	}

	memset(out->vector + start_index, ch, count); 
	out->len += count;

	return count;
}

char * tr_char_printable_repr(unsigned char c) {
	// backslash + digits + terminator
	size_t max_len = 1 + OCTAL_LITERAL_MAX_LENGTH + 1; 
	char *ret = (char*)xmalloc(max_len);

	if(isprint(c)) {
		ret[0] = c;
		ret[1] = '\0';
	} else {
		tr_snprintf(ret, max_len, "\\%03o",
			        (unsigned int)c);
	}

	return ret;
}

int tr_char_find_in_set(char ch, const char_vector_t* set) {
	size_t i;
	if(set == NULL)
		return -1;

	for(i = 0; i < set->len; i++) {
		if(ch == set->vector[i]) {
			// set2 is short, so there's no correspondence
			return i;
		}
	}

	return -1;
}

char tr_char_translate(char ch, const char_vector_t* set1,
	                   const char_vector_t* set2, int complement)
{
	size_t idx;
	if(set1 != NULL && set2 != NULL) {
		idx = tr_char_find_in_set(ch, set1);

		// Check if `ch` IS on set1 if complement is false,
		// or if `ch`IS NOT on set1 if complement is true
		if((complement ? idx == -1 : idx != -1)
			&& set2->len > idx)
		{
			return set2->vector[idx];
		}
	}

	return ch;
}