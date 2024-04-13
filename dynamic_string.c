/**
 *Project: Implementácia prekladača imperatívneho jazyka IFJ22
 *@file dynamic_string.c
 *@brief Functions for dynamically allocated strings
 *@author Lucia Šírová <xsirov01>
 *@author David Brandšteter <xbrand13>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dynamic_string.h"

#define DYNAMIC_STR_START 50

//dynamic str
bool dynamicStrInit(dynamicStr *myStr){
    myStr -> str = (char *) malloc(DYNAMIC_STR_START);
    
    if (myStr -> str == NULL){
        return false;
    }
    myStr -> length = 0;
    myStr -> size = 50;
    myStr -> str[myStr -> length] = '\0';

    return true;

}

bool dynamicStrAdd(dynamicStr *myStr, char c){
    myStr -> size += 1;
    myStr -> str = realloc(myStr -> str, myStr -> size);

    if (myStr -> str == NULL){
        return false;
    }

    myStr -> str[myStr -> length] = c;
    myStr -> str[myStr -> length + 1] = '\0';
    myStr -> length += 1;

    return true;
}

bool dynamicStrAddStr(dynamicStr *myStr, char *string){
    myStr -> size += strlen(string);
    myStr -> str = realloc(myStr -> str, myStr -> size);

    if (myStr -> str == NULL){
        return false;
    }

    strcat(myStr -> str, string);
    myStr -> length += strlen(string);
    myStr -> str[myStr -> length + 1] = '\0';


    return true;
}

void dynamicStrDispose(dynamicStr *myStr){
    //myStr->length = 0;
    //myStr->str[myStr->length] = '\0';
    free(myStr -> str);

}
