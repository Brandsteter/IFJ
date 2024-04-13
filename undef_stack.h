/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file undef_stack.h 
 *@brief Header file for the stack for undefined variables
 *@author Daniel Blaško <xblask05>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexical_analysis.h"
#include "err_handling.h"
#include "param_list.h"
#include "symtable.h"

#ifndef UNDEF_STACK
#define UNDEF_STACK

typedef struct undefinedStackElement {
    char *function_id;
    paramList *param_list;
    struct bst_node *function_frame;
    struct undefinedStackElement *next;
} undefinedStackElementPtr;

typedef struct {
    undefinedStackElementPtr *top;
} UndefinedStack;


void undefined_stack_init(UndefinedStack *stack);
void undefined_stack_push(UndefinedStack *stack, char *function_id, bst_node_t *function_frame);
void undefined_stack_pop(UndefinedStack *stack);
undefinedStackElementPtr *undefined_stack_top(UndefinedStack *stack);
bool undefined_stack_is_empty(UndefinedStack *stack);
void undefined_stack_dispose(UndefinedStack *stack);
void check_undefined_calls(UndefinedStack *stack, bst_node_t **btree);

#endif