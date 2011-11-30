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

void bishop_pos_stack_pop(bishop_pos_stack *stack, int *diag_main, int *diag_sec)
{
    if(diag_main == NULL || diag_sec == NULL)
        return;
    
    if(stack == NULL || stack->len == 0) {
        *diag_main = *diag_sec = -1;
        return;
    }

    *diag_main = stack->data[len-1].diag_main;
    *diag_sec = stack->data[len-1].diag_sec;
    (stack->len)--;
}

void bishop_pos_stack_push(bishop_pos_stack *stack, int diag_main, int diag_sec)
{
    if(stack == NULL || stack->size == 0)
        return;

    if(stack->len + 1 > stack->size) {
        stack->size *= 2;
        stack->data = realloc(sizeof(bishop_pos) * stack->size);
    }

    stack->data[stack->len].diag_main = diag_main;
    stack->data[stack->len].diag_sec = diag_sec;
    stack->len++;
}

void bishop_pos_stack_peek(bishop_pos_stack *stack, int index, int *diag_main, int *diag_sec)
{
    if(diag_main == NULL || diag_sec == NULL)
        return;

    if(stack == NULL || stack->len >= index) {
        *diag_main = *diag_sec = -1;
        return;
    }

    *diag_main = stack->data[index].diag_main;
    *diag_sec = stack->data[index].diag_sec;
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
        int diag_main, diag_sec;
        bishop_pos_stack_peek(stack, i, &diag_main, &diag_sec);

        if(diag_main < 0 || diag_sec < 0)
            break;

        


    }



}
