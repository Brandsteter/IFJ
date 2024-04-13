/**
 *Project: Implementácia prekladača imperatívneho jazyka IFJ22
 *@file dynamic_string.h
 *@brief Header file for dynamic_string.c
 *@author Lucia Šírová <xsirov01>
 *@author David Brandšteter <xbrand13>
*/

#ifndef DYNAMIC_STRING
#define DYNAMIC_STRING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    char *str;
    int length;
    int size;
} dynamicStr;


bool dynamicStrInit(dynamicStr *myStr);

bool dynamicStrAdd(dynamicStr *myStr, char c);

bool dynamicStrAddStr(dynamicStr *myStr, char *string);

void dynamicStrDispose(dynamicStr *myStr);

#endif
