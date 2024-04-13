/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file param_list.c
 *@brief Implementation of linked list structure for function parameters
 *@author Lucia Šírová <xsirov01>
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "param_list.h"

/// @brief Initialization of paramList.
/// @param pList 
void param_list_init(paramList *pList){
    pList -> firstElement = NULL;
    pList -> activeElement = NULL;
}


void param_list_insert(paramList *pList, Token tkn_np){
    paramListElementPtr newElement = malloc(sizeof(struct paramListElement));
    if (tkn_np.type == type_string || tkn_np.type == type_functionID || tkn_np.type == type_variableID || tkn_np.type == type_dataTypeID || tkn_np.type == type_keyword){
        newElement -> name = tkn_np.attribute.string->str; 
    }
    else if (tkn_np.type == type_integer){
        newElement -> int_value = tkn_np.attribute.integerNum;
    }
    else if (tkn_np.type == type_float){
        newElement -> float_value = tkn_np.attribute.floatNum;
    }    
    
    newElement-> type = tkn_np.type;
    newElement -> next = NULL;
    if (newElement == NULL){
        INTERNAL_ERR_MSG();
    }
    
    if (pList -> firstElement == NULL){
        pList -> firstElement = newElement;
        pList -> activeElement = newElement;
    }
    else{
        
        while (pList -> activeElement -> next != NULL){
            pList -> activeElement = pList -> activeElement -> next;
        }
        pList -> activeElement -> next = newElement;
        pList -> activeElement = pList -> activeElement -> next;
    }


}
/// @brief Delete last element from list.
/// @param pList 
void param_list_delete_last(paramList *pList){
    param_list_first(pList);
    if (pList -> activeElement != NULL){
        while (pList->activeElement->next->next != NULL) {
            pList->activeElement = pList->activeElement->next;
        }
        paramListElementPtr del_node = pList->activeElement->next;
        pList->activeElement->next = NULL;
        free(del_node);
    }
}

void param_list_copy(paramList *srcList, paramList *destList){
    
    while (srcList -> activeElement != NULL){
        paramListElementPtr newElement = malloc(sizeof(struct paramListElement));
        if (newElement == NULL) {
            INTERNAL_ERR_MSG();
        }
        newElement -> name = srcList ->activeElement -> name;
        newElement -> int_value = srcList ->activeElement ->int_value;
        newElement -> float_value = srcList -> activeElement -> float_value;
        newElement -> type = srcList -> activeElement -> type;
        newElement -> index = srcList -> activeElement -> index;
        newElement -> next = srcList -> activeElement -> next;
        if (destList->firstElement == NULL) {
            destList->firstElement = newElement;
            destList->activeElement = destList->firstElement;
            destList -> activeElement -> next = NULL;

            srcList->activeElement = srcList -> activeElement -> next;
        }
        else {
            destList -> activeElement ->next = newElement;
            srcList->activeElement = srcList -> activeElement -> next;
            destList->activeElement = destList -> activeElement -> next;
            destList -> activeElement -> next = NULL;
        }
    }
}

void param_list_dispose(paramList *pList) {
    paramListElementPtr tempNode;
    while (pList->firstElement != NULL) {
        tempNode = pList->firstElement;
        
        pList->firstElement = tempNode->next;
        
        free(tempNode);
    }
    pList->firstElement = NULL;
    pList->activeElement = NULL;
}

/// @brief Check if param_list is empty.
/// @param pList 
/// @return 1 if it is empty, 0 if it is not empty
bool param_list_is_empty(paramList *pList){
    if (pList -> firstElement == NULL){
        return true;
    }
    else{
        return false;
    }
}

void param_list_delete_first(paramList *pList) {
    if (pList->firstElement != NULL) {
        paramListElementPtr tempNode = pList->firstElement;
        if (pList->firstElement == pList->activeElement) {
            pList->activeElement = pList->activeElement->next;
        }
        pList->firstElement = tempNode->next;
        free(tempNode);
    }
}

void param_list_first(paramList *pList) {
    pList->activeElement = pList->firstElement;
}

void param_list_last(paramList *pList) {
    param_list_first(pList);
    while (pList->activeElement->next != NULL) {
        pList->activeElement = pList->activeElement->next;
    }
}
