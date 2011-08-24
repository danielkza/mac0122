#ifndef TR_CHAR_LIST_H
#define TR_CHAR_LIST_H

#include "char_vector.h"

typedef enum {
	TR_CHAR_LIST_ENTRY_CHARS,
	TR_CHAR_LIST_ENTRY_REPETITION,
	TR_CHAR_LIST_ENTRY_CLASS,
	TR_CHAR_LIST_ENTRY_EQUIV
} tr_char_list_entry_type_t;

typedef struct {
	char value;
	size_t count;
} tr_char_repetition_t;

typedef struct {
	char value;
} tr_char_equivalence_t;

typedef struct tr_char_list_entry_t {
	tr_char_list_entry_type_t type;
	
	union {
		char_vector_t *chars;
		tr_char_repetition_t* repeat;
		tr_char_equivalence_t* equiv;
	}
};

typedef struct tr_char_list_t {
	tr_char_list_entry_t* entry;
	struct tr_char_list_t* next;
} tr_char_list_t;


tr_char_list_t* tr_char_list_new(char* list);
tr_char_list_t* tr_char_list_insert(tr_char_list_t* node, char* list);
void            tr_char_list_remove_next(tr_char_list_t* first);
void            tr_char_list_remove_free_next(tr_char_list_t* first);
//void            tr_char_list_remove_all(tr_char_list_t* first);
//void            tr_char_list_remove_free_all(tr_char_list_t* first);
tr_char_list_t* tr_char_list_get_by_index(tr_char_list_t* head, int index);

#endif // #ifndef TR_CHAR_LIST_H