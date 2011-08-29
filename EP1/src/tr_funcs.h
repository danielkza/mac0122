#ifndef TR_TR_FUNCS_H
#define TR_TR_FUNCS_H

#include <stddef.h>
#include <wctype.h>

#include "char_vector.h"
#include "char_classes.h"

int tr_char_class_expand(char_class_t type, char_vector_t* out);
int tr_char_equiv_expand(char ch, char_vector_t* out);
int tr_char_range_expand(unsigned char start, unsigned char end,
	                     char_vector_t* out);
int tr_char_repeat_expand(char ch, size_t count, char_vector_t* out);
int tr_char_indef_repeat_expand(char ch, size_t target_len, size_t start_index,
	                            char_vector_t* out);

char* tr_char_printable_repr(unsigned char c);

char tr_char_translate(char ch, const char_vector_t* set1,
	                   const char_vector_t* set2, int complement);
int tr_char_find_in_set(char ch, const char_vector_t* set, size_t *idx);

#endif // #ifndef TR_TR_FUNCS_H