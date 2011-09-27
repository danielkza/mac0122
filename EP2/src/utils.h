/** @file utils.h
 *
 * General utility functions
 *
 * @author Daniel Miranda (No. USP: 7577406) <danielkza2@gmail.com>
 *         Exercício-Programa 2 - MAC0122 - IME-USP - 2011
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>

/**
 * Iterates through a linked list free()'ing all it's items
 * 
 * @param list The linked list
 * @param type The type of the linked list nodes
 * @param next_member Name of the member of the list node that points to the 
 *                    next item
 */

#define linked_list_free(list, type, next_member) \
{ \
    type *iter__, *next__; \
    for(iter__ = list, next__ = (list != NULL) ? list->next_member : NULL; \
        iter__ != NULL; \
        iter__ = next__, next__ = (next__ != NULL) ? next__->next_member : NULL) \
    { \
        free(iter__); \
    } \
}


/**
 * Macro for iterating through a linked list. Use it where you would include
 * a for loop, following it with an opening bracket, your code, and a matching
 * closing bracket.
 *
 * Example of use:
 *
 * @code
 *
 * typedef struct my_list {
 *     int n;
 *     struct my_list *next;
 * } my_list;
 * 
 * my_list *head, *tail, *list_cur, *list_next;
 * int i;
 * 
 * for(i = 0; i < 100; i++) {
 *     my_list* item = malloc(sizeof(*item));
 *
 *     item->n = i;
 *     item->next = NULL;
 *
 *     if(head == NULL)
 *         head = item;
 *     else if(tail != NULL)
 *         tail->next = item;
 *
 *     tail = item;
 *  }
  
 * linked_list_foreach(head, list_cur, list_next, next) {
 *     printf("%d\n", list_cur->n);
 * }
 *
 * @endcode
 * 
 * @param list        Pointer to the first node of the list
 * @param iter        A variable with the type of a pointer to a list node,
 *                    that will receive the current node during the iteration
 * @param iter_next   An auxiliary variable with the type of a pointer to a
 *                    list node that will hold a pointer to the next item to be
 *                    visited during the iteration
 * @param next_member Name of the member of the list node type that holds a
 *                    a pointer to the next item
 */
#define linked_list_foreach(list, iter, iter_next, next_member) \
    for(iter = list, iter_next = (list != NULL) ? list->next_member \
                                                : NULL; \
        iter != NULL; \
        iter = iter_next, iter_next = (iter_next != NULL) ? \
                                      iter_next->next_member : NULL \
    )

#endif
