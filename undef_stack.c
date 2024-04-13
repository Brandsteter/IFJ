/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file undef_stack.c
 *@brief Stack for calls to functions which have not been declared yet
 *@author Daniel Blaško <xblask05>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexical_analysis.h"
#include "err_handling.h"
#include "param_list.h"
#include "undef_stack.h"
#include "symtable.h"

void undefined_stack_init(UndefinedStack *stack) {
    stack->top = NULL;
}

void undefined_stack_push(UndefinedStack *stack, char *function_id, bst_node_t *function_frame) {
    undefinedStackElementPtr *new_element = malloc(sizeof(struct undefinedStackElement));
    if (new_element == NULL) {
        INTERNAL_ERR_MSG();
    }

    new_element->function_id = function_id;  
    new_element->function_frame = function_frame;
    if (stack->top == NULL) {
        stack->top = new_element;
        stack->top->next = NULL;
    }
    else {
        new_element->next = stack->top;
        stack->top = new_element;
    }
}

void undefined_stack_pop(UndefinedStack *stack) {
    if (stack->top != NULL) {
        undefinedStackElementPtr *tmp = stack->top;
        stack->top = stack->top->next;
        free(tmp);
    }
}

undefinedStackElementPtr *undefined_stack_top(UndefinedStack *stack) {
    return stack->top;
}

bool undefined_stack_is_empty(UndefinedStack *stack) {
    if (stack->top == NULL) {
        return true;
    }
    return false;
}

void undefined_stack_dispose(UndefinedStack *stack) {
    while (stack->top != NULL) {
        undefined_stack_pop(stack);
    }
    stack->top = NULL;
}

void check_undefined_calls(UndefinedStack *stack, bst_node_t **btree) {
    bst_node_t *temp_node;
    bst_node_t *var_node;
    int dataVal;
    int callval;
    while (stack->top != NULL) { 
        temp_node = bst_search(btree, stack->top->function_id);
        ///Function still not declared
        if (temp_node == NULL) {
                SEMANTICS_ERR_DEF_MSG();
        }
        param_list_first(stack->top->param_list);
        param_list_first(temp_node->function_parameters.param_list);
        while ((temp_node->function_parameters.param_list->activeElement != NULL) && (stack->top->param_list -> activeElement != NULL)) {
            ///Types are matching
            if (!strcmp( temp_node->function_parameters.param_list->activeElement->name,"int")){
                dataVal = type_integer;
            }
            else if (!strcmp( temp_node->function_parameters.param_list->activeElement->name,"float")){
                dataVal = type_float;
            }
            else if (!strcmp( temp_node->function_parameters.param_list->activeElement->name,"string")){
                dataVal = type_string;
            }

            if (stack->top->param_list->activeElement->type == type_variableID) {
                var_node = bst_search(&(stack->top->function_frame->function_parameters.var_tree), stack->top->param_list->activeElement->name);
                ///Variable not found in tree
                if (var_node == NULL) {
                    SEMANTICS_ERR_VAR_MSG();
                }
                ///Found variable
                else {
                    if (var_node->variable_data.var_type == integer) {
                        callval = type_integer;
                    }
                    else if (var_node->variable_data.var_type == floatval) {
                        callval = type_float;
                    }
                    else if (var_node->variable_data.var_type == string) {
                        callval = type_string;
                    }
                    else if (var_node->variable_data.var_type == nullval) {
                        callval = type_null;
                    }
                }
            }
            ///Function call parameter isn't a variable
            if ((stack->top->param_list->activeElement->type != type_variableID) && (stack->top->param_list->activeElement->type == dataVal)) {
                ///Called function only has variables without type declaration, so we move the call stack by one and node stack by two
                stack->top->param_list->activeElement = stack->top->param_list->activeElement->next;
                temp_node->function_parameters.param_list->activeElement = temp_node->function_parameters.param_list->activeElement->next->next;
            }
            ///Function call parameter is a variable
            else if ((stack->top->param_list->activeElement->type == type_variableID) && (callval == dataVal)) {
                stack->top->param_list->activeElement = stack->top->param_list->activeElement->next;
                temp_node->function_parameters.param_list->activeElement = temp_node->function_parameters.param_list->activeElement->next->next; 
            }
            ///Types are different
            else  {
                SEMANTICS_ERR_RUN_MSG();
            }
        }
        ///If the number of parameters isn't the same, one of the lists didn't cycle through to the end
        if ((temp_node->function_parameters.param_list->activeElement != NULL) || (stack->top->param_list->activeElement != NULL)) {
            SEMANTICS_ERR_RUN_MSG(); 
        }
        undefined_stack_pop(stack);
    }
}
