/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file sym_table.h
 *@brief Symbol table types and function declarations 
 *@author Daniel Blaško <xblask05>
*/

#ifndef SYMTABLE
#define SYMTABLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "err_handling.h"
#include "dynamic_string.h"
#include "param_list.h"

///Types of variables
typedef enum {
    integer, 
    floatval, 
    string,
    voidval,
    boolval,
    nullval
} var_type;

/// @brief storing data of a function (previous calling, function parameters data and return type)
typedef struct function_data {
    bool is_defined;
    paramList *param_list;
    int return_type;
    struct bst_node *var_tree;
} function_data_t;

/// @brief storing data of a variable (variable type, value)
typedef struct var_data {
    int var_type;
    int int_value;
    float float_value;
    char *string_value;
    bool is_param;
    int param_index;
    bool declared_in_if;
} var_data_t;


/**
 *@brief BST Node structure
 * id - BST Node identificator
 * is_function - if 1, then node contains a function, if 0, node stores a variable
 * function_parameters, variable_data - containing data of function/variable, if node contains a function, variable data is NULL and vice versa
 * lptr, rptr - pointers to node children
*/
typedef struct bst_node {
    char * id;
    bool is_function; ///If 1, node contains a function, if 0, node contains a variable
    function_data_t function_parameters;
    var_data_t variable_data;

    struct bst_node *lptr;
    struct bst_node *rptr;
} bst_node_t;

///BST Function declarations
bst_node_t *new_node_func(int return_type, paramList *param_list);
bst_node_t *new_node_var(int data_type, char *name);
bst_node_t *new_node_parameter_var(int var_type, char* name, int param_index);
void bst_init(bst_node_t **btree);
void bst_insert(bst_node_t **btree, bst_node_t *new_node, char *id); //id bude pravdepodobne new_node->id vo volani, je to takto spravene pre prehladnost
bst_node_t *bst_search(bst_node_t **btree, char *id);
void bst_replace_by_rightmost(bst_node_t *tager_node, bst_node_t **btree);
void bst_delete(bst_node_t **btree, char *id);
void bst_dispose(bst_node_t *btree);
void bst_free_param_lists(bst_node_t *btree);
void bst_print_node(bst_node_t *node);

#endif