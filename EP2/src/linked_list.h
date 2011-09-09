/*! @file linked_list.h
 *
 * A circular, doubly-linked list datatype implementation you can include in
 * other structs for use with any kind of data.
 *
 * Functions for initialization, search, insertion, removal and sorting of
 * items are included, along with macros for iteration.
 *
 * @author Daniel Miranda
 *
 * @copyright Copyright (c) 2011 Daniel Miranda. All Rights Reserved.
 *            Licensed under the GNU General Public License, version 2.
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

/**
 * A circular, doubly-linked list struct you can insert into your
 * own structs, and therefore use with whatever type of data you want.
 *
 * Initialize the head with linked_list_init()
 *
 * @warning It <em>must</em> be the first element of its containing struct.
 */

typedef struct linked_list {
    /// Pointer to the previous item on the list
    struct linked_list *prev;

    /// Pointer to the next item on the list
    struct linked_list *next;
} linked_list;

/**
 * Initializes a linked list head for a circular list. The head will then point
 * to itself.
 *
 * @param head Pointer to the head to initalize
 */

static void
linked_list_init(linked_list* head)
{
    if(head == NULL)
        return;

    head->next = head;
    head->prev = head;
}

/**
 * Finds the front (first item after the head) of a linked list.
 *
 * @param head Pointer to the head of a linked list
 *
 * @return Pointer to the front item or a null pointer on failure
 */

static linked_list*
linked_list_front(linked_list* head)
{
    linked_list* front;
    if(head == NULL)
        return NULL;

    front = head->next;
    if(front == NULL || front == head)
        return NULL;

    return front;
}

/**
 * Finds the tail (last item after the head) of a linked list.
 *
 * @param head Pointer to the head of a linked list
 *
 * @return Pointer to the tail item or a null pointer on failure
 */

static linked_list*
linked_list_tail(linked_list* head)
{
    linked_list* tail;
    if(head == NULL)
        return NULL;

    tail = head->prev;
    if(tail == NULL || tail == head)
        return NULL;

    return tail;
}

/**
 * Retrieves the numerical 0-based index of an item on a list.
 *
 * @param head Pointer to the head of the list
 * @param node Pointer to the item whose index is to be retrieved
 *
 * @return The numerical index, or 0 on failure.
 */

static size_t
linked_list_item_index(linked_list* node,
                       linked_list* head)
{
    size_t index = 0;
    linked_list* cur;
    
    if(node == NULL || head == NULL || node == head)
        return 0;

    for(cur = head->next;
        cur != NULL && cur != head;
        cur = cur->next)
    {
        if(cur == node)
            break;
        index++;
    }

    return index;
}

/**
 * Determines if a list is empty.
 *
 * @param head Pointer to the head of a linked list
 *
 * @return 1 if the list has no items, 0 otherwise
 */

static int
linked_list_empty(linked_list *head)
{
    return head == NULL || head->next == head;
}

/**
 * Determines the number of items contained on a list.
 *
 * @param head Pointer to the head of a linked list
 *
 * @return Count of items in the list, or 0 on failure
 */

static size_t
linked_list_length(linked_list* head)
{
    size_t size = 0;
    linked_list *cur;

    if(head == NULL)
        return 0;

    for(cur = head->next;
        cur != NULL && cur != head;
        cur = cur->next)
    {
        size++;
    }
       
    return size;
}

/**
 * Inserts an item between two existing items on a list.
 *
 * If the two items do not directly follow each other, other items between them
 * will possibly leak. Be sure to take care of them if that's what is actually
 * intended.
 *
 * @param node Pointer to the item to insert
 * @param prev Pointer to the item that will precede the inserted item on the
 *             list order
 * @param next Pointer to the item that will follow the inserted item on the
 *             list order
 */

static void
linked_list_insert(linked_list* node,
                   linked_list* prev,
                   linked_list* next)
{
    if(node == NULL || prev == NULL || next == NULL)
        return;

    node->next = next;
    node->prev = prev;

    prev->next = node;
    next->prev = node;
}

/**
 * Removes an item from a linked list.
 *
 * Removing the head is an invalid operation, no guarantees are made if you
 * pass it to this function.
 *
 * @param node Pointer to the item to remove
 *
 * @return Pointer to the removed item or a null pointer on failure
 */

static linked_list*
linked_list_remove(linked_list* node)
{
    linked_list *next, *prev;
    if(node == NULL)
        return NULL;

    next = node->next;
    prev = node->prev;

    // Don't remove the head
    if(node == next || node == prev)
        return NULL;

    next->prev = prev;
    prev->next = next;

    return node;
}

/**
 * Inserts an item after an existing item on a list.
 *
 * @param node Pointer to the item to insert
 * @param prev Pointer to the item that will precede the inserted item on the
 *             list order
 */

static void
linked_list_insert_after(linked_list* node,
                         linked_list* prev)
{
    if(node == NULL || prev == NULL)
        return;

    linked_list_insert(node, prev, prev->next);
}

/**
 * Inserts an item before an existing item on a list.
 *
 * @param node Pointer to the item to insert
 * @param next Pointer to the item that will follow the inserted item on the
 *             list order
 */

static void
linked_list_insert_before(linked_list* node,
                          linked_list* next)
{
    if(node == NULL || next == NULL)
        return;

    linked_list_insert(node, next->prev, next);
}

/**
 * Inserts an item after all existing items on a list.
 *
 * @param node Pointer to the item to insert
 * @param head Pointer to the head of the list
 */

static void
linked_list_push(linked_list* node,
                 linked_list* head)
{
    if(node == NULL || head == NULL)
        return;
    
    linked_list_insert_before(node, head);
}

/**
 * Inserts an item before all existing items on a list.
 *
 * @param node Pointer to the item to insert
 * @param head Pointer to the head of the list
 */

static void
linked_list_push_front(linked_list* node,
                       linked_list* head)
{
    if(node == NULL || head == NULL)
        return;

    linked_list_insert_after(node, head);
}

/**
 * Removes the last item after the head from the list and returns it.
 *
 * @param head Pointer to the head of the list
 *
 * @return Pointer to the removed item or a null pointer on failure
 */

static linked_list*
linked_list_pop(linked_list* head)
{
    linked_list *tail;
    if(head == NULL)
        return NULL;

    tail = linked_list_tail(head);
    // Don't pop the head
    if(tail == NULL)
        return NULL;

    return linked_list_remove(tail);
}

/**
 * Removes the first item after the head from the list and returns it.
 *
 * @param head Pointer to the head of the list
 *
 * @return Pointer to the removed item or a null pointer on failure
 */

static linked_list*
linked_list_pop_front(linked_list* head)
{
    linked_list *front;
    if(head == NULL)
        return NULL;

    front = linked_list_front(head);
    // Don't pop the head
    if(front == NULL)
        return NULL;

    return linked_list_remove(front);
}

/**
 * Swaps the positions of two items on a list.
 *
 * @param a Item to swap
 * @param b Other item to swap
 */

static void
linked_list_swap(linked_list* a,
                 linked_list* b)
{
    linked_list *a_prev, *a_next;
    if(a == NULL || b == NULL || a == b)
        return;

    a_prev = a->prev;
    a_next = a->next;

    a->prev = b->prev;
    a->next = b->next;
    
    b->prev = a_prev;
    b->next = a_next;
}

/**
 * Moves a section of the list to a different position.
 *
 * @param start Pointer to the first item (<em>not the head</em>) of the section to be
 *              moved
 * @param end   Pointer to the item <em>past</em> the last item to be moved
 * @param pos   Pointer to the item that will precede the inserted section on
 *              the new list ordering.
 */

static void
linked_list_move_section_after(linked_list* start,
                               linked_list* end,
                               linked_list* pos)
{
    linked_list *last, *pos_next, *first_prev, *last_next;

    if(start == NULL || end == NULL || pos == NULL)
        return;

    // Moving something after itself or what was just previous to it has no
    // effect
    if(start == pos || start->prev == pos)
        return;

    // The end pointer is actually an item past the last item of the section,
    // to keep consistency with all the other functions (that get much simpler
    // using that).
    last = end->prev;

    // Make copies of some data we'll change but still need later
    first_prev = start->prev;
    last_next = end;
    pos_next = pos->next;

    // Attach the beginning of the section to it's new place
    pos->next = start;
    start->prev = pos;

    // Attach the end of the section to what was previously next to the
    // insertion position
    last->next = pos_next;
    pos_next->prev = last;

    // Reattach the lose ends, between where the section used to be
    first_prev->next = last_next;
    last_next->prev = first_prev;
}

/**
 * @internal
 * 
 * Finds the first item higher than a pivot in a section of the list. Used
 * internally by the merge sort implementation.
 *
 * @param start      Pointer to the first item (<em>not the head</em>) of the section
 * @param end        Pointer to the item <em>past</em> the last item of the section
 * @param comparator Pointer to a comparator function, which determines the
 *                   ordering of items
 * @param pivot      Pointer to the pivot item
 *
 * @return Pointer to the item. If not higher items are found, `end` is
 *         returned, or a null pointer on error.
 */
static linked_list*
linked_list_merge_sorted_find_upper__(linked_list* start,
                                      linked_list* end,
                                      int (*comparator)(const void*, const void*),
                                      const void* pivot)
{
    linked_list *cur;
    if(start == NULL || end == NULL)
        return NULL;

    cur = start;
    while((*comparator)((const void*)cur, (const void*)pivot) <= 0) {
        cur = cur->next;
        if(cur == NULL || cur == end)
            break;
    }

    return cur;
}

/**
 * Merges a section of a sorted list into another section of a sorted list,
 * inserting elements at their proper sorted positions.
 *
 * @param half1_start Pointer to pointer to the first item (<em>not the
 *                    head</em>) of a sorted list on which the second list's
 *                    items will be inserted. Since the first item may change
 *                    with the sorting, it will be updated accordingly
 * @param half1_end   Pointer to one item <em>past</em> the end of the first
 *                    list
 * @param half2_start Pointer to the first item (<em>not the head</em>) of a
 *                    second sorted list which will have it's elements
 *                    extracted then inserted into the first list
 * @param half2_end   Pointer to one item <em>past</em> the end of the second
 *                    list
 * @param comparator  Pointer to a comparator function, which determines the
 *                    ordering of items
 */

static void
linked_list_merge_sorted_sections(linked_list** half1_start,
                                  linked_list* half1_end, 
                                  linked_list* half2_start, 
                                  linked_list* half2_end,
                                  int (*comparator)(const void*, const void*))
{
    linked_list *half1_start_prev,
                *half1_cur, *half1_upper,
                *half2_cur, *half2_upper,
                *cur_end;

    if(half1_start == NULL || half1_end == NULL
       || half2_start == NULL || half2_end == NULL)
    {
        return;
    }

    /* Keep a reference to the item before the start because the start may
     * change. We'll use it to refer to the updated start later.
     */
    cur_end = half1_start_prev = (*half1_start)->prev;

    half1_cur = *half1_start;
    half2_cur = half2_start;

    /* We merge the lists by copying sections of the second half into the first
     * half as needed.
     *
     * Starting at the beggining of both halfs, we start by finding all items
     * of the first half lower than the current position of the second half.
     * They don't need to be copied, so we just advance the first half position
     * accordingly.
     *
     * Then we do the same for the second half, but actually moving the items
     * lower than the last checked item on the first half to the proper
     * positions.
     *
     * We repeat that until we totally use up one of the halfs. Then we find
     * which half still has items to be used: if it is the first one, they're
     * already in place. If it is the second, we move them.
     */

    while(half1_cur != half1_end && half2_cur != half2_end) {
        half1_upper =
        linked_list_merge_sorted_find_upper__(half1_cur,
                                              half1_end,
                                              comparator,
                                              (const void*)half2_cur);
        
        if(half1_upper != half1_cur) {          
            half1_cur = half1_upper;            
            cur_end = half1_upper->prev;

            if(half1_cur == half1_end)
                break;
        }
        
        half2_upper =
        linked_list_merge_sorted_find_upper__(half2_cur,
                                              half2_end,
                                              comparator,
                                              (const void*)half1_cur);
        if(half2_upper != half2_cur) {
            linked_list_move_section_after(half2_cur, half2_upper, cur_end);
            
            half2_cur = half2_upper;
            cur_end = half2_upper->prev;
        }
    }

    if(half2_cur != half2_end)
        linked_list_move_section_after(half2_cur, half2_end, cur_end);

    *half1_start = half1_start_prev->next;
}

/**
 * Merges a sorted list into another sorted list, inserting elements at their
 * proper sorted positions.
 *
 * @param head1      Pointer to the head of a sorted list onto which the second
 *                   list's items will be inserted
 * @param head2      Pointer to the head of a second sorted list which will
 *                   have it's elements extracted then inserted into the first
 *                   list
 * @param comparator Pointer to a comparator function, which determines the
 *                   ordering of items
 */

static void
linked_list_merge_sorted(linked_list* head1,
                         linked_list* head2,
                         int (*comparator)(const void*, const void*))
{
    linked_list *front1, *front2;
    if(head1 == NULL || head2 == NULL || comparator == NULL)
        return;

    front1 = linked_list_front(head1);
    front2 = linked_list_front(head2);

    // Merging two empty lists has no effect
    if(front1 == NULL && front2 == NULL) {
        return;
    // If the destination list is empty, merging is equivalent to moving the 
    // whole second list into the first
    } else if(front1 == NULL) {
        linked_list_move_section_after(front2, head2, head1);
    // If the source list is empty, we have nothing to do
    } else if(front2 == NULL) {
        return;
    // None of the lists is empty, actually do the sorted merge
    } else {
        linked_list_merge_sorted_sections(&front1, head1,
                                          front2, head2,
                                          comparator);
    }
}

/**
 * @internal
 *
 * Sorts a (section of a) list according to a comparator function using the
 * merge sort algorithm.
 *
 * @param start      Pointer to pointer to the first item (<em>not the
 *                   head</em>) of the section. Since the start may change with
 *                   the sorting, it will be updated accordingly
 * @param end        Pointer to the item <em>past</em> the last item of the
 *                   section
 * @param comparator Pointer to a comparator function, which determines the
 *                   ordering of items
 */

static void
linked_list_merge_sort__(linked_list** start,
                         linked_list* end,
                         int (*comparator)(const void*, const void*))
{
    linked_list *half1_last, *half1_end,
                *half2_start,
                *start_prev, *new_start;
    
    if(start == NULL || *start == NULL || end == NULL)
        return;

    // A list consisting of zero or one item is always sorted
    if(*start == end || (*start)->next == end) {
        return;
    }

    /* Split the section of the list into two by walking two pointers, one from
     * the start to the end, the other from the end to the start, until they
     * meet.
     */
    half1_last = *start;
    half2_start = end->prev;

    while(half1_last != half2_start->prev) {                
        half1_last = half1_last->next;

        /* The check for the meeting needs to be done before the second half's
         * start is modified or the pointers will walk past each other instead
         * of ending up adjacent as they should. 
         */
        if(half1_last == half2_start->prev)
            break;

        half2_start = half2_start->prev;
    }
    
    /* We have to keep a reference to the item preceding the start because
     * start itself may change positions; we reference the 'new' start
     * indirectly then through start_prev
     */
    start_prev = (*start)->prev;
    half1_end = half1_last->next;
    
    linked_list_merge_sort__(start, half1_end, comparator);
    linked_list_merge_sort__(&half2_start, end, comparator);

    new_start = start_prev->next;
    
    linked_list_merge_sorted_sections(&new_start, half1_end,
                                      half2_start, end,
                                      comparator);

    *start = new_start;

}

/**
 * Sorts a section of a list according to a comparator function.
 *
 * @param start      Pointer to pointer to the first item (<em>not the
 *                   head</em>) of the section. Since the start may change with
 *                   the sorting, it will be updated accordingly
 * @param end        Pointer to the item <em>past</em> the last item of the
 *                   section
 * @param comparator Pointer to a comparator function, which determines the
 *                   ordering of items
 */
static void
linked_list_sort_section(linked_list** start,
                         linked_list* end,
                         int (*comparator)(const void*, const void*))
{
    if(start == NULL || end == NULL || comparator == NULL)
        return;

    linked_list_merge_sort__(start, end, comparator);
}

/**
 * Sorts a list according to a comparator function.
 *
 * @param head      Pointer to the head of the list
 * @param comparator Pointer to a comparator function, which determines the
 *                   ordering of items
 */
static void
linked_list_sort(linked_list* head,
                 int (*comparator)(const void*, const void*))
{
    linked_list *front;
    if(head == NULL)
        return;

    front = linked_list_front(head);
    if(front == NULL)
        return;

    linked_list_merge_sort__(&front, head, comparator);
}

/**
 * Macro for iterating through a linked list. Use it where you would include
 * a for loop, following it with an opening bracket, your code, and a matching
 * closing bracket.
 *
 * @warning It is not safe to remove items from the list using this macro.
 *          Use linked_list_foreach_safe instead if you want to do that.
 *
 * Example of use:
 *
 * @code
 *
 * typedef struct {
 *     linked_list list;
 *     int my_number;
 * } my_data;
 *  
 * my_data head, *iter;
 * size_t i;
 *
 * linked_list_init(&head.list);
 *
 * for(i = 0; i < 100; i++) {
 *     my_data* data = (my_data*)malloc(sizeof *data);
 *     if(data != NULL) {
 *         data->my_number = i;
 *         linked_list_push(&(data->list), &(head.list));
 *     }
 * }
 *
 * linked_list_foreach(iter, &(head.list), my_data) {
 *     printf("%d\n", iter->my_number);
 * }
 *
 * @endcode
 * 
 * @param iter A variable of the same type as the struct containing the
 *             linked_list, that will be the iterator. It will contain a
 *             pointer to the current list item at each iteration.
 * @param head Pointer to the head of the list
 * @param type Type of the struct where the linked_list is contained
 *
 * @see linked_list_foreach_safe
 */

#define linked_list_foreach(iter, head, type) \
    for(iter = (type*)((head)->next); \
        iter != NULL && iter != (type*)(head); \
        iter = (type*)(((linked_list*)iter)->next) \
    )

/**
 * Macro for iterating through a linked list in reverse.
 *
 * @warning It is not safe to remove items from the list using this macro.
 *          Use linked_list_foreach_safe_reverse instead if you want to do
 *          that.
 *
 * @param iter A variable of the same type as the struct containing the
 *             linked_list, that will be the iterator. It will contain a
 *             pointer to the current list item at each iteration.
 * @param head Pointer to the head of the list
 * @param type Type of the struct where the linked_list is contained
 *
 * @see linked_list_foreach
 * @see linked_list_foreach_safe_reverse
 */

#define linked_list_foreach_reverse(iter, head, type) \
    for(iter = (type*)((head)->prev); \
        iter != NULL && iter != (type*)(head); \
        iter = (type*)(((linked_list*)iter)->prev) \
    )


/**
 * Macro for iterating through a linked list allowing the removal of items.
 *
 * @param iter A variable of the same type as the struct containing the
 *             linked_list, that will be the iterator. It will contain a
 *             pointer to the current list item at each iteration
 * @param next Another variable to use as a temporary iterator
 * @param head Pointer to the head of the list
 * @param type Type of the struct where the linked_list is contained
 *
 * @see linked_list_foreach
 */

#define linked_list_foreach_safe(iter, next, head, type) \
    for(iter = (type*)((head)->next), \
        next = (type*)(((linked_list*)iter)->next); \
        \
        iter != NULL && iter != (type*)(head); \
        \
        iter = next, next = (type*)(((linked_list*)iter)->next) \
    )

/**
 * Macro for iterating through a linked list in reverse allowing the removal
 * of items.
 *
 * @param iter A variable of the same type as the struct containing the
 *             linked_list, that will be the iterator. It will contain a
 *             pointer to the current list item at each iteration
 * @param next Another variable to use as a temporary iterator
 * @param head Pointer to the head of the list
 * @param type Type of the struct where the linked_list is contained
 *
 * @see linked_list_foreach
 * @see linked_list_foreach_safe
 */

#define linked_list_foreach_safe_reverse(iter, next, head, type) \
    for(iter = (type*)((head)->prev), \
        next = (type*)(((linked_list*)iter)->prev); \
        \
        iter != NULL && iter != (type*)(head); \
        \
        iter = next, next = (type*)(((linked_list*)iter)->prev) \
    )



/**
 * Walks through a linked list destroying all it's items. If you allocated the
 * head in the heap, you should also free it yourself.
 *
 * @param head Pointer to the head of the list
 * @param type Type of the struct where the linked_list is contained
 * @param func Name of a function that will be called to cleanup each item.
 */

#define linked_list_free_items(head, type, func) \
    { \
        type *iter, *next; \
        for(iter = (type*)((head)->next), \
            next = (type*)(((linked_list*)iter)->next); \
            \
            iter != NULL && iter != (type*)(head); \
            \
            iter = next, next = (type*)(((linked_list*)iter)->next) \
        ) { \
            func(iter); \
        } \
        linked_list_init(head); \
    }

#endif // LINKED_LIST_H

