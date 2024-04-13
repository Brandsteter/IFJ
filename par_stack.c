/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file par_stack.c  
 *@brief stack for checking function, if and while clause brackets 
 *@author Daniel Blaško <xblask05>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexical_analysis.h"
#include "err_handling.h"
#include "par_stack.h"

void par_stack_init(ParStack *par_stack){
    par_stack -> top = NULL;
}

void par_stack_push(ParStack *par_stack, int par_type, int if_count, int while_count){
    parStackElementPtr *newStackElement = malloc(sizeof(struct parStackElement));
    if (newStackElement == NULL){
        INTERNAL_ERR_MSG();
    }

    newStackElement ->par_type = par_type;

    if ((par_type == if_par) || (par_type == else_par))  {
        newStackElement->if_count = if_count;
    }
    else if (par_type == while_par) {
        newStackElement->while_count = while_count;
    }

    if (par_stack -> top == NULL){
        par_stack -> top = newStackElement;
        par_stack -> top -> next = NULL;
    }
    else{
        newStackElement -> next = par_stack -> top;
        par_stack -> top = newStackElement;
    } 
}

void par_stack_pop(ParStack *par_stack){   
    if(par_stack -> top != NULL){
        parStackElementPtr *tmp = par_stack -> top;
        par_stack ->top = tmp->next;
        free(tmp);
    }  
}

parStackElementPtr *par_stack_top(ParStack *par_stack){
    return par_stack -> top;
}

bool par_stack_is_empty(ParStack *par_stack){
    if (par_stack -> top != NULL){
        return true;
    }
    else {
        return false;
    }
}

void par_stack_dispose(ParStack *par_stack){
    while (par_stack -> top != NULL){
        par_stack_pop(par_stack);
    }
    par_stack->top = NULL;
}
