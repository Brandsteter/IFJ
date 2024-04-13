/**
 *Project: Implementácia prekladača imperatívneho jazyka IFJ22
 *@file expression_pars.h
 *@brief header file for expression parser for IFJ22 code 
 *@author Lucia Šírová <xsirov01>
 *@author Daniel Blaško <xblask05>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "err_handling.h"
#include "dynamic_string.h"
#include "param_list.h"
#include "symtable.h"
#include "expression_stack.h"
#include "code_generator.h"

#ifndef EXPRESSION_PARS
#define EXPRESSION_PARS

#define MAX_TMP_STRING_SIZE 15


///Enumetating relation types
typedef enum {
    SH, //SHIFT <
    RD, //REDUCE >
    EQ, //EQUAL =
    ER, //ERROR
    DN  //DONE
} table_relation_type;


///Enumerating operator types
typedef enum {
    MUL_type,        // *   (0)
    DIV_type,        // /   (1)
    ADD_type,       // +   (2)
    SUB_type,      // -   (3)
    CONCAT_type,     // .   (4)
    LESSER_type,     // <   (5)
    LESS_EQ_type,    // <=  (6)
    GREATER_type,    // >   (7)
    GREATER_EQ_type, // >=  (8)
    EQ_type,         // === (9)
    NOT_EQ_type,     // !== (10)
    LEFT_PAR_type,   // (   (11)
    RIGHT_PAR_type,  // )   (12)
    ID_type,         // id  (13)
    OTHER_type,       // $   (14)
    //
    EXPRESSION_type  // E->i for code generator
} table_operator_type;


int return_operator_val(Token token);
int token_fetching(Token *token, bst_node_t **btree, bool in_function);
int expression_eval(bool in_function, bst_node_t **btree);
int reduce_function(bool in_function, bst_node_t **btree);
int reduction_semantics_check(stackElementPtr *op1, stackElementPtr *op2, stackElementPtr *op3);
int reduction_semantics_check_single_op(stackElementPtr *op3, bool in_function, bst_node_t **btree);
#endif
