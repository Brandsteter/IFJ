/**
 * Project: Implementácia prekladača imperatívneho jazyka IFJ22
 *
 * @brief Code generator.
 *
 * @author David Brandšteter <xbrand13>
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dynamic_string.h"
#include "code_generator.h"
#include "lexical_analysis.h"

#define MAX_CHAR 50
#define CODE(...) generate_code(__VA_ARGS__)

dynamicStr str;
dynamicStr *final_code = &str;

void init_final_code (){
  dynamicStrInit(final_code);
}

void print_final_code () {
  printf("%s", final_code->str);
}

void dispose_final_code (){
  dynamicStrDispose(final_code);
}

/**
 * @brief Adds a piece of IFJcode22 code into the final dynamic string
 * 
 * @param final_code final code
 * @param code added piece of code
 */
void generate_code(dynamicStr *final_code, char *code){
  dynamicStrAddStr(final_code, code);
} 

void print_start ()
{
  CODE(final_code, ".IFJcode22\n");
  
  CODE(final_code, "DEFVAR GF@$input_int\n");
  CODE(final_code, "MOVE GF@$input_int string@awaiting\\032input\\032(int):\\032\n");
  CODE(final_code, "DEFVAR GF@$input_float\n");
  CODE(final_code, "MOVE GF@$input_float string@awaiting\\032input\\032(float):\\032\n");
  CODE(final_code, "DEFVAR GF@$input_string\n");
  CODE(final_code, "MOVE GF@$input_string string@awaiting\\032input\\032(string):\\032\n");

  CODE(final_code, "DEFVAR GF@%tmp1\n");
  CODE(final_code, "DEFVAR GF@%tmp2\n");
  CODE(final_code, "DEFVAR GF@%tmp3\n");

  CODE(final_code, "DEFVAR GF@%expression\n");

  CODE(final_code, "JUMP main\n");



  // readi
  CODE(final_code, "\n# Function READI:\n");
  CODE(final_code, "LABEL $readi\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 nil@nil\n");
  CODE(final_code, "DEFVAR LF@tmp\n");
  CODE(final_code, "DEFVAR LF@type\n");
  CODE(final_code, "WRITE GF@$input_int\n");
  CODE(final_code, "READ LF@tmp int\n");
  CODE(final_code, "TYPE LF@type LF@tmp\n");
  CODE(final_code, "PUSHS LF@type\n");
  CODE(final_code, "PUSHS string@int\n");
  CODE(final_code, "JUMPIFNEQS readi_return\n");
  CODE(final_code, "MOVE LF@%retval0 LF@tmp\n");
  CODE(final_code, "LABEL readi_return\n");
  CODE(final_code, "PUSHS LF@%retval0\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN \n");

  // readf
  CODE(final_code, "\n# Function READF:\n");
  CODE(final_code, "LABEL $readf\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 nil@nil\n");
  CODE(final_code, "DEFVAR LF@tmp\n");
  CODE(final_code, "DEFVAR LF@type\n");
  CODE(final_code, "WRITE GF@$input_float\n");
  CODE(final_code, "READ LF@tmp float\n");
  CODE(final_code, "TYPE LF@type LF@tmp\n");
  CODE(final_code, "PUSHS LF@type\n");
  CODE(final_code, "PUSHS string@float\n");
  CODE(final_code, "JUMPIFNEQS readf_return\n");
  CODE(final_code, "MOVE LF@%retval0 LF@tmp\n");
  CODE(final_code, "LABEL readf_return\n");
  CODE(final_code, "PUSHS LF@%retval0\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN \n");

  // reads
  CODE(final_code, "\n# Function READS:\n");
  CODE(final_code, "LABEL $reads\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 nil@nil\n");
  CODE(final_code, "DEFVAR LF@tmp\n");
  CODE(final_code, "DEFVAR LF@type\n");
  CODE(final_code, "WRITE GF@$input_string\n");
  CODE(final_code, "READ LF@tmp string\n");
  CODE(final_code, "TYPE LF@type LF@tmp\n");
  CODE(final_code, "PUSHS LF@type\n");
  CODE(final_code, "PUSHS string@string\n");
  CODE(final_code, "JUMPIFNEQS reads_return\n");
  CODE(final_code, "MOVE LF@%retval0 LF@tmp\n");
  CODE(final_code, "LABEL reads_return\n");
  CODE(final_code, "PUSHS LF@%retval0\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN \n");

  // write
  CODE(final_code, "\n# Function WRITE:\n");
  CODE(final_code, "LABEL $write\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@write_var\n");
  CODE(final_code, "MOVE LF@write_var LF@%param0\n");
  CODE(final_code, "DEFVAR LF@nil_check\n");
  CODE(final_code, "TYPE LF@nil_check LF@%param0\n");
  CODE(final_code, "PUSHS string@nil\n");
  CODE(final_code, "PUSHS LF@nil_check\n");
  CODE(final_code, "JUMPIFNEQS not_nil\n");
  CODE(final_code, "MOVE LF@write_var string@\n");
  CODE(final_code, "LABEL not_nil\n");
  CODE(final_code, "WRITE LF@write_var\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN\n");

  // ord
  CODE(final_code, "\n# Function ORD:\n");
  CODE(final_code, "LABEL $ord\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "DEFVAR LF@len_check\n");
  CODE(final_code, "STRLEN LF@len_check LF@%param0\n");
  CODE(final_code, "PUSHS LF@len_check\n");
  CODE(final_code, "PUSHS int@0\n");
  CODE(final_code, "MOVE LF@%retval0 int@0\n");
  CODE(final_code, "JUMPIFEQS len_zero\n");
  CODE(final_code, "STRI2INT LF@%retval0 LF@%param0 int@0\n");
  CODE(final_code, "LABEL len_zero\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN\n");

  // chr
  CODE(final_code, "\n# Function CHR:\n");
  CODE(final_code, "LABEL $chr\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "INT2CHAR LF@%retval0 LF@%param0\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN\n");

  // strval
  CODE(final_code, "\n# Function STRVAL:\n");
  CODE(final_code, "LABEL $strval\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "DEFVAR LF@nil_check\n");
  CODE(final_code, "TYPE LF@nil_check LF@%param0\n");
  CODE(final_code, "PUSHS string@nil\n");
  CODE(final_code, "PUSHS LF@nil_check\n");
  CODE(final_code, "MOVE LF@%retval0 LF@%param0\n");
  CODE(final_code, "JUMPIFNEQS str_not_nil\n");
  CODE(final_code, "MOVE LF@%retval0 string@\n");
  CODE(final_code, "LABEL str_not_nil\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN\n");

  // strlen
  CODE(final_code, "\n# Function STRLEN:\n");
  CODE(final_code, "LABEL $strlen\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "STRLEN LF@%retval0 LF@%param0\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN\n");
  
  // substring
  CODE(final_code, "\n# Function SUBSTRING:\n");
  CODE(final_code, "LABEL $substring\n");
  CODE(final_code, "PUSHFRAME\n");
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 nil@nil\n");
  CODE(final_code, "DEFVAR LF@str_len\n");
  CODE(final_code, "CREATEFRAME\n");
  CODE(final_code, "DEFVAR TF@%param0\n");
  CODE(final_code, "MOVE TF@%param0 LF@%param0\n");
  CODE(final_code, "CALL $strlen\n");
  CODE(final_code, "MOVE LF@str_len TF@%retval0\n");
  CODE(final_code, "DEFVAR LF@return_cond\n");
  CODE(final_code, "LT LF@return_cond LF@%param1 int@0\n");
  CODE(final_code, "JUMPIFEQ substring_ret LF@return_cond bool@true\n");
  CODE(final_code, "LT LF@return_cond LF@%param2 int@0\n");
  CODE(final_code, "JUMPIFEQ substring_ret LF@return_cond bool@true\n");
  CODE(final_code, "GT LF@return_cond LF@%param1 LF@%param2\n");
  CODE(final_code, "JUMPIFEQ substring_ret LF@return_cond bool@true\n");
  CODE(final_code, "GT LF@return_cond LF@%param1 LF@str_len\n");
  CODE(final_code, "JUMPIFEQ substring_ret LF@return_cond bool@true\n");
  CODE(final_code, "EQ LF@return_cond LF@%param1 LF@str_len\n");
  CODE(final_code, "JUMPIFEQ substring_ret LF@return_cond bool@true\n");
  CODE(final_code, "GT LF@return_cond LF@%param2 LF@str_len\n");
  CODE(final_code, "JUMPIFEQ substring_ret LF@return_cond bool@true\n");

  CODE(final_code, "MOVE LF@%retval0 string@\n");
  CODE(final_code, "DEFVAR LF@char\n");
  CODE(final_code, "LABEL substring_loop\n");
  CODE(final_code, "EQ LF@return_cond LF@%param1 LF@%param2\n");
  CODE(final_code, "JUMPIFEQ substring_ret LF@return_cond bool@true\n");
  CODE(final_code, "GETCHAR LF@char LF@%param0 LF@%param1\n");
  CODE(final_code, "CONCAT LF@%retval0 LF@%retval0 LF@char\n");
  CODE(final_code, "ADD LF@%param1 LF@%param1 int@1\n");
  CODE(final_code, "JUMP substring_loop\n");

  CODE(final_code, "LABEL substring_ret\n");
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN\n");
  
}

void print_main ()
{
  CODE(final_code, "\n # MAIN_START: \n");
  CODE(final_code, "LABEL main\n");
  CODE(final_code, "CREATEFRAME\n");
  CODE(final_code, "PUSHFRAME\n");
}

void print_main_end ()
{
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "CLEARS\n");
  CODE(final_code, "EXIT "); CODE(final_code, "int@0\n");

  CODE(final_code, "\n# MAIN_END\n");
  CODE(final_code, "\0");
}

///////// FUNCTIONS

void print_function_start (char *fun_id)
{
  CODE(final_code, "JUMP $"); CODE(final_code, fun_id); CODE(final_code, "$end"); CODE(final_code, "\n");
  CODE(final_code, "LABEL $"); CODE(final_code, fun_id); CODE(final_code, "\n");
  CODE(final_code, "PUSHFRAME\n");
}

void print_function_end (char *fun_id)
{
  CODE(final_code, "POPFRAME\n");
  CODE(final_code, "RETURN\n");
  CODE(final_code, "LABEL $"); CODE(final_code, fun_id); CODE(final_code, "$end"); CODE(final_code, "\n");
}

void print_function_param_frame ()
{
  CODE(final_code, "CREATEFRAME\n");
}

void print_function_param (char *param_id, int param_index)
{
  char index[MAX_CHAR];
  sprintf(index, "%d", param_index);
  CODE(final_code, "DEFVAR TF@%param"); CODE(final_code, index); CODE(final_code, "\n");
  CODE(final_code, "MOVE TF@%param"); CODE(final_code, index); CODE(final_code, " LF@"); CODE(final_code, param_id); CODE(final_code, "\n"); 
}

void print_function_param_int (int term_val, int param_index)
{
  char index[MAX_CHAR];
  sprintf(index, "%d", param_index);
  char value[MAX_CHAR];
  sprintf(value, "%d", term_val);
  CODE(final_code, "DEFVAR TF@%param"); CODE(final_code, index); CODE(final_code, "\n");
  CODE(final_code, "MOVE TF@%param"); CODE(final_code, index); CODE(final_code, " int@"); CODE(final_code, value); CODE(final_code, "\n"); 
}

void print_function_param_float (float term_val, int param_index)
{
  char index[MAX_CHAR];
  sprintf(index, "%d", param_index);
  char value[MAX_CHAR];
  sprintf(value, "%a", term_val);
  CODE(final_code, "DEFVAR TF@%param"); CODE(final_code, index); CODE(final_code, "\n");
  CODE(final_code, "MOVE TF@%param"); CODE(final_code, index); CODE(final_code, " float@"); CODE(final_code, value); CODE(final_code, "\n"); 
}

void print_function_param_string (char *term_val, int param_index)
{
  char index[MAX_CHAR];
  sprintf(index, "%d", param_index);

  CODE(final_code, "DEFVAR TF@%param"); CODE(final_code, index); CODE(final_code, "\n");
  CODE(final_code, "MOVE TF@%param"); CODE(final_code, index); CODE(final_code, " string@"); 
  
  char tmp;
  char tmp_string[MAX_CHAR];
  for(int i = 0; term_val[i] != '\0'; i++){
    tmp = term_val[i];
    if (!isprint(tmp) || tmp == '#' || tmp <= 32 || tmp == '\\'){
      CODE(final_code, "\\");
      sprintf(tmp_string, "%03d", tmp);
      CODE(final_code, tmp_string);
    }
    else{
      dynamicStrAdd(final_code, tmp);
    }
  }

  CODE(final_code, "\n"); 
}

void print_function_param_nil (int param_index)
{
  char index[MAX_CHAR];
  sprintf(index, "%d", param_index);
  CODE(final_code, "DEFVAR TF@%param"); CODE(final_code, index); CODE(final_code, "\n");
  CODE(final_code, "MOVE TF@%param"); CODE(final_code, index); CODE(final_code, " nil@nil\n");
}

void print_function_retval_exp ()
{
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 GF@%expression\n");
}

void print_function_retval_assign (char *var_id)
{
  CODE(final_code, "MOVE LF@"); CODE(final_code, var_id); CODE(final_code, " TF@%retval0\n");
}

void print_function_retval_int ()
{
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 int@0\n");
}

void print_function_retval_float ()
{
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 float@0.0\n");
}

void print_function_retval_string ()
{
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 string@0\n");
}

void print_function_retval_nil ()
{
  CODE(final_code, "DEFVAR LF@%retval0\n");
  CODE(final_code, "MOVE LF@%retval0 nil@nil\n");
}

void print_function_call (char *fun_id)
{
  CODE(final_code, "CALL $"); CODE(final_code, fun_id); CODE(final_code, "\n");
}

///////////////// STACK OPERATIONS

void print_pushs (char *var_id)
{
   CODE(final_code, "PUSHS LF@");  CODE(final_code, var_id);  CODE(final_code, "\n");
}

void print_pushs_int (int data)
{
  char value[MAX_CHAR];
  sprintf(value, "%d", data);
  CODE(final_code, "PUSHS int@");  CODE(final_code, value);  CODE(final_code, "\n");
}

void print_pushs_float (float data)
{
  char value[MAX_CHAR];
  sprintf(value, "%a", data);
  CODE(final_code, "PUSHS float@");  CODE(final_code, value);  CODE(final_code, "\n");
}

void print_pushs_string (char *data)
{
  CODE(final_code, "PUSHS string@");

  char tmp;
  char tmp_string[MAX_CHAR];
  for(int i = 0; data[i] != '\0'; i++){
    tmp = data[i];
    if (!isprint(tmp) || tmp == '#' || tmp <= 32 || tmp == '\\'){
      CODE(final_code, "\\");
      sprintf(tmp_string, "%03d", tmp);
      CODE(final_code, tmp_string);
    }
    else{
      dynamicStrAdd(final_code, tmp);
    }
  }

  CODE(final_code, "\n");
}

void print_pushs_nil ()
{
  CODE(final_code, "PUSHS nil@nil\n");
}



void print_stack_add ()
{
  CODE(final_code, "ADDS\n");
}

void print_stack_sub ()
{
  CODE(final_code, "SUBS\n");
}

void print_stack_mul ()
{
  CODE(final_code, "MULS\n");
}

void print_stack_div ()
{
  CODE(final_code, "DIVS\n");
}

void print_stack_equals ()
{
  CODE(final_code, "EQS\n");
}

void print_stack_not_equals ()
{
  CODE(final_code, "EQS\n");
  CODE(final_code, "NOTS\n");
}

void print_stack_less_equals ()
{
  CODE(final_code, "POPS GF@%tmp1\n");
  CODE(final_code, "POPS GF@%tmp2\n");
  CODE(final_code, "PUSHS GF@%tmp2\n");
  CODE(final_code, "PUSHS GF@%tmp1\n");
  CODE(final_code, "LTS\n");
  CODE(final_code, "PUSHS GF@%tmp2\n");
  CODE(final_code, "PUSHS GF@%tmp1\n");
  CODE(final_code, "EQS\n");
  CODE(final_code, "ORS\n");
}

void print_stack_less_than ()
{
  CODE(final_code, "LTS\n");
}

void print_stack_greater_equals ()
{
  CODE(final_code, "POPS GF@%tmp1\n");
  CODE(final_code, "POPS GF@%tmp2\n");
  CODE(final_code, "PUSHS GF@%tmp2\n");
  CODE(final_code, "PUSHS GF@%tmp1\n");
  CODE(final_code, "GTS\n");
  CODE(final_code, "PUSHS GF@%tmp2\n");
  CODE(final_code, "PUSHS GF@%tmp1\n");
  CODE(final_code, "EQS\n");
  CODE(final_code, "ORS\n");
}

void print_stack_greater_than ()
{
  CODE(final_code, "GTS\n");
}

void print_stack_op3_to_float ()
{
  CODE(final_code, "INT2FLOATS\n");
}

void print_stack_op3_to_int ()
{
  CODE(final_code, "FLOAT2INTS\n");
}

void print_stack_op1_to_float ()
{
  CODE(final_code, "POPS GF@%tmp1\n");
  CODE(final_code, "INT2FLOATS\n");
  CODE(final_code, "PUSHS GF@%tmp1\n");
}

void print_stack_op1_to_int ()
{
  CODE(final_code, "POPS GF@%tmp1\n");
  CODE(final_code, "FLOAT2INTS\n");
  CODE(final_code, "PUSHS GF@%tmp1\n");
}

void print_stack_concat ()
{
  CODE(final_code, "POPS GF@%tmp3\n");
  CODE(final_code, "POPS GF@%tmp2\n");
  CODE(final_code, "CONCAT GF@%tmp1 GF@%tmp2 GF@%tmp3\n");
  CODE(final_code, "PUSHS GF@%tmp1\n");
}

void print_stack_push_true() 
{
  CODE(final_code, "PUSHS bool@true\n");
}

void print_stack_push_false() 
{
  CODE(final_code, "PUSHS bool@false\n");
}

void print_stack_pop ()
{
  CODE(final_code, "POPS GF@%expression\n"); 
}
///////////////////////////// VARIABLES

void print_var_declare (char *var_id)
{
  CODE(final_code, "DEFVAR LF@"); CODE(final_code, var_id); CODE(final_code, "\n");
}

void print_var_assign_exp (char *var_id)
{
  CODE(final_code, "MOVE LF@"); CODE(final_code, var_id); CODE(final_code, " GF@%expression"); CODE(final_code, "\n");
}

void print_var_assign_int (char *var_id, int data)
{
  char value[MAX_CHAR];
  sprintf(value, "%d", data);
  CODE(final_code, "MOVE LF@"); CODE(final_code, var_id); CODE(final_code, " int@"); CODE(final_code, value); CODE(final_code, "\n");
}

void print_var_assign_float (char *var_id, float data)
{
  char value[MAX_CHAR];
  sprintf(value, "%a", data);
  CODE(final_code, "MOVE LF@"); CODE(final_code, var_id); CODE(final_code, " float@"); CODE(final_code, value); CODE(final_code, "\n");
}

void print_var_assign_string (char *var_id, char *data)
{
  CODE(final_code, "MOVE LF@"); CODE(final_code, var_id); CODE(final_code, " string@");

  char tmp;
  char tmp_string[MAX_CHAR];
  for(int i = 0; data[i] != '\0'; i++){
    tmp = data[i];
    if (!isprint(tmp) || tmp == '#' || tmp <= 32 || tmp == '\\'){
      CODE(final_code, "\\");
      sprintf(tmp_string, "%03d", tmp);
      CODE(final_code, tmp_string);
    }
    else{
      dynamicStrAdd(final_code, tmp);
    }
  }

  CODE(final_code, "\n");
}

void print_var_assing_nil (char *var_id)
{
  CODE(final_code, "MOVE LF@"); CODE(final_code, var_id); CODE(final_code, " nil@nil"); CODE(final_code, "\n");
}

///////////////////////////// IFS n WHILES
void print_if_start (int index)
{
  char if_index[MAX_CHAR];
  sprintf(if_index, "%d", index);
  CODE(final_code, "PUSHS GF@%expression\n");
  CODE(final_code, "PUSHS bool@false\n");
  CODE(final_code, "JUMPIFEQS $if_end"); CODE(final_code, if_index); CODE(final_code, "\n");
}

void print_if_end (int index)
{
  char if_index[MAX_CHAR];
  sprintf(if_index, "%d", index);
  CODE(final_code, "JUMP $else_end"); CODE(final_code, if_index); CODE(final_code, "\n");
  CODE(final_code, "LABEL $if_end"); CODE(final_code, if_index); CODE(final_code, "\n");
}

void print_else_end (int index)
{
  char if_index[MAX_CHAR];
  sprintf(if_index, "%d", index);
  CODE(final_code, "LABEL $else_end"); CODE(final_code, if_index); CODE(final_code, "\n");
}

void print_while_start (int index)
{
  char while_index[MAX_CHAR];
  sprintf(while_index, "%d", index);
  CODE(final_code, "LABEL $while"); CODE(final_code, while_index); CODE(final_code, "\n");
}

void print_while_check (int index)
{
  char while_index[MAX_CHAR];
  sprintf(while_index, "%d", index);
  CODE(final_code, "PUSHS GF@%expression\n");
  CODE(final_code, "PUSHS bool@false\n");
  CODE(final_code, "JUMPIFEQS $while_end"); CODE(final_code, while_index); CODE(final_code, "\n");
}

void print_while_end (int index)
{
  char while_index[MAX_CHAR];
  sprintf(while_index, "%d", index);
  CODE(final_code, "JUMP $while"); CODE(final_code, while_index); CODE(final_code, "\n");
  CODE(final_code, "LABEL $while_end"); CODE(final_code, while_index); CODE(final_code, "\n");
}

