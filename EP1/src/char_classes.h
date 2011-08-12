#ifndef TR_CHAR_CLASSES_H
#define TR_CHAR_CLASSES_H

#include <ctype.h>

#include "isblank.h"

#include "tr_char_list.h"

enum {
	CHAR_CLASS_EXPAND_SUCCESS       = 0,
	CHAR_CLASS_EXPAND_INVAL         = -1,
	CHAR_CLASS_EXPAND_BUF_TOO_SMALL = -2
};

typedef int (*tr_char_class_func_t)(int);

typedef struct tr_char_class_t {
	char*                name;
	tr_char_class_func_t func;
} tr_char_class_t;


const tr_char_class_t* tr_char_class_get_by_name(const char* name);
int tr_char_class_expand(const tr_char_class_t* chclass, char* buf, int* len);

	
#endif // #ifndef TR_CHAR_CLASSES_H
