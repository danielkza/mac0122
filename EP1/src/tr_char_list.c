#include <stdlib.h>
#include <string.h>

#include "tr_char_list.h"

tr_char_list_t* tr_char_list_new(char* list) {
	tr_char_list_t* head = (tr_char_list_t*)malloc(sizeof(tr_char_list_t));
	if(!head)
		return NULL;

	head->list = list;
	head->next = NULL;

	return head;
}

tr_char_list_t* tr_char_list_insert(tr_char_list_t* node, char* list) {
	tr_char_list_t* new_node;
	if(node == NULL)
		return NULL;

	new_node = tr_char_list_new(list);
	if(new_node == NULL)
		return NULL;

	if(node->next)
		new_node->next = node->next;

	node->next = new_node;
	return new_node;
}

void tr_char_list_remove_next(tr_char_list_t* first) {
	tr_char_list_t *second, *third;
	if(!first)
		return;

	second = first->next;
	if(!second)
		return;
	
	third = second->next;
	if(third)
		first->next = third;
	
	free(second);
}

void tr_char_list_remove_free_next(tr_char_list_t* first) {
	tr_char_list_t *second, *third;
	if(!first)
		return;
	
	second = first->next;
	if(!second)
		return;

	third = second->next;
	if(third)
		first->next = third;
	
	if(second->list)
		free(second->list);

	free(second);
}

tr_char_list_t* tr_char_list_get_by_index(tr_char_list_t* head, int index) {
	tr_char_list_t* current = head;
	int i;

	if(index < 0 || current == NULL)
		return NULL;

	for(i = 0; i < index; i++) {
		if(current->next)
			current = current->next;
		else
			return NULL;
	}

	return current;
}