/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file syntax_analysis.c
 *@brief Syntax and semantics analysis of IFJ22 code 
 *@author Lucia Šírová  <xsirov01>
 *@author Daniel Blaško <xblask05>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "syntax_analysis.h"
#include "lexical_analysis.h"
#include "err_handling.h"
#include "symtable.h"
#include "dynamic_string.h"
#include "param_list.h"
#include "expression_pars.h"
#include "code_generator.h"
#include "par_stack.h"
#include "undef_stack.h"

#define RULE_CHECK_OK 0

///Global variables

///Declaring global symbol table tree and symbol table tree node we will be working with in frames
bst_node_t *global_btree;
bst_node_t *current_btree;

///Checking if we checked function parameters during function declaration
bool function_param_visited = false;
///Checking if we checked function parameters during function calling
bool call_param_visited = false;

///Checking whether declared function is void or non-void 
bool is_void;

///Current function ID
char *global_id;

paramList tmpList; paramList *param_list = &tmpList;
paramList tmpX; paramList *aux_list = &tmpX;

///Stack for if, while clauses and functions
ParStack tmps; ParStack *par_stack = &tmps;
///Stack for undefined function calls
UndefinedStack tmpu; UndefinedStack *undefined_stack = &tmpu;

///bool variable determining if we're inside a function or if
bool in_function = false;
bool in_if = false;
///variables checking presence of "return"
bool return_in_if = false;
bool return_in_else = false;
bool return_in_function = false;
bool asssign_function_to_variable = false;
bool undefined_function = false;

///Builting function parameter lists
paramList rl;  paramList *read_list = &rl; 
paramList wl;  paramList *write_list = &wl;
paramList sl;  paramList *strval_list = &sl; 
paramList stl; paramList *strlen_list = &stl;   
paramList sul; paramList *substring_list = &sul;
paramList ol;  paramList *ord_list = &ol;
paramList cl;  paramList *chr_list = &cl;

///Counter of "if", "else" and "while" clauses for code generator
int if_count = 0;
int while_count = 0;
int index_count = 0;


/**
 *@brief Implementation of <prog> rule 
 *<prog> -> <?php declare(strict_types=1); body 
*/
int prog_rule(Token *token) {
    /// Prologue included
    if (token->type == type_prologue) {
        ///Generating code
        init_final_code();
        print_start();
        print_main();
        return body_rule(token);
    }
    /// Missing or incorrect prologue 
    else {
        SYNTAX_ERR_MSG();
    };
}


/**
 *@brief Implementation of <body> rules
 *<body> -> <statement> <body> 
 *<body> -> <function_declare> <body> 
 *<body> -> <end> 
*/
int body_rule(Token *token) {
    if (!getNextToken(token)) {
        ///<body> -> <statement>
        if (((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "if"))) 
            || ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "else")) && (in_if == true))
            || ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "while"))) 
            || ((token->type == type_functionID)) || ((token->type == type_variableID))) {
                return statement_rule(token);
        }
        ///<body> -> <function_declare> <body>
        else if ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "function"))) {
            return function_declare_rule(token);
        }
        /// "}" for if, while and function
        else if (token->type == type_rightCurlyPar) {
            return right_curly_par_rule(token);
        }
        else if ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "return"))) {
            ///checking if return is used inside a function body
            return return_rule(token);
        } 
        ///<body> -> <end>
        else if ((token->type == type_endPrologue) || (token->type == type_EOF)) {
            return end_rule(token);
        }

    }
    INTERNAL_ERR_MSG();
} 

int segment_body_rule(Token *token){

    if (!getNextToken(token)) {
        if (token -> type == 0){
        }
        ///"Else" not present after "if"
        if  ((in_if == true) && (strcmp(token->attribute.string->str, "else") != 0)) {
            SYNTAX_ERR_MSG();
        }

        ///<body> -> <statement>
        if (((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "if"))) 
            || ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "else")) && (in_if == true))
            || ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "while"))) 
            || ((token->type == type_functionID)) || ((token->type == type_variableID))) {

                return statement_rule(token);
        }
        ///Return 
        else if ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "return"))) {
            return return_rule(token);
        } 
        /// "}" for if, while and function
        else if (token->type == type_rightCurlyPar) {
            
            return right_curly_par_rule(token);
        }
        else {
            SYNTAX_ERR_MSG();
        }
    }
    
    INTERNAL_ERR_MSG();

}

/**
 *@brief Implementation of Function Declare rules 
 *<function_declare> -> FUNCTION ID "(" (params) ")" : <type> { (statement) return <expression> }
*/
int function_declare_rule(Token *token) {
    
    // call_declare = 1;
    Token token_np;
    token_np.type = token -> type;
    token_np.attribute = token -> attribute; //fix
    paramList tmp_btree_param_list;
    paramList *btree_param_list = &tmp_btree_param_list;
    Token variable_return_type = token_np;


    if (!function_param_visited) {
        ///fetching ID token
        if ((!getNextToken(token)) && (token->type == type_functionID)) {
                token_np.attribute = token->attribute;
                token_np.type = token->type;
                global_id = token_np.attribute.string->str;
                if (bst_search(&(global_btree), token_np.attribute.string->str) == NULL) {
                    ///fetching "(" token
                    if (!getNextToken(token)) {
                        if (token->type == type_leftPar) {
                            ///Fetching function parameters
                            return params_rule(token);
                        }
                        else {
                            SYNTAX_ERR_MSG();}
                    }
                }
                ///Found declared function with the same ID
                else {
                    SEMANTICS_ERR_DEF_MSG();
                }
        }
    }
    ///if we visited params rule, continuing to return type
    else {
        ///Setting function parameter index counter to 0
        index_count = 0;
        ///Creating label
        print_function_start(global_id);
        param_list_first(param_list);
        param_list_init(btree_param_list);
        param_list_copy(param_list, btree_param_list);

        param_list_first(btree_param_list);
        function_param_visited = false;
        ///Token isn't colon
        if ((!getNextToken(token)) && (token->type != type_colon)) {
            SYNTAX_ERR_MSG();                        
        } 
        //looking for function return data type
        if ((!getNextToken(token)) && (token->type == type_dataTypeID)) { //////////datatype/keyword
            ///PUSHNUT FUNKCIU NA BST
            ///Is a void function
            if (!strcmp(token->attribute.string->str, "void")) {
                is_void = true;
                //Inserting function to tree
                bst_insert(&(global_btree), new_node_func(voidval, btree_param_list), global_id);
                ///Switching current_btree pointer to newly created frame
                current_btree = bst_search(&(global_btree), global_id);
                ///Pushes void function onto parenthesis stack
                par_stack_push(par_stack, function_void_par, if_count, while_count);
            }
            ///Is a non-void function 
            else {
                is_void = false;
                //Inserting function to tree

                if (!strcmp(token->attribute.string->str, "int")) { 
                    bst_insert(&(global_btree), new_node_func(integer, btree_param_list), global_id);
                    ///Switching current_btree pointer to newly created frame
                    current_btree = bst_search(&(global_btree), global_id);
                }
                else if (!strcmp(token->attribute.string->str, "float")) { 
                    bst_insert(&(global_btree), new_node_func(floatval, btree_param_list), global_id);
                    ///Switching current_btree pointer to newly created frame
                    current_btree = bst_search(&(global_btree), global_id);
                }
                else if (!strcmp(token->attribute.string->str, "string")) { 
                    bst_insert(&(global_btree), new_node_func(string, btree_param_list), global_id);
                    ///Switching current_btree pointer to newly created frame
                    current_btree = bst_search(&(global_btree), global_id);
                }
                par_stack_push(par_stack, function_par, if_count, while_count);
            }
         
            ///Inserting function parameters to function's variable binary tree
            param_list_first(btree_param_list);
            bst_init(&(current_btree)->function_parameters.var_tree);
            
            bst_node_t *variable_temp_node;
            while (btree_param_list->activeElement != NULL) {
                if (btree_param_list->activeElement->type != type_variableID) {
                    variable_return_type.type = btree_param_list->activeElement->type;
                    ///assignit data type from data_type_id token
                    if (variable_return_type.type == type_dataTypeID){
                        if (!(strcmp(btree_param_list->activeElement->name,"int"))){
                            variable_return_type.type = (tokenType) integer;
                        }
                        else if (!(strcmp(btree_param_list->activeElement->name,"float"))){
                            variable_return_type.type = (tokenType) floatval;
                        }
                        else if (!(strcmp(btree_param_list->activeElement->name,"string"))){
                            variable_return_type.type = (tokenType) string;
                        }
                    }
                }
                else if (btree_param_list->activeElement->type == type_variableID) {
                    variable_temp_node = new_node_parameter_var(variable_return_type.type, btree_param_list->activeElement->name, btree_param_list->activeElement->index);
                    bst_insert(&(current_btree)->function_parameters.var_tree, variable_temp_node, btree_param_list->activeElement->name);
                }
                btree_param_list->activeElement = btree_param_list->activeElement->next;
            }
    
        } 
        ///Fetching "{", clearing stack
        param_list_dispose(param_list);

        if ((!getNextToken(token)) && (token->type == type_leftCurlyPar)) {
            in_function = true;
            return segment_body_rule(token);
        }
        else {
            SYNTAX_ERR_MSG();
        }
    }
   
    INTERNAL_ERR_MSG();
}

/**
 *@brief Implementation of Void function declare rule
 *<function_declare> -> FUNCTION ID "(" (params) ")" : void { (statement) (return) } 
*/
// int void_function_declare_rule(Token *token) {
//     ///fetching ID token
//     if ((!getNextToken(token)) && (token->type == type_functionID)) { 

//     }
// }

/**
 *@brief Checking whether function declaration has arguments and pushing the first one into the stack
 *<type> ID <params_n>
*/
int params_rule(Token *token) {
    ///If 1 function fetches parameters for function declaration
    ///If 0 function fetches parameters for function calling
    function_param_visited = true;

    Token token_np;
    ///fetching parameter type 
    if (!getNextToken(token)) {
        token_np.attribute = token->attribute;
        token_np.type = token->type;
        if (token->type == type_dataTypeID) { //pozor na datatypeID/Keyword
            param_list_insert(param_list, token_np); 
        }
        ///No parameters, returning to function declare rule
        else if (token->type == type_rightPar) {
            // if (call_declare == 1) {
            return function_declare_rule(token);
        }
        else {SYNTAX_ERR_MSG();}
    }
    ///fetching parameter
    if (!getNextToken(token)) {
        token_np.attribute = token->attribute;
        token_np.type = token->type;
        if (token->type == type_variableID) { 
            param_list_insert(param_list, token_np);
            param_list->activeElement->index = index_count;
            index_count++;
        }
        else {
            SYNTAX_ERR_MSG();}
    }
    ///fetching "," or ")"
    if (!getNextToken(token)) {
        if ((token->type == type_comma) || (token->type == type_rightPar)) {
            /// "," found, continuing to fetch next parameters
            if (token->type == type_comma) {
                return params_n_rule(token);
            }
            /// ")", continuing to fetch the return type of function
            else {
                // if (call_declare == 1) {
                param_list_first(param_list);
                return function_declare_rule(token);
                // }
                // else if (call_declare == 0) {
                //     return function_call_rule(token);
                // }
            }
        }
        else {
            SYNTAX_ERR_MSG();}
    } 
    
    INTERNAL_ERR_MSG();
}

/**
 *@brief Implementing rules for params_n rule (multiple parameters in function)
 *<params_n> -> ε
 *<params_n> -> , <type> ID <params_n>
 *<params_n> -> , <expression> <params_n>
*/
int params_n_rule(Token *token) {
    Token token_np;
    ///fetching parameter type 
    
    if (!getNextToken(token)) {
        token_np.attribute = token->attribute;
        token_np.type = token->type;
        if (token->type == type_dataTypeID) { 
            param_list_insert(param_list, token_np); 
        }
        ///No parameters, returning to function declare rule
        else if (token->type == type_rightPar) {
            return function_declare_rule(token);
        }
        else {SYNTAX_ERR_MSG();}
    }
    ///fetching parameter
    if (!getNextToken(token)) {
        token_np.attribute = token->attribute;
        token_np.type = token->type;
        if (token->type == type_variableID) { 
            param_list_insert(param_list, token_np);
            param_list->activeElement->index = index_count;
            index_count++;
        }
        else {
            SYNTAX_ERR_MSG();}
    }
    ///fetching "," or ")"
    if (!getNextToken(token)) {
        if ((token->type == type_comma) || (token->type == type_rightPar)) {
            /// "," found, continuing to fetch next parameters
            if (token->type == type_comma) {
                return params_n_rule(token);
            }
            /// ")", continuing to fetch the return type of function
            else {
                return function_declare_rule(token);
            }
        }
        else {
            SYNTAX_ERR_MSG();}
    } 
   
    INTERNAL_ERR_MSG();
 return 0;
}



int params_function_call_rule(Token *token, char *variable_name){
    Token token_np;
    call_param_visited = true;
    if (!getNextToken(token)) {
            token_np.attribute = token->attribute;
            token_np.type = token->type;
            if (param_list -> firstElement == NULL){
            }
            if (token->type == type_variableID || token->type == type_integer || token->type == type_string || token->type == type_float) { 
                param_list_insert(param_list, token_np); 
                param_list->activeElement->index = index_count;
                index_count++;
            }
            ///No parameters, returning to function declare rule
            else if (token->type == type_rightPar) {
                return function_call_rule(token,variable_name);
            }
            else {
                SYNTAX_ERR_MSG();}
    }
    if (!getNextToken(token)) {
        if (token -> type == type_comma) {
            return params_n_function_call_rule(token,variable_name);
        }
        else if (token -> type == type_rightPar) {
            return function_call_rule(token,variable_name);
        }
        else {
            SYNTAX_ERR_MSG();
        }
    }
    
    INTERNAL_ERR_MSG();


}

int params_n_function_call_rule(Token *token,char *variable_name){
    Token token_np;
    if (!getNextToken(token)) {
            token_np.attribute = token->attribute;
            token_np.type = token->type;
            if (token->type == type_variableID || token->type == type_integer || token->type == type_string || token->type == type_float) { //pozor na datatypeID/Keyword
                param_list_insert(param_list, token_np); 
                param_list->activeElement->index = index_count;
                index_count++;
            }
            else {SYNTAX_ERR_MSG();}
    }
    if (!getNextToken(token)) {
        if (token -> type == type_comma) {
            return params_n_function_call_rule(token,variable_name);
        }
        else if (token -> type == type_rightPar) {
            return function_call_rule(token,variable_name);
        }
        else {
            SYNTAX_ERR_MSG();
        }
    }
    
    INTERNAL_ERR_MSG();

}

/**
 *@brief Implementing rules for statements 
 *<statement> -> if "(" <expression> ")" { (statement) } else { (statement) } 
 *<statement> -> while "(" <expression> ")" { (statement) }
 *<statement> -> ID "(" (params) ")" ; (statement) 
 *<statement> -> (type) ID = <expression> ; (statement) 
 *<statement> -> (string) ID = reads "(" ")" ; (statement)
 *<statement> -> (int) ID = readi "(" ")" ; (statement)
 *<statement> -> (float) ID = readf "(" ")" ; (statement)
 *<statement> -> write "(" (params) ")"
 *<statement> -> <end>
*/
int statement_rule(Token *token) {

    if  ((in_if == true) && (strcmp(token->attribute.string->str, "else") != 0)) {
            SYNTAX_ERR_MSG();
    }

    ///IF clause
    if ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "if"))) {
        return if_rule(token);
    }
    ///ELSE clause
    if ((in_if == 1) && (token->type == type_keyword) && (!strcmp(token->attribute.string->str, "else"))) {
        return else_rule(token);
    }
    ///WHILE clause
    if ((token->type == type_keyword) && (!strcmp(token->attribute.string->str, "while"))) {
        return while_rule(token);
    }
    //Function call
    if (token->type == type_functionID) {
        global_id = token->attribute.string->str;
        bst_node_t *temp_function_call = bst_search(&(global_btree), global_id);
        ///Function not declared yet
        if (temp_function_call == NULL) {
            undefined_function = true;
        }
        return function_call_rule(token, "");
    }
    //Variable declaration
    if (token->type == type_variableID) {
        return variable_rule(token);
    }
    if ((token->type == type_EOF) || (token->type == type_endPrologue)) {
        return end_rule(token);
    }
    return 0;
}

int if_rule(Token *token) {
    if_count ++;
    int return_type;
    if ((!getNextToken(token)) && (token->type != type_leftPar)) {
        
        SYNTAX_ERR_MSG();
    }
    else {
        return_type = token_fetching(token, &(current_btree)->function_parameters.var_tree, in_function);
        ///Condition expression is not of boolean type (< <= > >= === !===)
        if (return_type != boolval) {
            RUN_ERR_TYPE_MSG();
        }
        ///in case precedence analysis returns no error, check inside of "if" curly brackets
        if (token->type == type_semicolon) {

            SYNTAX_ERR_MSG();
        }

    }
    if (token->type == type_leftCurlyPar) {
        par_stack_push(par_stack, if_par, if_count, while_count);
        print_if_start(if_count);
    }
    if (in_function == true) {
        return segment_body_rule(token);
    }
    else {
        return body_rule(token);
    }
}

int else_rule(Token *token) {
    in_if = false;
    int temp_if_count = par_stack->top->if_count;
    print_if_end(temp_if_count);
    if (!(getNextToken(token))){
        if (token -> type != type_leftCurlyPar){
            SYNTAX_ERR_MSG();
        }
    }
    par_stack_pop(par_stack);
    par_stack_push(par_stack, else_par, temp_if_count, while_count);
    if (in_function == true) {
        return segment_body_rule(token);
    }
    else {
        return body_rule(token);
    }
}

int while_rule(Token *token) {
    int return_type;
    while_count++;
    if ((!getNextToken(token)) && (token->type != type_leftPar)) {
        SYNTAX_ERR_MSG();
    }    
    else {
        print_while_start(while_count);
        return_type = token_fetching(token, &(current_btree)->function_parameters.var_tree, in_function);
        ///Condition expression is not of boolean type (< <= > >= === !===)
        if (return_type != boolval) {
            RUN_ERR_TYPE_MSG();
        }
        //in case precedence analysis returns no error, check inside of "while" curly brackets
        if (token->type == type_semicolon) {
            SYNTAX_ERR_MSG();
        }
    }
    if (token->type == type_leftCurlyPar) {
        
        ///Generating code for "while"
       
        print_while_check(while_count);
        par_stack_push(par_stack, while_par, if_count, while_count);
    }
    if (in_function == true) {
        return segment_body_rule(token);
    }
    else {
        return body_rule(token);
    }
}

int return_rule(Token *token) {
    int return_type = -1;
    ///Return is inside "if"
    if (par_stack->top->par_type == if_par) {
        return_in_if = true;
        ///We are inside of a non-void function
        if (par_stack->top->next->par_type == function_par) {
            if (!getNextToken(token)) {
                ///Returning function
                if (token->type == type_functionID) {
                    bst_node_t *temporary_node = bst_search(&(global_btree), token->attribute.string->str);
                    ///Checking for type compatibility in return 
                    if (temporary_node->function_parameters.return_type != current_btree->function_parameters.return_type) {
                        SEMANTICS_ERR_RUN_MSG();
                    }
                    else return function_call_rule(token, "");
                }
                ///Returning expression
                else {
                    return_type = token_fetching(token, &(current_btree)->function_parameters.var_tree, in_function);

                    print_function_retval_exp();
                    print_function_end(current_btree->id);
                    

                    ///Checking for correct command ending syntax
                    if (token->type != type_semicolon) {
                        SYNTAX_ERR_MSG();
                    }
                    ///Checking for type compatibility in return 
                    if (return_type != current_btree->function_parameters.return_type) {                     
                        SEMANTICS_ERR_RUN_MSG();
                    }
                    else return segment_body_rule(token);
                }
            }
        }
        ///Inside of a void function
        else if (par_stack->top->next->par_type == function_void_par) {
            if (!getNextToken(token)) {
                ///If void return syntax isn't "return;", an error occurs
                if (token->type != type_semicolon) {
                    RUN_ERR_RETURN_MSG();
                }
                print_function_end(current_btree->id);
                return segment_body_rule(token);
            }
        }
        else {
            SYNTAX_ERR_MSG();
        }
    }//end return inside of if
     ///Return is inside "else"
    else if (par_stack->top->par_type == else_par) {
        return_in_else = true;
        ///We are inside of a non-void function
        if (par_stack->top->next->par_type == function_par) {
            if (!getNextToken(token)) {
                ///Returning function
                if (token->type == type_functionID) {
                    bst_node_t *temporary_node = bst_search(&(global_btree), token->attribute.string->str);
                    ///Checking for type compatibility in return 
                    if (temporary_node->function_parameters.return_type != current_btree->function_parameters.return_type) {
                        SEMANTICS_ERR_RUN_MSG();
                    }
                    else return function_call_rule(token, "");
                }
                ///Returning expression
                else {
                    return_type = token_fetching(token, &(current_btree)->function_parameters.var_tree, in_function);
                    print_function_retval_exp();
                    print_function_end(current_btree->id);
                    ///Checking for correct command ending syntax
                    if (token->type != type_semicolon) {
                        SYNTAX_ERR_MSG();
                    }
                    ///Checking for type compatibility in return 
                    if (return_type != current_btree->function_parameters.return_type) {
                        SEMANTICS_ERR_RUN_MSG();
                    }
                    else return segment_body_rule(token);
                }
            }
        }
        ///Inside of a void function
        else if (par_stack->top->next->par_type == function_void_par) {
            if (!getNextToken(token)) {
                ///If void return syntax isn't "return;", an error occurs
                if (token->type != type_semicolon) {
                    RUN_ERR_RETURN_MSG();
                }

                print_function_end(current_btree->id);
                return segment_body_rule(token);
            }
        }
        else {
            SYNTAX_ERR_MSG();
        }
    }//end of in else
    ///Inside of a non-void function 
    else if (par_stack->top->par_type == function_par) {
        return_in_function = true;
        if (!getNextToken(token)) {
            ///Returning function
            if (token->type == type_functionID) {
                bst_node_t *temporary_node = bst_search(&(global_btree), token->attribute.string->str);
                ///Checking for type compatibility in return 
                if (temporary_node->function_parameters.return_type != current_btree->function_parameters.return_type) {
                    SEMANTICS_ERR_RUN_MSG();
                }
                else {
                    return function_call_rule(token, "");
                }
            }
            ///Returning expression
            else {
                if (token -> type == type_semicolon){
                    RUN_ERR_RETURN_MSG();
                }
                return_type = token_fetching(token, &(current_btree)->function_parameters.var_tree, in_function);
                print_function_retval_exp();
                print_function_end(current_btree->id);
                ///Checking for correct command ending syntax
                if (token->type != type_semicolon) {
                    SYNTAX_ERR_MSG();
                }
                ///Checking for type compatibility in return 
                if (return_type != current_btree->function_parameters.return_type) {
                
                    SEMANTICS_ERR_RUN_MSG();
                }
                else return segment_body_rule(token);
            }
        }        
    }
    ///Inside of a void function
    else if (par_stack->top->par_type == function_void_par) {
        if (!getNextToken(token)) {
            ///If void return syntax isn't "return;", an error occurs
            if (token->type != type_semicolon) {
                RUN_ERR_RETURN_MSG();
            }
            
            print_function_end(current_btree->id);
            if (in_function){
                return segment_body_rule(token);
            }
            else {
                return body_rule(token);
            }
            
        }
    }
    else {
        SYNTAX_ERR_MSG();
    }
    INTERNAL_ERR_MSG();
}

int function_call_rule(Token *token, char *variable_name) {
    bst_node_t *fetched_node;
    bst_node_t *var_node;
    paramList tmp_btree_param_list;
    paramList *btree_param_list = &tmp_btree_param_list;
    char param_variable[MAX_TMP_STRING_SIZE];
    int dataVal = -1;
    int callval = -1;
    ///Parameters not visited yet
    if (!call_param_visited) {
        global_id = token->attribute.string->str;
        // if (bst_search(&(global_btree), token->attribute.string->str) == NULL) {
        //     SEMANTICS_ERR_DEF_MSG();
        // }
        if (!getNextToken(token)){
            if (token -> type == type_leftPar){
                params_function_call_rule(token, variable_name);
            }
            else {SYNTAX_ERR_MSG();}
        }

       
    }
    ///Parameters visited
    else{
        call_param_visited = false;
        param_list_first(param_list);
        param_list_init(btree_param_list);
        param_list_copy(param_list, btree_param_list);       
         ///Resetting index count for function call/declare
        index_count = 0;
        ///Semantics check for correct variable types in function declaration and calling
        fetched_node = bst_search(&(global_btree), global_id);
        ///Write function
        if (!strcmp(global_id, "write")) {
            param_list_first(param_list);
            while (param_list->activeElement != NULL) {
                if (param_list->activeElement->type == type_variableID) {
                    var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->activeElement->name);
                    ///Undefined variable
                    if (var_node == NULL) {
                        SEMANTICS_ERR_VAR_MSG();
                    }
                }
                print_function_param_frame();
                if (param_list->activeElement->type == type_string) {
                    print_function_param_string(param_list->activeElement->name, 0);
                }
                else if (param_list->activeElement->type == type_integer) {
                    print_function_param_int(param_list->activeElement->int_value, 0);
                }
                else if (param_list->activeElement->type == type_float) {
                    print_function_param_float(param_list->activeElement->float_value, 0);
                }
                else if (param_list->activeElement->type == type_variableID) {
                    if (in_function) {
                        if (var_node->variable_data.is_param) {
                            sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                            print_function_param(param_variable, 0);
                        }
                        else {
                            print_function_param(param_list->activeElement->name, 0);
                        }
                    }
                    else {
                        print_function_param(param_list->activeElement->name, 0);
                    }
                }
                else if (param_list->activeElement->type == type_keyword && (!strcmp(param_list->activeElement->name,"null"))){
                    print_function_param_nil(0);
                }
                print_function_call("write");
                param_list->activeElement = param_list->activeElement->next;
            }
            param_list_dispose(param_list);
            if ((!getNextToken(token)) && (token->type != type_semicolon)) {

                SYNTAX_ERR_MSG();
            }
            else {
                if (in_function == true) {
                    return segment_body_rule(token);
                }
                else return body_rule(token);
            }
        }
        ///Strval function
        else if (!strcmp(global_id, "strval")) {
 
            ///Error if function call has more than one parameters or no parameters
            if ((param_list->firstElement->next != NULL) || (param_list->firstElement == NULL)) {
                SEMANTICS_ERR_RUN_MSG();
            }
            else if ((param_list -> firstElement ->type == type_integer) || (param_list-> firstElement -> type == type_float)){
                SEMANTICS_ERR_RUN_MSG();
            }
            else if (param_list -> firstElement -> type == type_variableID){
                //looking for variable which we call in function in
                var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->firstElement->name);
                ///Undefined variable
                if (var_node == NULL) {
                    SEMANTICS_ERR_VAR_MSG();
                }
                if ((var_node->variable_data.var_type == integer) || (var_node->variable_data.var_type == floatval)){
                    SEMANTICS_ERR_RUN_MSG();
                }
                print_function_param_frame();
                if (in_function) {
                    if (var_node->variable_data.is_param) {
                        sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                        print_function_param(param_variable, 0);
                    }
                    else {
                        print_function_param(param_list->firstElement->name, 0);
                    }
                    }
                    else {
                        print_function_param(param_list->firstElement->name, 0);
                    }
                    print_function_call("strval");
            }
            else {
                ///Error if parameter isn't of type string or null
                print_function_param_frame();
                if (param_list->firstElement->type == type_string) {
                    print_function_param_string(param_list->firstElement->name, 0);

                }
                else if ((param_list->firstElement->type == type_keyword) && (!strcmp(param_list->firstElement->name, "null"))) {
                    print_function_param_nil(0);
                }
                else {
                    SEMANTICS_ERR_RUN_MSG();
                }
                print_function_call("strval");
              
            }  
            param_list_dispose(param_list);
            if ((!getNextToken(token)) && (token->type != type_semicolon)) {

                SYNTAX_ERR_MSG();
            }
            else {
                if (asssign_function_to_variable) {
                    if (in_function){
                        var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->firstElement->name);
                        if (var_node->variable_data.is_param){
                            sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                            print_function_retval_assign(param_variable);
                        }
                        else{
                            print_function_retval_assign(variable_name);
                        }
                    }
                    
                    else{
                        print_function_retval_assign(variable_name);
                    }
                    asssign_function_to_variable = false;
                    
                }
                if (in_function == true) {
                    return segment_body_rule(token);
                }
                else return body_rule(token);
            } 

        }
        ///Readi, Reads, Readf fuctions
        else if (!strcmp(global_id, "readi") || !strcmp(global_id, "reads") || !strcmp(global_id, "readf")){
            if (param_list -> firstElement != NULL){
                SEMANTICS_ERR_RUN_MSG();
            }
            
            if ((!getNextToken(token)) && (token->type != type_semicolon)) {
                SYNTAX_ERR_MSG();
            }
            else {
                ///Generating code for reads/readf/readi
                print_function_param_frame();
                print_function_call(global_id);
                    ///Assigning function to variable
                if (asssign_function_to_variable) {
                    if (in_function){
                        var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->firstElement->name);
                        if (var_node->variable_data.is_param){
                            sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                            print_function_retval_assign(param_variable);
                        }
                        else{
                            print_function_retval_assign(variable_name);
                        }
                    }
                    
                    else{
                        print_function_retval_assign(variable_name);
                    }
                    asssign_function_to_variable = false;
                    
                }
                if (in_function == true) {
                    return segment_body_rule(token);
                }
                else return body_rule(token);
            }
        }
        ///Strlen, substrinng, ord, char and user defined functions
        else {
            ///Pushing function parameters to stack if functions hasn't been declard yet
            if (undefined_function == true) {
                param_list_first(btree_param_list);
                undefined_stack_push(undefined_stack, global_id, current_btree);
                undefined_stack->top->param_list = btree_param_list;
            }
            ///Ignoring parameter compatibility check if function is undefined (check will be run at the end of the program)
            else {
                param_list_first(param_list);
                param_list_first(fetched_node->function_parameters.param_list);
                while ((fetched_node->function_parameters.param_list->activeElement != NULL) && (param_list -> activeElement != NULL)) {
                    ///Types are matching
                    if (!strcmp( fetched_node->function_parameters.param_list->activeElement->name,"int")){
                        dataVal = type_integer;
                    }
                    else if (!strcmp( fetched_node->function_parameters.param_list->activeElement->name,"float")){
                        dataVal = type_float;
                    }
                    else if (!strcmp( fetched_node->function_parameters.param_list->activeElement->name,"string")){
                        dataVal = type_string;
                    }

                    if (param_list->activeElement->type == type_variableID) {
                        var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->activeElement->name);
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
                        }
                    }
                    ///Function call parameter isn't a variable
                    if ((param_list->activeElement->type != type_variableID) && (param_list->activeElement->type == dataVal)) {
                        ///Called function only has variables without type declaration, so we move the call stack by one and node stack by two
                        param_list->activeElement = param_list->activeElement->next;
                        fetched_node->function_parameters.param_list->activeElement = fetched_node->function_parameters.param_list->activeElement->next->next;
                    }
                    ///Function call parameter is a variable
                    else if ((param_list->activeElement->type == type_variableID) && (callval == dataVal)) {
                        param_list->activeElement = param_list->activeElement->next;
                        fetched_node->function_parameters.param_list->activeElement = fetched_node->function_parameters.param_list->activeElement->next->next; 
                    }
                    ///Types are different
                    else  {
                        SEMANTICS_ERR_RUN_MSG();
                    }
                }
                ///If the number of parameters isn't the same, one of the lists didn't cycle through to the end
                if ((fetched_node->function_parameters.param_list->activeElement != NULL) || (param_list->activeElement != NULL)) {
                    SEMANTICS_ERR_RUN_MSG(); 
                }
            }
            ///Strlen, Substring, Ord and Chr functions

            ///strlen function
            if (!strcmp(global_id, "strlen")) {
                print_function_param_frame();
                var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->firstElement->name);
                if (param_list->firstElement->type == type_variableID) {
                    if (in_function) {
                        if (var_node->variable_data.is_param) {
                            sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                            print_function_param(param_variable, 0);
                        }
                        else {
                            print_function_param(param_list->activeElement->name, 0);
                        }
                    }
                    else {
                        print_function_param(param_list->activeElement->name, 0);
                    }
                }
                else if (param_list->firstElement->type == type_string) {
                    print_function_param_string(param_list->firstElement->name, 0);
                }
                else {
                    INTERNAL_ERR_MSG();
                }
                print_function_call(global_id);
            }
            ///substring function
            else if (!strcmp(global_id, "substring")) {
                print_function_param_frame();
                param_list_first(param_list);
                
                ///param 1 (string)
                    if (param_list->firstElement->type == type_variableID) {
                        if (in_function) {
                            var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->firstElement->name);
                            if (var_node == NULL){
                                SEMANTICS_ERR_VAR_MSG();
                            }
                            if (var_node->variable_data.is_param) {
                                sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                                print_function_param(param_variable, 0);
                            }
                            else {
                                print_function_param(param_list->activeElement->name, 0);
                            }
                            }
                        else {
                            print_function_param(param_list->activeElement->name, 0);
                        }
                    
                    }
                    else if (param_list->firstElement->type == type_string) {
                        print_function_param_string(param_list->firstElement->name, 0);
                    }
                    else {
                        INTERNAL_ERR_MSG();
                    }
                    param_list->activeElement = param_list->activeElement->next;
                ///param 2 (int)
                    if (param_list->activeElement->type == type_variableID) {
                        if (in_function) {
                            var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->activeElement->name);
                            if (var_node->variable_data.is_param) {
                                sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                                print_function_param(param_variable, 1);
                            }
                            else {
                                print_function_param(param_list->activeElement->name, 1);
                            }
                        }
                        else {
                            print_function_param(param_list->activeElement->name, 1);
                    }
                        
                    }
                    else if (param_list->activeElement->type == type_integer) {
                        print_function_param_int(param_list->activeElement->int_value, 1);
                    }
                    else {
                        INTERNAL_ERR_MSG();
                    }
                    param_list->activeElement = param_list->activeElement->next;
                ///param 3 (int)
                    if (param_list->activeElement->type == type_variableID) {
                        if (in_function) {
                            var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->activeElement->name);
                            if (var_node->variable_data.is_param) {
                                sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                                print_function_param(param_variable, 2);
                            }
                            else {
                                print_function_param(param_list->activeElement->name, 2);
                            }
                        }
                        else {
                            print_function_param(param_list->activeElement->name, 2);
                        }
                    }
                    else if (param_list->activeElement->type == type_integer) {
                        print_function_param_int(param_list->activeElement->int_value, 2);
                    }
                    else {
                        INTERNAL_ERR_MSG();
                    }

                    print_function_call(global_id);
            }
            ///ord function
            else if (!strcmp(global_id, "ord")) {
                print_function_param_frame();
                if (param_list->firstElement->type == type_variableID) {
                    if (in_function) {
                        if (var_node->variable_data.is_param) {
                            sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                            print_function_param(param_variable, 0);
                        }
                        else {
                            print_function_param(param_list->activeElement->name, 0);
                        }
                    }
                    else {
                        print_function_param(param_list->activeElement->name, 0);
                    }

                }
                else if (param_list->firstElement->type == type_string) {
                    print_function_param_string(param_list->firstElement->name, 0);
                }
                else {
                    INTERNAL_ERR_MSG();
                }
                print_function_call(global_id);

            }
            ///chr function
            else if (!strcmp(global_id, "chr")) {
                print_function_param_frame();
                if (param_list->firstElement->type == type_variableID) {
                    if (in_function) {
                        if (var_node->variable_data.is_param) {
                            sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                            print_function_param(param_variable, 0);
                        }
                        else {
                            print_function_param(param_list->activeElement->name, 0);
                        }
                    }
                    else {
                        print_function_param(param_list->activeElement->name, 0);
                    }
                }
                else if (param_list->firstElement->type == type_integer) {
                    print_function_param_int(param_list->firstElement->int_value, 0);
                }
                else {
                    INTERNAL_ERR_MSG();
                }
                print_function_call(global_id);
            }
            ///User defined functions
            else {
                print_function_param_frame();
                int param_count = 0;
                param_list_first(param_list);
                ///Iterating through function call parameters, pushing to code generator
                while (param_list->activeElement != NULL) {
                    ///Integer value
                    if (param_list->activeElement->type == type_integer) {
                        print_function_param_int(param_list->activeElement->int_value, param_count);
                    }
                    ///Float value
                    else if (param_list->activeElement->type == type_float) {
                        print_function_param_float(param_list->activeElement->float_value, param_count);
                    }
                    ///String value
                    else if (param_list->activeElement->type == type_string) {
                        print_function_param_string( param_list->activeElement->name, param_count);
                    }
                    ///variable
                    else if (param_list->activeElement->type == type_variableID) {
                        if (in_function) {
                            var_node = bst_search(&(current_btree)->function_parameters.var_tree, param_list->activeElement->name);
                            if (var_node->variable_data.is_param) {
                                sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                                print_function_param(param_variable, param_count);
                            }
                            else {
                                print_function_param(param_list->activeElement->name, param_count);
                                }
                        }
                        else {
                            print_function_param(param_list->activeElement->name, param_count);
                        }
                    }
                    ///Null
                    else if ((param_list->activeElement->type == type_keyword) && (!strcmp(param_list->activeElement->name, "null"))) {
                        print_function_param_nil(param_count);
                    }
                    else {
                        INTERNAL_ERR_MSG();
                    }
                    param_list->activeElement = param_list->activeElement->next;
                    param_count++;
                }
                print_function_call(global_id);
            }
            param_list_dispose(param_list);
            undefined_function = false;

            if ((!getNextToken(token)) && (token->type != type_semicolon)) {

                SYNTAX_ERR_MSG();
            }
            else {
                
                ///Assigning function to variable
                if (asssign_function_to_variable) {
                    if (in_function){
                        if (var_node->variable_data.is_param){
                            sprintf(param_variable, "%%param%d", var_node->variable_data.param_index);
                            print_function_retval_assign(param_variable);
                        }
                        else{
                            print_function_retval_assign(variable_name);
                        }
                    }
                    
                    else{
                        print_function_retval_assign(variable_name);
                    }
                    asssign_function_to_variable = false;
                    
                }

               if (in_function == true) {
                    return segment_body_rule(token);
                }
                else return body_rule(token);
            }
        }

    }
    return -1;
}


int variable_rule(Token *token) {
    int return_type;
    char *id_temp = NULL;
    char param_variable[MAX_TMP_STRING_SIZE];
    bst_node_t *node_temp = NULL;
    bst_node_t *called_function = NULL;
    ///Fetching variable ID
    if (token->type == type_variableID) {
        id_temp = token->attribute.string->str;
        
        ///Checking for missing "=" in variable declaration
        if ((!getNextToken(token)) && (token->type != type_assign)) {
            SYNTAX_ERR_MSG();
        }

        ///Calling Precedence analysis for expression
        if (!getNextToken(token)) {
            ///Assigning function call to variable
            if (token->type == type_functionID) {
                called_function = bst_search(&(global_btree), token->attribute.string->str);
                node_temp = bst_search(&(current_btree)->function_parameters.var_tree, id_temp);

                ///Variable not declared yet
                if (node_temp == NULL) {
                    ///If both varaible and function are undefined yet
                    if (called_function == NULL) {
                        node_temp = new_node_var(integer, id_temp);
                    }
                    else {
                        node_temp = new_node_var(called_function->function_parameters.return_type, id_temp);
                    }
                    if (par_stack->top->par_type == if_par){
                        node_temp->variable_data.declared_in_if = true;
                    }
                    else{
                        node_temp->variable_data.declared_in_if = false;
                    }
                    bst_insert(&(current_btree)->function_parameters.var_tree, node_temp, id_temp);
                    print_var_declare(id_temp);
                }
                else{
                   if (node_temp -> variable_data.declared_in_if){
                        if (par_stack -> top ->par_type == else_par){
                            print_var_declare(id_temp);
                        }
                    }
                }
                ///Checking if function has been declared
                if (called_function != NULL) {
                    ///Error when assigning a void function to a variable
                    if (called_function->function_parameters.return_type == voidval) {
                        SEMANTICS_ERR_RUN_MSG();
                    }
                }
                ///Function not declared yet
                else {
                    undefined_function = true;
                    ///push_to_undefined;
                }
                asssign_function_to_variable = true;
                return function_call_rule(token, id_temp);
                ///Function calling
            }
            ///Expression
            else {
                node_temp = bst_search(&(current_btree)->function_parameters.var_tree, id_temp);  
                ///Calling expression parser
                return_type = token_fetching(token, &(current_btree)->function_parameters.var_tree, in_function);

                ///New variable not declared in btree, inserting
                if (node_temp == NULL ) {
                    node_temp = new_node_var(return_type, id_temp);
                    if (par_stack -> top -> par_type == if_par){
                        node_temp->variable_data.declared_in_if = true;
                    }
                    else{
                        node_temp->variable_data.declared_in_if = false;
                    }
                    bst_insert(&(current_btree)->function_parameters.var_tree, node_temp, id_temp);
                    print_var_declare(id_temp);
                }
                else {
                    if (node_temp -> variable_data.declared_in_if){
                        if (par_stack -> top ->par_type == else_par){
                            print_var_declare(id_temp);
                        }
                    }
                }

                //assigning value to variable after expression -- in code generator
                if (in_function){
                    if (node_temp->variable_data.is_param){
                        sprintf(param_variable, "%%param%d", node_temp->variable_data.param_index);
                        print_var_assign_exp(param_variable);
                    }
                    else{
                        print_var_assign_exp(id_temp);
                    }
                }
                else{
                    print_var_assign_exp(id_temp);
                }

                ///Ending variable rule with ";" (token fetched in predecence analysis)
                if (token->type != type_semicolon) {
                    SYNTAX_ERR_MSG();
                }
            }
        }
    }
    else {
        SYNTAX_ERR_MSG();
    }
    if (in_function == true) {
        return segment_body_rule(token);
    }
    else return body_rule(token);

    
    INTERNAL_ERR_MSG();
}


/**
 *@brief implementing rules for code end
 * <end> -> ?> <end>
 * <end> -> ε
*/
int end_rule(Token *token) {
    if (in_if == 1){
        SYNTAX_ERR_MSG();
    }
    if (token->type == type_endPrologue) {
        ///Checking for tokens after epilogue
        if (!getNextToken(token) && (token->type != type_EOF)) {
            SYNTAX_ERR_MSG();
        }
        else {
            ///Kontrola dosial nedefinovanych funkcii 
            check_undefined_calls(undefined_stack, &(global_btree));
            ///Generating code
            print_main_end();
            print_final_code();
            dispose_final_code();
            bst_dispose(global_btree);
            return 0;
        }
    }
    if (token->type == type_EOF)  {
        ///Kontrola dosial nedefinovanych funkcii 
        check_undefined_calls(undefined_stack, &(global_btree));
        ///Generating code
        print_main_end();
        print_final_code();
        dispose_final_code();
        bst_dispose(global_btree);
        return 0;
    }
    INTERNAL_ERR_MSG();
}

int right_curly_par_rule(Token *token) {
    if (par_stack->top->par_type == if_par) {
        in_if = true;
    }
    if ((par_stack->top->par_type == function_par) || (par_stack->top->par_type == function_void_par)) {
        ///Checking correct return command placement
        ///Return in if and else (in function true or false)
        ///OR
        ///Return in function (in if and else true or false)
        if (((return_in_if == true) && (return_in_else == true)) || (return_in_function == true)) {   
            in_function = false;
            current_btree = global_btree;
        }
        else {
            SEMANTICS_ERR_RUN_MSG();
        }
    }
    
    if (par_stack->top->par_type == else_par) {
        print_else_end(par_stack->top->if_count);
    }
    else if (par_stack->top->par_type == while_par){
        print_while_end(while_count);
    }
    ///We cannot pop if yet since we need its attributes for "else", we pop it in else_rule
    if (par_stack->top->par_type != if_par) {
        par_stack_pop(par_stack);
    }
    if (in_function == true) {
        return segment_body_rule(token);
    }
    else return body_rule(token);
}

void push_builtin_functions() {
    param_list_init(read_list);

    ///reads() : ?string
    bst_node_t *reads_node = new_node_func(string, read_list);
    reads_node->is_function = true;
    reads_node->id = "reads";
    reads_node->function_parameters.is_defined = true;
    reads_node->function_parameters.return_type = string;
    bst_insert(&(global_btree), reads_node, "reads");
    
    ///readf() : ?float
    bst_node_t *readf_node = new_node_func(floatval, read_list);
    readf_node->is_function = true;
    readf_node->id = "readf";
    readf_node->function_parameters.is_defined = true;
    readf_node->function_parameters.return_type = floatval;
    bst_insert(&(global_btree), readf_node, "readf");
    
    ///readi() : ?int
    bst_node_t *readi_node = new_node_func(integer, read_list);
    readi_node->is_function = true;
    readi_node->id = "readi";
    readi_node->function_parameters.is_defined = true;
    readi_node->function_parameters.return_type = integer;
    bst_insert(&(global_btree), readi_node, "readi");

    ///write(x) : void
    param_list_init(write_list);
    bst_node_t *write_node = new_node_func(voidval, write_list);
    write_node->is_function = true;
    write_node->id = "write";
    write_node->function_parameters.is_defined = true;
    write_node->function_parameters.return_type = voidval;    
    bst_insert(&(global_btree), write_node, "write");

    
    ///strval(term) : string
    param_list_init(strval_list);
    bst_node_t *strval_node = new_node_func(string, strval_list);
    strval_node->is_function = true;
    strval_node->id = "strval";
    strval_node->function_parameters.is_defined = true;
    strval_node->function_parameters.return_type = string;
    bst_insert(&(global_btree), strval_node, "strval");
 
    ///strlen(string $s) : int
    param_list_init(strlen_list);
    paramListElementPtr type_strlen_list_el = malloc(sizeof(struct paramListElement));
    type_strlen_list_el->type = type_dataTypeID;
    type_strlen_list_el->name = "string";
    strlen_list->firstElement = type_strlen_list_el;
    param_list_first(strlen_list);
    paramListElementPtr var_strlen_list_el = malloc(sizeof(struct paramListElement));
    var_strlen_list_el->type = type_variableID;
    var_strlen_list_el->name = "$s";
    strlen_list->activeElement->next = var_strlen_list_el;
    
    bst_node_t *strlen_node = new_node_func(integer, strlen_list);
    strlen_node->is_function = true;
    strlen_node->id = "strlen";
    strlen_node->function_parameters.is_defined = true;
    strlen_node->function_parameters.return_type = integer;
    bst_insert(&(global_btree), strlen_node, "strlen");

    
    ///substring(string $s, int $i, int $j) : string
    param_list_init(substring_list);
    paramListElementPtr type_substr_list_el1 = malloc(sizeof(struct paramListElement));
    type_substr_list_el1->type = type_dataTypeID;
    type_substr_list_el1->name = "string";
    substring_list->firstElement = type_substr_list_el1;
    param_list_first(substring_list);
    paramListElementPtr var_substr_list_el1 = malloc(sizeof(struct paramListElement));
    var_substr_list_el1->type = type_variableID;
    var_substr_list_el1->name = "$s";
    substring_list->activeElement->next = var_substr_list_el1;
    substring_list->activeElement = substring_list->activeElement->next;
    paramListElementPtr type_substr_list_el2 = malloc(sizeof(struct paramListElement));
    type_substr_list_el2->type = type_dataTypeID;
    type_substr_list_el2->name = "int";
    substring_list->activeElement->next = type_substr_list_el2;
    substring_list->activeElement = substring_list->activeElement->next;    
    paramListElementPtr var_substr_list_el2 = malloc(sizeof(struct paramListElement));
    var_substr_list_el2->type = type_variableID;
    var_substr_list_el2->name = "$i";
    substring_list->activeElement->next = var_substr_list_el2;
    substring_list->activeElement = substring_list->activeElement->next;
    paramListElementPtr type_substr_list_el3 = malloc(sizeof(struct paramListElement));
    type_substr_list_el3->type = type_dataTypeID;
    type_substr_list_el3->name = "int";
    substring_list->activeElement->next = type_substr_list_el3;
    substring_list->activeElement = substring_list->activeElement->next;
    paramListElementPtr var_substr_list_el3 = malloc(sizeof(struct paramListElement));
    var_substr_list_el3->type = type_variableID;
    var_substr_list_el3->name = "$j";
    substring_list->activeElement->next = var_substr_list_el3;
    
    bst_node_t *substring_node = new_node_func(string, substring_list);
    substring_node->is_function = true;
    substring_node->id = "substring";
    substring_node->function_parameters.is_defined = true;
    bst_insert(&(global_btree), substring_node, "substring");


    ///ord(string $c) : int
    param_list_init(ord_list);
    paramListElementPtr type_ord_list_el = malloc(sizeof(struct paramListElement));
    
    type_ord_list_el->name = "string";
    type_ord_list_el->type = type_dataTypeID;
    ord_list->firstElement = type_ord_list_el;
    ord_list->activeElement = type_ord_list_el;

    paramListElementPtr var_ord_list_el = malloc(sizeof(struct paramListElement));
    var_ord_list_el->name = "$c";
    var_ord_list_el->type = type_variableID;
    ord_list->activeElement->next = var_ord_list_el;

    bst_node_t *ord_node = new_node_func(integer, ord_list);
    ord_node->function_parameters.param_list = ord_list;
    ord_node->is_function = true;
    ord_node->id = "ord";
    ord_node->function_parameters.is_defined = true;
    ord_node->function_parameters.return_type = integer;
    bst_insert(&(global_btree), ord_node, "ord");


    ///chr(int $i) : string
    param_list_init(chr_list);
    paramListElementPtr type_chr_list_el = malloc(sizeof(struct paramListElement));
    type_chr_list_el->name = "int";
    type_chr_list_el->type = type_dataTypeID;
    chr_list->firstElement = type_chr_list_el;
    chr_list->activeElement = type_chr_list_el;

    paramListElementPtr var_chr_list_el = malloc(sizeof(struct paramListElement));
    var_chr_list_el->type = type_variableID;
    var_chr_list_el->name = "$i";
    chr_list->activeElement->next = var_chr_list_el;

    bst_node_t *chr_node = new_node_func(string, chr_list);
    chr_node->function_parameters.param_list = chr_list;
    chr_node->is_function = true;
    chr_node->id = "chr";
    chr_node->function_parameters.is_defined = true;
    chr_node->function_parameters.return_type = string;
    bst_insert(&(global_btree), chr_node, "chr");
}


int main () {
    ///Initializating global binary tree
    bst_init(&(global_btree));
    paramList aux;
    paramList *aux_list = &aux;
    bst_node_t *start_node = new_node_func(voidval, aux_list);
    bst_insert(&(global_btree), start_node, "global_btree");
    bst_init(&(global_btree)->function_parameters.var_tree);
    current_btree = global_btree;
    ///
    param_list_init(param_list);
    par_stack_init(par_stack);
    undefined_stack_init(undefined_stack);
    
    Token *token = malloc(sizeof(Token));
    push_builtin_functions();
    token -> type = type_empty;

    par_stack_push(par_stack, function_void_par, if_count, while_count);

    int result = getNextToken(token);
    result = prog_rule(token);
    return result;
}
