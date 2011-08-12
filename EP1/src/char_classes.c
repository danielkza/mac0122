#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "utils.h"

#include "char_classes.h"

// These stubs are needed because we cannot take the adddress of library functions directly.

int tr_isalnum(int ch) { return isalnum(ch); }
int tr_isalpha(int ch) { return isalpha(ch); }
int tr_isblank(int ch) { return isblank(ch); }
int tr_iscntrl(int ch) { return iscntrl(ch); }
int tr_isdigit(int ch) { return isdigit(ch); }
int tr_isgraph(int ch) { return isgraph(ch); }
int tr_islower(int ch) { return islower(ch); }
int tr_isprint(int ch) { return isprint(ch); }
int tr_ispunct(int ch) { return ispunct(ch); }
int tr_isspace(int ch) { return isspace(ch); }
int tr_isupper(int ch) { return isupper(ch); }
int tr_isxdigit(int ch) { return isxdigit(ch); }

#define CHAR_CLASS(name) { #name, tr_is##name }

static const tr_char_class_t tr_char_classes[] = {
	CHAR_CLASS(alnum),
	CHAR_CLASS(alpha),
	CHAR_CLASS(blank),
	CHAR_CLASS(cntrl),
	CHAR_CLASS(digit),
	CHAR_CLASS(graph),
	CHAR_CLASS(lower),
	CHAR_CLASS(print),
	CHAR_CLASS(punct),
	CHAR_CLASS(space),
	CHAR_CLASS(upper),
	CHAR_CLASS(xdigit)
};

char* tr_char_class_expand(const tr_char_class_t* chclass, int *len) {
	char *buf, *smaller_buf;
	int ch, i;

	if(chclass == NULL || chclass->func == NULL || !len)
		return NULL;

	buf = (char*)malloc((CHAR_MAX - CHAR_MIN + 1) * sizeof(char));
	if(!buf)
		return NULL;

	i = 0;
	for(ch = '\0'; ch <= CHAR_MAX; ch++) {
		if((chclass->func)(ch))
			buf[i++] = (char)ch;
	}

	smaller_buf = (char*)realloc(buf, i * sizeof(char));
	if(smaller_buf)
		buf = smaller_buf;

	*len = i;
	return buf;
}

const tr_char_class_t* tr_char_class_get_by_name(const char* name) {
	int i;
	
	if(!name)
		return NULL;

	for(i = 0; i < ARRAY_SIZE(tr_char_classes); i++) {
		const tr_char_class_t* chclass = &tr_char_classes[i];
		if(stricmp(name, chclass->name) == 0)
			return chclass;
	}

	return NULL;
}