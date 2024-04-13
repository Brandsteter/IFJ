/**
 *Project: Implementácia prekladača imperatívneho jazyka IFJ22
 *@file expression_pars.c
 *@brief parsing expressions in IFJ22 code  
 *@author Lucia Šírová  <xsirov01>
 *@author Daniel Blaško <xblask05>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexical_analysis.h"
#include "expression_pars.h"
#include "err_handling.h"
#include "param_list.h"
#include "symtable.h"
#include "expression_stack.h"
#include "code_generator.h"


#define TABLE_SIZE 15




paramList tmpL;
paramList *expression_list = &tmpL;
Stack tmpS;
Stack *eval_stack = &tmpS;
bool token_fetching_start = false;
bool was_reduced = false;


int precedence_table[TABLE_SIZE][TABLE_SIZE] = {
//    0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  
    {RD, RD, RD, RD, ER, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 0  *
    {RD, RD, RD, RD, ER, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 1  /
    {SH, SH, RD, RD, ER, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 2  +
    {SH, SH, RD, RD, ER, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 3  -
    {ER, ER, ER, ER, RD, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 4  .
    {SH, SH, SH, SH, SH, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 5  <
    {SH, SH, SH, SH, SH, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 6  <=
    {SH, SH, SH, SH, SH, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 7  >
    {SH, SH, SH, SH, SH, RD, RD, RD, RD, RD, RD, SH, RD, SH, RD}, // 8  >=
    {SH, SH, SH, SH, SH, SH, SH, SH, SH, RD, RD, SH, RD, SH, RD}, // 9  ===
    {SH, SH, SH, SH, SH, SH, SH, SH, SH, RD, RD, SH, RD, SH, RD}, // 10 !==
    {SH, SH, SH, SH, SH, SH, SH, SH, SH, SH, SH, SH, EQ, SH, RD}, // 11 (
    {RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, ER, RD, ER, RD}, // 12 )
    {RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, ER, RD, ER, RD}, // 13 id 
    {SH, SH, SH, SH, SH, SH, SH, SH, SH, SH, SH, SH, ER, SH, DN}  // 14 $  
};


///Returning operator types based on their token type
int return_operator_val(Token token) {
    if(token.type == type_multiplication) {
        return MUL_type;
    }
    else if(token.type == type_division) {
        return DIV_type;
    }
    else if(token.type == type_addition) {
        return ADD_type;
    }
    else if(token.type == type_subtraction) {
        return SUB_type;
    }
    else if(token.type == type_period) {
        return CONCAT_type;
    }
    else if(token.type == type_lessThan) {
        return LESSER_type;
    }
    else if(token.type == type_lessOrEqualThan) {
        return LESS_EQ_type;
    }
    else if(token.type == type_greaterThan) {
        return GREATER_type;
    }
    else if(token.type == type_greaterOrEqualThan) {
        return GREATER_EQ_type;
    }
    else if(token.type == type_equal) {
        return EQ_type;
    }
    else if(token.type == type_notEqual) {
        return NOT_EQ_type;
    }
    else if(token.type == type_leftPar) {
        return LEFT_PAR_type;
    }
    else if(token.type == type_rightPar) {
        return RIGHT_PAR_type;
    }
    else if((token.type == type_string) || (token.type == type_integer)
           || (token.type == type_float) || (token.type == type_variableID) 
           || (token.type == type_functionID) || (token.type == type_null)) {
        return ID_type;
    }
    else {
        
        INTERNAL_ERR_MSG()};
}

int token_fetching(Token *token, bst_node_t **btree, bool in_function) {
    ///Token fetching and returning operator value
    ///Initializating the expression list for the first tfunction iteration
    if (token_fetching_start == false) {
        param_list_init(expression_list);
        token_fetching_start = true;
    }
    Token tkn_np;
    tkn_np.type = token->type;
    tkn_np.attribute = token->attribute;
    int op_type;
    bool is_var = false;

    ///Token is a variable, searching for variable in tree and assigning attributes
    if (tkn_np.type == type_variableID) {
        bst_node_t *seeked_var = bst_search(btree, tkn_np.attribute.string->str);
        if (seeked_var == NULL){
            SEMANTICS_ERR_VAR_MSG();
        }
        else {
            is_var = true;
            tkn_np.attribute = token->attribute;
            if (seeked_var->variable_data.var_type == integer){
                //tkn_np.attribute.integerNum = seeked_var ->variable_data.int_value;
                tkn_np.type = type_integer;
                op_type = return_operator_val(tkn_np);
            }
            else if (seeked_var->variable_data.var_type == floatval){
                //tkn_np.attribute.floatNum = seeked_var ->variable_data.float_value;
                tkn_np.type = type_float;
                op_type = return_operator_val(tkn_np);
            }
            else if (seeked_var->variable_data.var_type == string){
                //tkn_np.attribute.string -> str = seeked_var ->variable_data.string_value;
                tkn_np.type = type_string;
                op_type = return_operator_val(tkn_np);
            }
            else if (seeked_var->variable_data.var_type == nullval) {
                tkn_np.type = type_null;
                op_type = return_operator_val(tkn_np);
            }
        }
    }
    else if ((tkn_np.type == type_keyword) && (!strcmp(tkn_np.attribute.string->str, "null"))) {
        tkn_np.type = type_null;
        op_type = return_operator_val(tkn_np);
    }
    else {
        tkn_np.attribute = token->attribute;
        tkn_np.type = token->type;
        op_type = return_operator_val(tkn_np);
    }

    ///Inserting expression operator

    param_list_insert(expression_list, tkn_np);
    expression_list->activeElement->is_var = is_var;
    if (is_var){
        expression_list->activeElement->name = tkn_np.attribute.string->str;
    }
    expression_list->activeElement->op_type = op_type;
    ///Fetching new token, breaking out if token is ";" or ")" (end of expression)
    if (!getNextToken(token)) {
        if ((token->type != type_semicolon) && (token->type != type_leftCurlyPar)) {
            return(token_fetching(token, btree, in_function));
        }
        else return (expression_eval(in_function, btree));
    }
    else {SYNTAX_ERR_MSG();}
}

int expression_eval(bool in_function, bst_node_t **btree) {
    bool eval_done = false;
    int return_type = -1;
    //int expression_counter = 0;
    ///Creating stack for expression parsing
    stack_init(eval_stack);
    ///Inserting $ into stack top and list end
    ///Manual insertion for eval_stack, insertion via function for expression_list
    Token tkn_top;
    tkn_top.type = -1;

    
    param_list_last(expression_list);
    param_list_insert(expression_list, tkn_top);
    stack_push(eval_stack, -1, OTHER_type, 0, 0.0, "", false);
    expression_list->activeElement->op_type = OTHER_type;

    ///Parsing expression until the original token list only has empty $ inside
    param_list_first(expression_list);
    do {
        int value_row;
        ///If stack top is of type E, returning 2nd stack element, else returning top
        stackElementPtr *temp_element = stack_top(eval_stack);
        stackElementPtr *temp_element2;

        if (temp_element->op_type == EXPRESSION_type) {
            ///If both top and 2nd element are of type E
            if (temp_element -> next != NULL){
                temp_element2 = stack_2nd(eval_stack);
            }
            if (temp_element2->op_type == EXPRESSION_type) {
                INTERNAL_ERR_MSG();
            }
            else {
                value_row = temp_element2->op_type;  
            }  
        }
        ///Top element is not of type E
        else {
            value_row = temp_element->op_type;
        }

        int value_column = expression_list -> activeElement -> op_type;
        int table_result = precedence_table[value_row][value_column];
        
        ///Operator table operations
        
        ///SHIFT operation
        if (table_result == SH) {
            stack_push(eval_stack, expression_list->firstElement->type, expression_list->firstElement->op_type, expression_list->firstElement->int_value, expression_list->firstElement->float_value, expression_list->firstElement->name, expression_list->firstElement->is_var); 
            param_list_delete_first(expression_list);
        }
        ///REDUCE operation
        else if (table_result == RD) {
            return_type = reduce_function(in_function, btree);
        }
        ///EQUAL operation
        else if (table_result == EQ) {
            ///Empty parenthesis pair "()"
            if (temp_element->op_type == LEFT_PAR_type) {
                SYNTAX_ERR_MSG();
            }
            else if ((temp_element->op_type == EXPRESSION_type) && (temp_element2->op_type == LEFT_PAR_type)) {
                stack_pop(eval_stack);
                stack_pop(eval_stack);
                stack_push(eval_stack, temp_element->type, temp_element->op_type, temp_element->int_value, temp_element->float_value, temp_element->name, temp_element->is_var);
                param_list_delete_first(expression_list);
            }
        }
        ///ERROR
        else if (table_result == ER) {
            SYNTAX_ERR_MSG();
        }
        ///DONE
        else if (table_result == DN) {
            print_stack_pop();
            param_list_dispose(expression_list);
            stack_dispose(eval_stack);
            token_fetching_start = false;
            eval_done = true;
        }
    } while (!eval_done);
    return return_type;
}


int reduce_function(bool in_function, bst_node_t **btree) {
    int return_type = -1;
    stackElementPtr *operator1 = (stackElementPtr*) malloc(sizeof(stackElementPtr));
    stackElementPtr *operator2 = (stackElementPtr*) malloc(sizeof(stackElementPtr));
    stackElementPtr *operator3 = (stackElementPtr*) malloc(sizeof(stackElementPtr));

    ///Checking if single-operator or triple-operator operation will be done
    ///Fetching operator 3 first (we are taking operators from the stack top)
    operator3->op_type = eval_stack->top->op_type;
    operator3->type = eval_stack->top->type;
    operator3->is_var = eval_stack->top->is_var;

    if (operator3->is_var){
        operator3->name = eval_stack->top-> name;
    }
    if (operator3->type == type_integer) {
        operator3->int_value = eval_stack->top->int_value;
    }
    else if (operator3->type == type_float) {
        operator3->float_value = eval_stack->top->float_value;
    }
    else if (operator3->type == type_string) {
        operator3->name = eval_stack->top->name;
    }

    stack_pop(eval_stack);
    ///single-operator operation
    if (operator3->op_type == ID_type) {
        /// E -> i
        operator3->op_type = EXPRESSION_type;
        ///generate_code(operator3, op_type);
      
        // if (operator3 -> is_var == true){
        //     stack_push(eval_stack, operator3->type, operator3->op_type,0, 0.0, operator3->name, operator3->is_var);
        // }
        // else{
        //     stack_push(eval_stack, operator3->type, operator3->op_type, operator3->int_value, operator3->float_value, operator3->name, operator3->is_var);
        // }
        stack_push(eval_stack, operator3->type, operator3->op_type, operator3->int_value, operator3->float_value, operator3->name, operator3->is_var);

        return_type = reduction_semantics_check_single_op(operator3, in_function, btree);

        free(operator1);
        free(operator2);
        free(operator3);
    }
    ///triple-operator operation
    else {
        ///Fetching operator 1 and 2
        operator2->op_type = eval_stack->top->op_type;
        stack_pop(eval_stack);
        operator1->op_type = eval_stack->top->op_type;
        operator1->type = eval_stack->top->type;
        stack_pop(eval_stack);

        ///Checking if both operators 1 and 3 are of EXPRESSION type
        if ((operator1->op_type != EXPRESSION_type) || (operator3->op_type != EXPRESSION_type)) {
            SYNTAX_ERR_MSG();
        }
        ///Operators are of correct type
        else {
            //Operator types are enumerated from 0 to 10
            if ((operator2->op_type >= 0) && (operator2->op_type <= 10)) {
                return_type = reduction_semantics_check(operator1, operator2, operator3);
            }
            else {SYNTAX_ERR_MSG();}
        }
    }
    return return_type;
}


///for rule E -> i
int reduction_semantics_check_single_op(stackElementPtr *op3, bool in_function, bst_node_t **btree){
    char param_variable[MAX_TMP_STRING_SIZE];

    var_type eval_type = -1;
    bst_node_t *variable_node;
    if (op3 -> type == type_integer){
        eval_type = integer;
    }
    else if (op3 -> type == type_float){
        eval_type = floatval;
    }
    else if (op3 -> type == type_string){
        eval_type= string;
    }
    else if ((op3 -> type == type_null)){
        eval_type = nullval;
    }

    ///If E -> i was the only operation, we push it to the code generator
    //checking what data type we need for code generator
    if (op3->is_var){
        if (in_function == false){
            print_pushs(op3->name);
        }
        else{
            variable_node = bst_search(btree, op3 -> name);
            if (variable_node->variable_data.is_param){
                sprintf(param_variable, "%%param%d", variable_node->variable_data.param_index);
                print_pushs(param_variable);
            }
            else {
                print_pushs(op3->name);
            }
        }    
    }
    else{
        if (op3 -> type == type_integer){
            print_pushs_int(op3->int_value);
        }
        else if (op3 -> type == type_float){
            print_pushs_float(op3->float_value);
        }
        else if (op3 -> type == type_string){
            print_pushs_string(op3->name);
        }
        else if (op3 -> type == type_null) {
            print_pushs_nil();
        }
    }
   
    return eval_type;
    
}

int reduction_semantics_check(stackElementPtr *op1, stackElementPtr *op2, stackElementPtr *op3) {
    var_type eval_type = -1;
    ///MULTIPLICATION (E -> E*E)
    if (op2->op_type == MUL_type) {
        if ((op1->type == type_integer) && (op3->type == type_integer)) {
            eval_type = integer;
            op1->type = type_integer;
            op1->op_type = EXPRESSION_type;
            print_stack_mul();
            stack_push(eval_stack, op1->type, op1->op_type, op1->int_value, 0.0, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_float) && (op3->type == type_float)) {
            eval_type = floatval;
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_mul();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_integer) && (op3->type == type_float)) {
            eval_type = floatval;
            ///retyping op1 to float in code generator
            print_stack_op1_to_float ();
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_mul();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_float) && (op3->type == type_integer)) {
            eval_type = floatval;
            ///retyping op3 to float in code generator
            print_stack_op3_to_float ();;
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_mul();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        ///Not integer or float
        else {RUN_ERR_TYPE_MSG();}
    }

    ///DIVISION (E -> E/E)
    else if (op2->op_type == DIV_type) {
        eval_type = floatval;
        ///Checking for incorrect types
        if ((op1->type == type_string) || (op3->type == type_string) || (op1->type == type_null) || (op3->type == type_null)) {
            RUN_ERR_TYPE_MSG();
        }
        ///Retyping op1 and op3 to float if they are of type integer
        if (op1->type == type_integer) {
            print_stack_op1_to_float();
            op1->type = type_float;
            op1->float_value = op1->int_value;
        }
        if (op3->type == type_integer) {
            print_stack_op3_to_float();
            op3->type = type_float; 
            op3->float_value = op3->int_value;           
        }
        ///Evaluating
        ///generate_code(op1, op2, op3, eval_type);
        op1->type = type_float;
        op1->op_type = EXPRESSION_type;
        print_stack_div();
        stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
        free(op1);
        free(op2);
        free(op3);
    }

    ///ADDITION (E -> E+E)
    else if (op2->op_type == ADD_type) {
        if ((op1->type == type_integer) && (op3->type == type_integer)) {
            eval_type = integer;
            op1->type = type_integer;
            op1->op_type = EXPRESSION_type;
            ///Generates code
            print_stack_add();
            stack_push(eval_stack, op1->type, op1->op_type, op1->int_value, 0.0, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_float) && (op3->type == type_float)) {
            eval_type = floatval;
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_add();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_integer) && (op3->type == type_float)) {
            eval_type = floatval;
            ///Retyping op1 to float
            print_stack_op1_to_float();
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_add();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_float) && (op3->type == type_integer)) {
            eval_type = floatval;
            ///Retyping op3 to float
            print_stack_op3_to_float();
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_add();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        ///not integer or float
        else {RUN_ERR_TYPE_MSG();}
    }

    ///SUBTRACTION (E -> E-E)
    else if (op2->op_type == SUB_type) {
        if ((op1->type == type_integer) && (op3->type == type_integer)) {
            eval_type = integer;
            ///Evaluates and pushes back to evaluation list
            op1->type = type_integer;
            op1->op_type = EXPRESSION_type;
            print_stack_sub();
            stack_push(eval_stack, op1->type, op1->op_type, op1->int_value, 0.0, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_float) && (op3->type == type_float)) {
            eval_type = floatval;
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_sub();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_integer) && (op3->type == type_float)) {
            eval_type = floatval;
            ///Retyping op1 to float
            print_stack_op1_to_float();
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_sub();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        else if ((op1->type == type_float) && (op3->type == type_integer)) {
            eval_type = floatval;
            ///Retyping op3 to float
            print_stack_op3_to_float();
            op1->type = type_float;
            op1->op_type = EXPRESSION_type;
            print_stack_sub();
            stack_push(eval_stack, op1->type, op1->op_type, 0, op1->float_value, "", op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
        ///not integer or float
        else {RUN_ERR_TYPE_MSG();}
    }

    ///CONCATENATION (E -> E.E)
    else if (op2->op_type == CONCAT_type) {
        ///NOT OF TYPE STRING 
        if ((op1->type != type_string) || (op3->type != type_string)) {
                RUN_ERR_TYPE_MSG();
        }
        else {
            eval_type = string;
            op1->type = type_string;
            op1->op_type = EXPRESSION_type;
            ///Generating code and pushing reduced Expression back to stack
            print_stack_concat();
            stack_push(eval_stack, op1->type, op1->op_type, 0, 0.0, op1->name, op1->is_var);
            free(op1);
            free(op2);
            free(op3);
        }
    }

    /// LESSER THAN           (E -> E < E)
    /// LESSER OR EQUAL THAN  (E -> E <= E)
    /// GREATER THAN          (E -> E > E)
    /// GREATER OR EQUAL THAN (E -> E >= E)
    else if ((op2->op_type == LESSER_type) || (op2->op_type == LESS_EQ_type) || (op2->op_type == GREATER_type) || (op2->op_type == GREATER_EQ_type)) {
        eval_type = boolval;
        ///Error - comparing string and number
        if (((op1->type == type_string) && ((op3->type == type_integer) || (op3->type == type_float))) 
            || (((op1->type == type_integer) || (op1->type == type_float)) && (op3->type == type_string))) {
                RUN_ERR_TYPE_MSG();
        }
        else {
            ///Float and integer comparison, retyping required
            if ((op1->type == type_integer) && (op3->type == type_float)) {
                print_stack_op1_to_float ();
            }
            else if ((op1->type == type_float) && (op3->type == type_integer)) {
                print_stack_op3_to_float ();
            }
            ///generate_code(op1, op2, op3, eval_type);
            op1->type = type_bool;
            op1->op_type = EXPRESSION_type;

            ///Code generator for expressions 
            if (op2->op_type == LESSER_type) {
                print_stack_less_than();
            }
            else if (op2->op_type == LESS_EQ_type) {
                print_stack_less_equals();
            }
            else if (op2->op_type == GREATER_type) {
                print_stack_greater_than();
            }
            else{ ///Greater than
                print_stack_greater_equals();
            }

            stack_push(eval_stack, op1->type, op1->op_type, 0, 0.0, "", op1->is_var);
        }
    }

    ///EQUAL (E -> E === E)
    ///NOT EQUAL (E -> E !== E) 
    else if ((op2->op_type == EQ_type) || (op2->op_type == NOT_EQ_type)) {
        eval_type = boolval;
        if (op2->op_type == EQ_type) {
            ///Comparing different types
            if (op1->type != op3->type) {
                print_stack_pop();
                print_stack_pop();
                print_stack_push_false();
            }
            ///Comparing values of the same type
            else {
                print_stack_equals();
            }
        }
        else { ///Not EQUAL
            ///Comparing different types
            if (op1->type != op3->type) {
                print_stack_pop();
                print_stack_pop();
                print_stack_push_true();
            }
            else {
                print_stack_not_equals();
            }
        }
        ///generate_code(op1, op2, op3, eval_type);
        op1->type = type_bool;
        op1->op_type = EXPRESSION_type;
        stack_push(eval_stack, op1->type, op1->op_type, 0, 0.0, "", op1->is_var);
        free(op1);
        free(op2);
        free(op3);
    }
    else {
        SYNTAX_ERR_MSG()}

    return eval_type;
}
