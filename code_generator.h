/**
 * Project: Implementácia prekladača imperatívneho jazyka IFJ22
 * @brief Code generator header.
 *
 * @author David Brandšteter <xbrand13>
 */
#include <stdio.h>
#include <stdbool.h>

#include "dynamic_string.h"
#include "lexical_analysis.h"

#ifndef CODE_GENERATOR
#define CODE_GENERATOR

/**
 * Initiates dynamic string of the final IFJcode22.
 */
void init_final_code ();

/**
 * Prints dynamic string of the final IFJcode22 to stdout.
*/
void print_final_code ();

/**
 * Disposes of the dynamic string containing final IFJcode22 code.
*/
void dispose_final_code ();

/**
 * Prints IFJ file header and built-in functions.
 */
void print_start ();

/**
 * Prints start of main function.
 */
void print_main ();

/**
 * Prints end of main function.
 */
void print_main_end ();

/**
 * Prints start of a function.
 * @param fun_id Function identifier.
 */
void print_function_start (char *fun_id);

/**
 * Creates a temporary-frame for function parameters
 */
void print_function_param_frame ();

/**
 * Prints a parameter assign for a function.
 * @param param_id Variable identifier for param.
 * @param param_index index of a parameter (first is 0).
 */
void print_function_param (char *param_id, int param_index);

/**
 * Prints an assign of a term (int) to a function parameter.
 * @param term_val Value of term.
 * @param param_index index of a parameter (first is 0).
 */
void print_function_param_int (int term_val, int param_index);

/**
 * Prints an assign of a term (float) to a function parameter.
 * @param term_val Value of term.
 * @param param_index index of a parameter (first is 0).
 */
void print_function_param_float (float term_val, int param_index);

/**
 * Prints an assign of a term (string) to a function parameter.
 * @param term_val Value of term.
 * @param param_index index of a parameter (first is 0).
 */
void print_function_param_string (char *term_val, int param_index);

/**
 * Prints an assign of NULL value to a function parameter.
 * @param param_index index of a parameter (first is 0).
 */
void print_function_param_nil (int param_index);

void print_function_retval_exp ();

void print_function_retval_assign (char *var_id);

/**
 * Prints a default return value of type INT in a function
 */
void print_function_retval_int ();

/**
 * Prints a default return value of type FLOAT in a function
 */
void print_function_retval_float ();

/**
 * Prints a default return value of type STRING in a function
 */
void print_function_retval_string ();

/**
 * Prints a default return value of type NULL in a function
 */
void print_function_retval_nil ();

/**
 * Prints end of a function.
 * @param fun_id Function identifier.
 */
void print_function_end (char *fun_id);

/**
 * Prints call instruction of a function.
 * @param fun_id Function identifier.
 */
void print_function_call (char *fun_id);

/**
 * Prints declaration of a variable.
 * @param var_id Variable identifier.
 */
void print_var_declare (char *var_id);

/**
 * Prints an assign of a value of a type INTEGER to a variable.
 * @param var_id Variable identifier.
 * @param data Variable data.
 */
void print_var_assign_int (char *var_id, int data);

/**
 * Prints an assign of a value of a type FLOAT to a variable.
 * @param var_id Variable identifier.
 * @param data Variable data.
 */
void print_var_assign_float (char *var_id, float data);

/**
 * Converts a string into IFJcode22 syntax and prints an assign of a value type STRING to a variable.
 * @param var_id Variable indentifier.
 * @param data Variable data.
 */
void print_var_assign_string (char *var_id, char *data);

/**
 * Prints an assign of a NULL value to a variable.
 * @param var_id Variable identifier.
 */
void print_var_assing_nil (char *var_id);


/**
 * Prints while start LABEL, after this, use stack operations to result the expression in the condition.
 * @param fun_id Function identifier.
 * @param index While index.
 */
void print_while_start (int index);

/**
 * Prints a comparisson of the result of the expression and exits the loop if false.
 * @param fun_id Function identifier.
 * @param index While index.
 */
void print_while_check (int index);

/**
 * Prints a JUMP command back to start of the while AND a while exit LABEL
 * @param fun_id Function identifier.
 * @param index While index.
 */
void print_while_end (int index);

/**
 * Prints IF start.
 * For correct functioning, have the result of a expression ready at the top of the stack before calling this.
 * @param fun_id Function identifier.
 * @param index If index.
 */
void print_if_start (int index);

/**
 * Prints the end LABEL of IF.
 * @param fun_id Funciton identifier.
 * @param index If index.
 */
void print_if_end (int index);

/**
 * Prints the end LABEL of ELSE.
 * For correct functioning, call this function at the end of IF if ELSE isn't present.
 * Otherwise, call this function at the end of ELSE.
 * @param fun_id Function identifier.
 * @param index IF index
 */
void print_else_end (int index);
// STACK OPERATIONS

void print_stack_add ();
void print_stack_sub ();
void print_stack_mul ();
void print_stack_div ();
void print_stack_concat ();
void print_stack_push_true();
void print_stack_push_false();
void print_stack_equals ();
void print_stack_not_equals ();
void print_stack_less_than ();
void print_stack_less_equals ();
void print_stack_greater_than ();
void print_stack_greater_equals ();
void print_stack_op3_to_float ();
void print_stack_op3_to_int ();
void print_stack_op1_to_float ();
void print_stack_op1_to_int ();
void print_pushs (char *var_id);
void print_pushs_int (int data);
void print_pushs_float (float data);
void print_pushs_string (char *data);
void print_pushs_nil ();
void print_stack_pop ();
void print_var_assign_exp (char *var_id);


#endif