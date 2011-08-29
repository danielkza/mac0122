#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "strutils.h"

#ifndef POSIX

size_t tr_strnlen(const char *str, size_t len)
{
	const char *str_end;
	
	assert(str != NULL);	
	
	str_end = (const char *)memchr(str, '\0', len);
	if(str_end)
		len = (str_end - str);

	return len;
}

char * tr_strnchr(const char *str, size_t len, int ch)
{
	size_t orig_len = len;

	assert(str != NULL);
	assert(ch >= CHAR_MIN && ch <= CHAR_MAX);


	// include the terminating null in the search if max. length not reached.
	len = strnlen(str, len);
	if(len < orig_len)
		len++;

	return (char *)memchr(str, (char)ch, len);
}

char * tr_strndup(const char *str, size_t len)
{
	char *out = NULL;
	if(str != NULL) {
		len = strnlen(str, len);

		out = (char*)malloc(len + 1);
		if(out != NULL) {
			memcpy(out, str, len);
			out[len] = '\0';
		}
	}

	return out;
}

#endif // #ifndef POSIX

#if !defined(POSIX) && __STDC_VERSION__ < 199901L

int tr_snprintf(char *str, size_t size, const char *format, ...)
{
	int result;
	va_list ap;

	va_start(ap, format);

	result = vsnprintf(str, size, format, ap);
	
	va_end(ap);
	
	return result;
}

#endif // #if !defined(POSIX) && __STDC_VERSION__ < 199901L
