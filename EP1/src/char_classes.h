#ifndef TR_CHAR_CLASSES_H
#define TR_CHAR_CLASSES_H

#include <ctype.h>

#include "isblank.h"

#include "tr_char_list.h"

typedef int (*tr_char_class_func_t)(int);

typedef struct tr_char_class_t {
	char*                name;
	tr_char_class_func_t func;
} tr_char_class_t;


const tr_char_class_t* tr_char_class_get_by_name(const char* name);
char* tr_char_class_expand(const tr_char_class_t* chclass, int* len);

	
#endif // #ifndef TR_CHAR_CLASSES_H
