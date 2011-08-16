#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "char_utils.h"

/*
 * Fills a character buffer sequentially with all characters starting from 'start',
 * ranging up to 'end' or until 'maxlen' characters have been written.
 * No null terminator is appended to the buffer.
 *
 * If 'start's numerical value is higher than 'end's, no characters will be written.
 *
 * The return value is the number of characters effectively written to the buffer.
 */

int string_fill_char_range(char* buf, int maxlen, char start, char end) {
	int i, len;
	char c;

	len = (end - start) + 1;

	if(maxlen < len)
		len = maxlen;

	if(len <= 0 || !buf)
		return 0;

	c = start;
	for(i = 0; i < len; i++)
		buf[i] = c++;
	
	return i;
}