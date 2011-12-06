#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "board.h"

#include "board_hash_table.h"

#define IS_POWER_OF_2(x) ((x) != 0 && ((x) & ((x)-1)) == 0)

static uint32_t to_power_of_2(uint32_t n)
{
    if(!IS_POWER_OF_2(n)) {
        int bit;
        for(bit = 0; bit < 32; bit++) {
            if(n & (1 << bit))
                break;
        }

        n = 1 << (bit+1);
    }

    return n;
}

board_hash_table_t *board_hash_table_create(uint32_t size)
{
    board_hash_table_t *table;
    board_list_t **data;

    if(size == 0)
        return NULL;

    size = to_power_of_2(size);

    table = malloc(sizeof(*table));
    if(table == NULL)
        return NULL;

    data = calloc(sizeof(board_list_t*), size);
    if(data == NULL) {
        free(table);
        return NULL;
    }


    table->size = size;
    table->used = 0;
    table->data = data;

    return table;
}

board_t *board_hash_table_search(board_hash_table_t *table,
                                 board_t *board)
{
    uint64_t hash, hash2;
    uint32_t index;
    board_list_t *list;

    if(table == NULL || board == NULL)
        return NULL;

    hash = board_hash(board, &hash2);
    index = hash & (table->size - 1);

    for(list = table->data[index];
        list != NULL;
        list = list->next)
    {
        if(list->hash == hash && list->hash2 == hash2
           && board_compare(list->item, board))
        {
            return list->item;
        }
    }

    return NULL;
}

void board_hash_table_expand(board_hash_table_t *table,
                             uint32_t size)
{
    board_list_t **new_data;
    uint32_t old_size, i;

    if(table == NULL || size == 0)
        return;

    old_size = table->size;
    if(old_size >= size)
        return;

    size = to_power_of_2(size);

    new_data = calloc(sizeof(board_list_t*), size);
    if(new_data == NULL)
        return;

    for(i = 0; i < old_size; i++) {
        board_list_t* list = table->data[i];

        if(list) {
            uint32_t new_index = list->hash & (size-1);
            new_data[new_index] = list;
        }
    }

    free(table->data);
    table->data = new_data;
    table->size = size;
}


board_t *board_hash_table_insert(board_hash_table_t *table,
                                 board_t *board)
{
    board_t *board_new;
    board_list_t *list = NULL, *list_new;
    uint32_t index;
    uint64_t hash, hash2;

    if(table == NULL || board == NULL)
        return NULL;

    if(table->used >= ((table->size * 3) / 4)) {
        board_hash_table_expand(table, table->size * 2);
    }

    hash = board_hash(board, &hash2);
    index = hash & (table->size - 1);

    list = table->data[index];
    if(list != NULL) {

        for(;;) {
            if(list->hash == hash && list->hash2 == hash2
               && board_compare(list->item, board))
            {
                return list->item;
            }

            if(list->next == NULL)
                break;

            list = list->next;
        }
    }

    board_new = board_clone(board);
    if(board_new == NULL)
        return NULL;

    list_new = malloc(sizeof(*list_new));
    if(list_new == NULL) {
        board_free(board_new);
        return NULL;
    }

    list_new->item = board_new;
    list_new->hash = hash;
    list_new->hash2 = hash2;
    list_new->next = NULL;

    if(list != NULL)
        list->next = list_new;
    else
        table->data[index] = list_new;

    table->used++;

    return NULL;
}

void board_hash_table_print(board_hash_table_t *table)
{
    uint32_t i;

    if(table == NULL)
        return;

    for(i = 0; i < table->size; i++) {
        board_list_t *cur;

        for(cur = table->data[i];
            cur != NULL;
            cur = cur->next)
        {
            board_print(cur->item);
        }
    }

}

void board_hash_table_free(board_hash_table_t *table)
{
    uint32_t i;

    if(table == NULL)
        return;

    for(i = 0; i < table->size; i++) {
        board_list_t *cur, *next;

        cur = table->data[i];
        if(cur == NULL)
            break;

        for(next = cur->next;
            cur != NULL;
            cur = next, next = next->next)
        {
            board_free(cur->item);
            free(cur);
        }
    }

    free(table->data);
    free(table);
}
