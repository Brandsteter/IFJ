/**
 *Project: Implementácia prekladača imperatívneho jazyka IFJ22
 *@file expression_stack.c
 *@brief Header file for expression stack file
 *@author Lucia Šírová  <xsirov01>
 *@author Daniel Blaško <xblask05> 
*/

#ifndef EXPRESSION_STACK
#define EXPRESSION_STACK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexical_analysis.h"
#include "err_handling.h"
#include "param_list.h"


typedef struct stackElement {
    char *name;
    int int_value;
    float float_value;
    int type;
    int op_type; //For precedence operation
    bool is_var;
    struct stackElement *next;
} stackElementPtr;

typedef struct {
    stackElementPtr *top;
} Stack;

void stack_init(Stack *stack);
stackElementPtr *stack_top(Stack *stack);
stackElementPtr *stack_2nd(Stack *stack);
void stack_push(Stack *stack, int type, int op_type, int int_value, float float_value, char * string, bool is_var);
void stack_pop(Stack *stack);
bool stack_is_empty(Stack *stack);
void stack_dispose(Stack *stack);

#endif
