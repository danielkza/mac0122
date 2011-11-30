#ifndef BISHOP_POS_H
#define BISHOP_POS_H

#include <stddef.h>

typedef struct {
    int x;
    int y;
} bishop_pos;

typedef struct {
    int len;
    int size;
    bishop_pos *data
} bishop_pos_stack;

bishop_pos_stack* bishop_pos_stack_create(int size);
void bishop_pos_stack_pop(bishop_pos_stack *stack, int *x, int *y);
void bishop_pos_stack_push(bishop_pos_stack *stack, int x, int y);
void bishop_pos_stack_free(bishop_pos_stack *stack);

#endif


