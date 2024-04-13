/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file syntax_analysis.h
 *@brief Header file for syntax analysis
 *@author Lucia Šírová  <xsirov01>
 *@author Daniel Blaško <xblask05>
*/

#ifndef SYNTAX_ANALYSIS
#define SYNTAX_ANALYSIS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "syntax_analysis.h"
#include "lexical_analysis.h"
#include "err_handling.h"
#include "symtable.h"
#include "dynamic_string.h"
#include "param_list.h"
#include "par_stack.h"
#include "undef_stack.h"

int prog_rule(Token *token);
int body_rule(Token *token);
int function_declare_rule(Token *token);
int params_rule(Token *token);
int params_n_rule(Token *token);
int statement_rule(Token *token);
int if_rule(Token *token); 
int else_rule(Token *token);
int while_rule(Token *token);
int return_rule(Token *token);
int right_curly_par_rule(Token *token);
int function_call_rule(Token *token, char *variable_name);
int params_function_call_rule(Token *token, char *variable_name);
int params_n_function_call_rule(Token *token, char *variable_name);
int variable_rule(Token *token);
int end_rule(Token *token);
void push_builtin_functions();

#endif
