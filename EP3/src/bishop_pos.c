#include <stdlib.h>
#include <stdio.h>

#include "board.h"

#include "bishop_pos.h"

bishop_pos_stack* bishop_pos_stack_create(int size)
{
    bishop_pos_stack *stack;
    bishop_pos *data;

    if(size <= 0)
        return NULL;

    stack = malloc(sizeof(*stack));
    if(stack == NULL)
        return NULL;

    data = malloc(sizeof(*data) * size);
    if(data == NULL) {
        free(stack);
        return NULL;
    }

    stack->size = size;
    stack->len = 0;
    stack->data = data;

    return stack;
}

void bishop_pos_stack_pop(bishop_pos_stack *stack, int *x, int *y)
{
    if(x == NULL || y == NULL)
        return;
    
    if(stack == NULL || stack->len == 0) {
        *x = *y = -1;
        return;
    }

    *x = stack->data[len-1].x;
    *y = stack->data[len-1].y;
    (stack->len)--;
}

void bishop_pos_stack_push(bishop_pos_stack *stack, int x, int y)
{
    if(stack == NULL || stack->size == 0)
        return;

    if(stack->len + 1 > stack->size) {
        stack->size *= 2;
        stack->data = realloc(sizeof(bishop_pos) * stack->size);
    }

    stack->data[stack->len].x = x;
    stack->data[stack->len].y = y;
    stack->len++;
}

void bishop_pos_stack_peek(bishop_pos_stack *stack, int index, int *x, int *y)
{
    if(x == NULL || y == NULL)
        return;

    if(stack == NULL || stack->len >= index) {
        *x = *y = -1;
        return;
    }

    *x = stack->data[index].x;
    *y = stack->data[index].y;
}

void bishop_pos_stack_free(bishop_pos_stack *stack)
{
    if(stack != NULL) {
        free(stack->data);
        free(stack);
    }
}

board_t *bishop_pos_stack_to_board(bishop_pos_stack *stack, int board_size)
{
    board_t *board;
    int i;

    if(stack == NULL || board_size <= 0)
        return  NULL;

    board = board_create(board_size);
    if(board == NULL)
        return NULL;

    for(i = 0; i < stack->len; i++) {
        int x, y;
        bishop_pos_stack_peek(stack, i, &x, &y);

        if(x < 0 || y < 0)
            break;

        


    }



}
