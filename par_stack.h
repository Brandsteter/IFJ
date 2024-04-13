/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file par_stack.c  
 *@brief Header file for par_stack.c
 *@author Daniel Blaško <xblask05>
*/

#ifndef PAR_STACK
#define PAR_STACK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexical_analysis.h"
#include "err_handling.h"

typedef enum {
    if_par,
    else_par,
    while_par, 
    function_par, 
    function_void_par
} par_stack_type;

typedef struct parStackElement {
    par_stack_type par_type;
    int if_count;
    int while_count;
    struct parStackElement *next;
} parStackElementPtr;

typedef struct {
    parStackElementPtr *top;
} ParStack;

void par_stack_init(ParStack *par_stack);
void par_stack_push(ParStack *par_stack, int par_type, int if_count, int while_count);
void par_stack_pop(ParStack *par_stack);
parStackElementPtr *par_stack_top(ParStack *par_stack);
bool par_stack_is_empty(ParStack *par_stack);
void par_stack_dispose(ParStack *par_stack);

#endif
