/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file param_list.h
 *@brief Header file for param_list.c
 *@author Lucia Šírová <xsirov01>
*/

#ifndef PARAM_LIST
#define PARAM_LIST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexical_analysis.h"
#include "err_handling.h"

/**
 *@brief Linked list structure
 *@param type parameter type
 *@param param parameter data
 *@param next pointer to next item in list
*/
typedef struct paramListElement {
    char *name;
    int int_value;
    float float_value;
    int type;
    int op_type; //For precedence operation
    bool is_var;
    int index;
    struct paramListElement *next;
} *paramListElementPtr;

typedef struct  {
    paramListElementPtr firstElement;
    paramListElementPtr activeElement;
} paramList;

void param_list_init(paramList *plist);
void param_list_insert(paramList *pList, Token token);
void param_list_delete_last(paramList *pList);
bool param_list_is_empty(paramList *pList);
Token param_last_Element(paramList *pList);
void param_list_first(paramList *pList);
void param_list_copy(paramList *srcList, paramList *destList);
void param_list_dispose(paramList *plist);
void param_list_delete_first(paramList *pList);
void param_list_first(paramList *pList);
void param_list_last(paramList *pList);


#endif
