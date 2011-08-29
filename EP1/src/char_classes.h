#ifndef TR_CHAR_CLASSES_H
#define TR_CHAR_CLASSES_H

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L

int isblank(int c);

#endif

typedef enum {
	CC_INVALID = 0,
	CC_ALNUM, CC_ALPHA, CC_BLANK, CC_CNTRL,
	CC_DIGIT, CC_GRAPH, CC_LOWER, CC_PRINT,
	CC_PUNCT, CC_SPACE, CC_UPPER, CC_XDIGIT,
} char_class_t;

char_class_t char_class_get(const char* class_name);
int char_class_check(int c, char_class_t char_class);

#endif // #ifndef TR_CHAR_CLASSES_H