#ifndef TR_CHAR_LIST_H
#define TR_CHAR_LIST_H

typedef struct tr_char_list_t {
	char*           list;
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