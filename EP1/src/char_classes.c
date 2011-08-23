#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <limits.h>

#include "utils.h"
#include "char_vector.h"

#include "char_classes.h"

// These stubs are needed because we cannot take the adddress of library functions directly.

int tr_char_class_expand(wctype_t type, char_vector_t* out) {
	wint_t ch;
	int i;
	char_vector_t* out;

	if(!type)
		return 0;
	
	for(ch = CHAR_MIN; ch <= CHAR_MAX; ch++) {
		if(iswctype(ch, type)) {
			char_vector_append_char(out, (char)ch);
			i++;
		}
	}

	return i;
}