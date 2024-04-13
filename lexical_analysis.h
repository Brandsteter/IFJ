/**
 * Project: Implementacia prekladača imperatívneho jazyka IFJ22
 * @file lexical_analysis.h
 * @brief header file for lexical analysis functions
 * @author Lucia Šírová xsirov01
 */

#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS

#include "dynamic_string.h"

//Mozne stavy automatu
typedef enum {
        start,
        functionId0,
        variableId0,
        dataTypeId0,
        functionId,
        variableId,
        dataTypeId,
        number,
        floatNumber,
        floatNumber0,
        leftCurlyPar,
        rightCurlyPar,
        leftPar,
        rightPar,
        addition,
        subtraction,
        period,
        multiplication,
        division,
        divisionOrCommentary,
        greaterThan,
        lessThanOrPrologueStart,
        lessThan,
        greaterEqualThan,
        prologueQMark,
        lessEqualThan,
        notEqual0,
        notEqual1,
        notEqual,
        assignOrEqual,
        assign,
        equal,
        comparisonError,
        semicolon,
        colon,
        comma,
        commentary,
        blockCommentary,
        blockCommentaryBeginning,
        blockCOmmentaryStar,
        blockCommentaryEnding,
        endOfFile,
        endOfLine,
        end,
        stringBeginning,
        stringEnding,
        stringSlash,
        stringSlashOkToAdd,
        stringSlashNewLine,
        stringSlashHorizontalTab,
        error
} automatStates;



//typy tokenov
typedef enum {
    type_keyword,
    type_EOF,
    type_EOL,
    type_empty,

    type_variableID,
    type_functionID,
    type_dataTypeID,
    type_integer,
    type_float,
    type_string,
    type_null,

    type_assign,
    type_equal,
    type_notEqual,
    type_addition,
    type_subtraction,
    type_division,
    type_multiplication,
    type_period,
    
    type_lessThan,
    type_greaterThan,
    type_lessOrEqualThan,
    type_greaterOrEqualThan,


    type_leftCurlyPar,
    type_rightCurlyPar,
    type_leftPar,
    type_rightPar,
    type_commentary,
    type_blockCommentary,
    type_comma,
    type_semicolon,
    type_colon,
    type_prologue,
    type_endPrologue,

    ///Only for precedence analysis
    type_bool
} tokenType;




//atributy tokenu
typedef union{
    int integerNum;
    float floatNum;
    dynamicStr *string;

} tokenAttribute;

//token
typedef struct 
{
    tokenType type;
    tokenAttribute attribute;

} Token;



int getNextToken(Token *token);

automatStates automatStateTransition(automatStates currentState, char edge);

int lexAnalysisExit(int exitMessage);

float makeFloat(char *myStr);

int makeInteger(char *myStr);

bool isKeyword(char *myStr);

bool isPrologue();

#endif