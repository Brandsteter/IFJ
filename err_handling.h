/**
 *Project: Implementácia prekladača imperatívneho jazyka IFJ22 
 *@file err_handling.h
 *@brief defining error types in lexical, syntax and semantics analysis and exiting in case of error occurence
 *@author Daniel Blaško <xblask05>
*/

#ifndef ERR_HANDLING
#define ERR_HANDLING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_generator.h"

#define NO_ERR 0 //no error detected
#define LEX_ERR 1 //lexeme error
#define LEX_ERR_MSG() \
    fprintf(stderr, "Error 1: Lexeme error\n"); \
    dispose_final_code(); \
    exit(LEX_ERR);

#define SYNTAX_ERR 2 //incorrect program syntax/missing header
#define SYNTAX_ERR_MSG() \
    fprintf(stderr, "Error 2: Incorrect program syntax/missing header\n"); \
    dispose_final_code(); \
    exit(SYNTAX_ERR);

#define SEMANTICS_ERR_DEF 3 //undefined/redefined function
#define SEMANTICS_ERR_DEF_MSG() \
    fprintf(stderr, "Error 3: Undefined/redefined function\n"); \
    dispose_final_code(); \
    exit(SEMANTICS_ERR_DEF); 

#define SEMANTICS_ERR_RUN 4 //incorrect function calling or return value
#define SEMANTICS_ERR_RUN_MSG() \
    fprintf(stderr, "Error 4: Incorrect function calling or return value\n"); \
    dispose_final_code(); \
    exit(SEMANTICS_ERR_RUN); 
    
#define SEMANTICS_ERR_VAR 5 //usage of undefined variable
#define SEMANTICS_ERR_VAR_MSG() \
    fprintf(stderr, "Error 5: Usage of undefined variable\n"); \
    dispose_final_code(); \
    exit(SEMANTICS_ERR_VAR);

#define RUN_ERR_RETURN 6 //missing or excess expression in function return command
#define RUN_ERR_RETURN_MSG() \
    fprintf(stderr, "Error 6: Missing or excess expression in function return command\n"); \
    dispose_final_code(); \
    exit(RUN_ERR_RETURN);

#define RUN_ERR_TYPE 7 //incompatible type in expression
#define RUN_ERR_TYPE_MSG() \
    fprintf(stderr, "Error 7: Incompatible type in expression\n"); \
    dispose_final_code(); \
    exit(RUN_ERR_TYPE);

#define SEMANTICS_ERR_OTHER 8 //other errors in semantics analysis
#define SEMANTICS_ERR_OTHER_MSG() \
    fprintf(stderr, "Error 8: Other errors in semantics analysis\n"); \
    dispose_final_code(); \
    exit(SEMANTICS_ERR_OTHER);

#define INTERNAL_ERR 99 //internal compiler error
#define INTERNAL_ERR_MSG() \
    fprintf(stderr, "ERROR 99: Internal compiler error\n"); \
    dispose_final_code(); \
    exit(INTERNAL_ERR);

#endif