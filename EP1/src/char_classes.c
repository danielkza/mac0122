#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "utils.h"
#include "char_classes.h"

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L

int isblank(int c) {
	return c == ' ' || c == '\t';
}

#endif

char_class_t char_class_get(const char* class_name) {
	static const struct {
		const char   *name;
		char_class_t char_class;
	} char_classes[] = {
		{"alnum", CC_ALNUM},
		{"alpha", CC_ALPHA},
		{"blank", CC_BLANK},
		{"cntrl", CC_CNTRL},
		{"digit", CC_DIGIT},
		{"graph", CC_GRAPH},
		{"lower", CC_LOWER},
		{"print", CC_PRINT},
		{"punct", CC_PUNCT},
		{"space", CC_SPACE},
		{"upper", CC_UPPER},
		{"xdigit", CC_XDIGIT},
	};

	size_t i;

	if(!class_name)
		return CC_INVALID;

	for(i = 0; i < ARRAY_SIZE(char_classes); i++) {
		if(strcmp(char_classes[i].name, class_name) == 0) {
			return char_classes[i].char_class;
		}
	}

	return CC_INVALID;	
}

int char_class_check(int c, char_class_t char_class) {
	assert((unsigned)c <= UCHAR_MAX);
	
	switch(char_class) {
	case CC_ALNUM:
		return isalnum(c);  
	case CC_ALPHA:
		return isalpha(c);
	case CC_BLANK:
		return isblank(c);
	case CC_CNTRL:
		return iscntrl(c);
	case CC_DIGIT:
		return isdigit(c);
	case CC_GRAPH:
		return isgraph(c);
	case CC_LOWER:
		return islower(c);
	case CC_PRINT:
		return isprint(c);
	case CC_PUNCT:
		return ispunct(c);
	case CC_SPACE:
		return isspace(c);
	case CC_UPPER:
		return isupper(c);
	case CC_XDIGIT:
		return isxdigit(c);
	}

	return 0;
}