/**
 *Project: Implementácia prekladača imperatívneho jazyka IFJ22
 *@file expression_stack.c
 *@brief Stack structure for expression elements in expression parser 
 *@author Lucia Šírová  <xsirov01>
 *@author Daniel Blaško <xblask05> 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtable.h"
#include "syntax_analysis.h"
#include "expression_stack.h"


void stack_init(Stack *stack){
    stack -> top = NULL;
}

void stack_push(Stack *stack, int type, int op_type, int int_value, float float_value, char * string, bool is_var){
    stackElementPtr *newStackElement = malloc(sizeof(struct stackElement));
    if (newStackElement == NULL){
        INTERNAL_ERR_MSG();
    }
    if (type == type_string || type == type_functionID || type == type_variableID || type == type_dataTypeID || type == type_keyword){
        newStackElement -> name = string;
    }
   
    else if (type == type_integer){
        newStackElement -> int_value = int_value;
    }
    else if (type == type_float){
        newStackElement -> float_value = float_value;
    }
    newStackElement -> op_type = op_type;
    newStackElement -> type = type;
    newStackElement -> is_var = is_var;
    if (is_var){
        newStackElement->name = string;
    }
    if (stack -> top == NULL){
        stack -> top = newStackElement;
        stack -> top -> next = NULL;
    }
    else{
        newStackElement -> next = stack -> top;
        stack -> top = newStackElement;
    } 
}

void stack_pop(Stack *stack){   
    if(stack -> top != NULL){
        stackElementPtr *tmp = stack -> top;
        stack ->top = tmp->next;
        free(tmp);
    }  
}

stackElementPtr *stack_top(Stack *stack){
    return stack -> top;
}

stackElementPtr *stack_2nd(Stack *stack){
    if (stack -> top != NULL && stack -> top -> next != NULL){
        return stack -> top -> next;
    }
    else {
        INTERNAL_ERR_MSG();
    }
}

bool stack_is_empty(Stack *stack){
    if (stack -> top != NULL){
        return true;
    }
    else {
        return false;
    }
}

void stack_dispose(Stack *stack){
    while (stack -> top != NULL){
        stack_pop(stack);
    }
    stack->top = NULL;
}
