#ifndef TR_CHAR_CLASSES_H
#define TR_CHAR_CLASSES_H

#include <ctype.h>

#include "isblank.h"

#include "char_vector.h"

typedef int (*tr_char_class_func_t)(int);

typedef struct tr_char_class_t {
	char*                name;
	tr_char_class_func_t func;
} tr_char_class_t;

typedef struct tr_char_repetition_t {
	char c;
	int count;
} tr_char_repetition_t;

typedef struct tr_char_range_t {
	char start;
	char end;
} tr_char_range_t;

const tr_char_class_t* tr_char_class_get_by_name(const char* name);
int tr_char_class_expand(const tr_char_class_t* chclass, char_vector_t* out);

	
#endif // #ifndef TR_CHAR_CLASSES_H
