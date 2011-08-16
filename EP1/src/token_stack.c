#include <stdlib.h>

#include "utils.h"
#include "token_stack.h"

int token_stack_push(token_stack_t** head, token_t* token) {
	token_stack_t* new_node;
	
	if(!head || !token)
		return 0;

	if(*head && !(*head)->token) {
		(*head)->token = token;
		return 1;
	}

	new_node = (token_stack_t*)malloc(sizeof(token_stack_t));
	if(!new_node)
		return 0;

	new_node->token = token;
	new_node->next = *head;

	*head = new_node;

	return 1;
}

token_t* token_stack_pop(token_stack_t** head) {
	token_t *token;
	token_stack_t *next;

	if(!head || !(*head))
		return NULL;

	token = (*head)->token;
	next = (*head)->next;

	free(*head);
	*head = next;

	return token;
}