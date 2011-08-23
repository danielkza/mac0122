#ifndef TR_CHAR_CLASSES_H
#define TR_CHAR_CLASSES_H

#include <ctype.h>

#include "isblank.h"

#include "char_vector.h"

int tr_char_class_expand(wctype_t type, char_vector_t* out);
	
#endif // #ifndef TR_CHAR_CLASSES_H
