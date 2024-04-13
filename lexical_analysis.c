/**
 * Project: Implementacia prekladača imperatívneho jazyka IFJ22
 * @file lexical_analysis.c
 * @brief Implementation of lexical parser
 * @author Lucia Šírová xsirov01
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "lexical_analysis.h"
#include "err_handling.h"
#include "param_list.h"


dynamicStr tokenData;
dynamicStr *tokenDataAdr = &tokenData;

bool prologueCheck = false;
int counter_of_hex_oct_nums = 0;

char keywords [10][10] = { 
    "else", "float", "function", "if", "int" , "null", "return", "string" , "void", "while"
};



/**
 * Checks if identifier is a keyword.
 *
 * @param myStr identifier which is being processed.
 * @return 1 if identifier is a keyword, 0 if identifier is not a keyword.
 */
bool isKeyword(char *myStr){
    for (int i = 0; i < 10; i ++){
        if (!(strcmp(myStr,keywords[i]))){
            return true;
        }
    }
    return false;
}

/**
 * Checks if identifier is a datatype.
 *
 * @param myStr identifier which is being processed.
 * @return 1 if identifier is a datatype, 0 if identifier is not a datatype.
 */
bool isDataType(char *myStr){
    if (!(strcmp(myStr,"int")) || !(strcmp(myStr,"float")) || !(strcmp(myStr,"string"))){
        return true;
    }
    return false;
}

/**
 * Converts str to integer.
 *
 * @param myStr integer which is being processed.
 * @return processed integer, if fails returns LexAnalysisExit
 */
int makeInteger(char *myStr){
    char *endptr;
    int convertedNumber = strtol(myStr, &endptr, 10);
    if (*endptr){
        return lexAnalysisExit(INTERNAL_ERR);
    }
    return convertedNumber;
}

/**
 * Converts str to float.
 *
 * @param myStr float which is being processed.
 * @return processed float, if fails returns LexAnalysisExit
 */
float makeFloat(char *myStr){
    char *endptr;
    float convertedNumber = strtod(myStr, &endptr);
    if (*endptr){
        return lexAnalysisExit(INTERNAL_ERR);
    }
    return convertedNumber;
    
}


/**
 * Exit message.
 *
 * @param exitMessage exit message.
 * @return exit message
 */
int lexAnalysisExit(int exitMessage){
    return exitMessage;
}


/**
 * Looks for prologue.
 * @return 1 if prologue is found successfully, 0 if prologue not found
 */
bool isPrologue(){
    
    dynamicStr prologueStrAdr;
    dynamicStr *prologueStr = &prologueStrAdr;
    dynamicStrInit(prologueStr);
    int numOfWhiteSpaces = 0;
    char c;

    while(prologueStr -> length < 31 && c != EOF){
        c = getc(stdin);
        if (c == ' ' || c == '\n') {
            numOfWhiteSpaces ++;
            if (numOfWhiteSpaces <= 1){
                if (dynamicStrAdd(prologueStr, c) == 0){
                    return lexAnalysisExit(INTERNAL_ERR);
                }
            }
        }
        else if (dynamicStrAdd(prologueStr, c) == 0){
            return lexAnalysisExit(INTERNAL_ERR);
        }

        if (strcmp(prologueStr -> str, "<?php declare(strict_types=1);") == 0 || strcmp(prologueStr -> str, "<?php\ndeclare(strict_types=1);") == 0){
            dynamicStrDispose(prologueStr);
            return true;
        }
    }
    dynamicStrDispose(prologueStr);
    return false;
}

/**
 * Transition between automat states
 *
 * @param currentState current state of automat
 * @param edge character from stdin
 * @return next state of automat
 */
automatStates automatStateTransition(automatStates currentState, char edge){
    switch (currentState)
    {
        case start:
            {
                if (edge == ' ') return start;
                if (edge == 9) return start;
                if (edge == ';') return semicolon; 
                if (edge == ':') return colon;
                if (edge == ',') return comma;
                if (edge == '.') return period;
                if (edge == ')') return rightPar;  
                if (edge == '(') return leftPar;  
                if (edge == '{') return leftCurlyPar;
                if (edge == '}') return rightCurlyPar;
                if (isdigit(edge)) return number; 
                if (edge == '?') return dataTypeId0; 
                if (edge == '$') return variableId0; 
                if (edge == '!') return notEqual0;
                if (isalpha(edge) || edge == '_') return functionId0; 
                if (edge == '"') return stringBeginning; 
                if (edge == '=') return assignOrEqual; 
                if (edge == '>') return greaterThan; 
                if (edge == '<') return lessThan; 
                if (edge == '/') return divisionOrCommentary; 
                if (edge == '*') return multiplication; 
                if (edge == '-') return subtraction; 
                if (edge == '+') return addition; 
                if (edge == '\n') return endOfLine; 
                if (edge == EOF) return endOfFile; 
                return lexAnalysisExit(LEX_ERR);
            }

        case functionId0:
            if (isalnum(edge) || edge == '_') return functionId; 
            return error;

        case variableId0:
            if (isalpha(edge) || isdigit(edge) || edge == '_') return variableId; 
            return error;

        case dataTypeId0:
            if (isalpha(edge)) return dataTypeId;
            else if (edge == '>') return end;
            return error;

        case variableId:
            if (isalpha(edge) || isdigit(edge) || edge == '_') return variableId; 
            return error;

        case functionId:
            if (isalnum(edge) || edge == '_') return functionId; 
            else return error;

        case dataTypeId:
            if (isalpha(edge)) return dataTypeId;
            else return error;

        case greaterThan:
            if (edge == '=') return greaterEqualThan; 
            return error;

        case lessThan:
            if (edge == '=') return lessEqualThan; 
            return error;     

        case number:
            if (isdigit(edge)) return number;
            else if ((edge == '.') || (edge == 'e') || (edge == 'E')) return floatNumber0;
            return error;

        case floatNumber0:
            if (isdigit(edge) || (edge == '+') || (edge == '-')) return floatNumber;
            else return lexAnalysisExit(LEX_ERR);
            return error;

        case divisionOrCommentary:
            if (edge == '/') return commentary;
            else if (edge == '*') return blockCommentaryBeginning;
            else return division;
            
        case commentary:
            if ((edge != '\n') && (edge != EOF)) return commentary;
            return error;

        case blockCommentaryBeginning:
            if (edge == '*') return blockCOmmentaryStar;
            else return blockCommentaryBeginning;
            return error;

        case blockCOmmentaryStar:
            if (edge == '/') return blockCommentaryEnding;
            else return blockCommentaryBeginning;
            return error;

        case assignOrEqual:
            if (edge == '=') return comparisonError;
            else return assign;
            return error;

        case comparisonError:
            if (edge == '=') return equal;
            return error;      
  
        case notEqual0:
            if (edge == '=') return notEqual1;
            else return error;
        
        case notEqual1:
            if (edge == '=') return notEqual;
            else return error;

        case stringBeginning:
            if (edge == '"') return stringEnding;
            else if (edge == 92) return stringSlash;
            else if (edge >= 32) return stringBeginning;
            return error;     
        case stringSlash:
            if (edge == 92 || edge == '"' || edge == 'n' || edge == 't') return stringSlashOkToAdd;
            else if (edge >= 32) return stringBeginning;
            return error; 
        case stringSlashOkToAdd:
            if (edge == 92) return stringSlash;
            else if (edge =='"') return stringEnding;
            else if (edge >= 32) return stringBeginning;
            return error;
    
        case error:
            return lexAnalysisExit(LEX_ERR);
        default:
            return lexAnalysisExit(LEX_ERR);
    }
}


/**
 * Looks for new token from stdin.
 *
 * @param token pointer to new token
 * @return NO_ERR if token created sucessuflly, 1 if INTERNAL_ERR, 2 if LEX_ERR
 */
int getNextToken(Token *token){

    //init state
    automatStates currentState = start;

    //init token type
    token -> type = type_empty;


    //init token data
    if (dynamicStrInit(tokenDataAdr) == false){
        return lexAnalysisExit(INTERNAL_ERR);
    }

    
    if (prologueCheck == 0){
       if (isPrologue() == 0){
            prologueCheck = true;
            return lexAnalysisExit(LEX_ERR);
       }
       else{
            token -> type = type_prologue;
            prologueCheck = true;
            return lexAnalysisExit(NO_ERR);
       }
    }


    //pozerame kazdy znak a zistujeme tokeny
    while (true){
        char c = getc(stdin);
        automatStates nextState = automatStateTransition(currentState, c);
        
        //vyhodnotenie tokenov s jednym znakom 
        if (nextState == semicolon){
            token -> type = type_semicolon;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == addition){
            token -> type = type_addition;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == subtraction){
            token -> type = type_subtraction;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == multiplication){
            token -> type = type_multiplication;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == leftPar){
            token -> type = type_leftPar;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == rightPar){
            token -> type = type_rightPar;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == endOfLine){
            nextState = start;
        }
        else if (nextState == equal){
            token -> type = type_equal;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == notEqual){
            token -> type = type_notEqual;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == leftCurlyPar){
            token -> type = type_leftCurlyPar;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == rightCurlyPar){
            token -> type = type_rightCurlyPar;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == colon){
            token -> type = type_colon;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == comma){
            token -> type = type_comma;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == end){
            token -> type = type_endPrologue;
            return lexAnalysisExit(NO_ERR);
        }
        else if (nextState == period){
            token -> type = type_period;
            return lexAnalysisExit(NO_ERR);

        }

        //vyhodnotenie tokenov s viac znakmi  
        //= alebo ===
        else if (nextState == assignOrEqual){
            c = getc(stdin);
            if (automatStateTransition(nextState,c) == assign){
                token -> type = type_assign;
                ungetc(c,stdin);
                return lexAnalysisExit(NO_ERR);
            }
            ungetc(c,stdin);
        }

        // delenie alebo komentar
        else if (nextState == divisionOrCommentary){
            c = getc(stdin);
            if (automatStateTransition(nextState,c) != commentary && automatStateTransition(nextState,c) != blockCommentaryBeginning){
                token -> type = type_division;
                ungetc(c,stdin);
                return lexAnalysisExit(NO_ERR);
            }
            ungetc(c,stdin);
        }

        // komentar
        else if (nextState == commentary){
            c = getc(stdin);
            if (automatStateTransition(nextState,c) != commentary){
                nextState = start;
            }
            ungetc(c,stdin);
        }       

        //blokovy komentar
        else if (nextState == blockCommentaryEnding){
            nextState = start;
        }

        // > alebo >=
        else if (nextState == greaterThan){
            c = getc(stdin);
            if (automatStateTransition(nextState,c) == greaterEqualThan){
                token -> type = type_greaterOrEqualThan;
            }
            else {
                ungetc(c, stdin);
                token -> type = type_greaterThan;
            }
            return lexAnalysisExit(NO_ERR);
        }

        // < alebo <=
        else if (nextState == lessThan){
            c = getc(stdin);
            if (automatStateTransition(nextState,c) == lessEqualThan){
                token -> type = type_lessOrEqualThan;
            }
            else {
                ungetc(c, stdin);
                token -> type = type_lessThan;
            }
            return lexAnalysisExit(NO_ERR);
        }

        // premenna 
        else if (nextState == variableId){
            if (dynamicStrAdd(tokenDataAdr, c) == false){
                return lexAnalysisExit(INTERNAL_ERR);
            }

            c = getc(stdin);
            if (automatStateTransition(nextState,c) != variableId) {

                token -> attribute.string = tokenDataAdr;
                token -> type = type_variableID;
                ungetc(c,stdin);
                return lexAnalysisExit(NO_ERR); 

            }
            ungetc(c, stdin);
        }
        
        // funkcia 
        else if (nextState == functionId || nextState == functionId0){
            if (dynamicStrAdd(tokenDataAdr, c) == false){
                return lexAnalysisExit(INTERNAL_ERR);
            }

            c = getc(stdin);
            if (automatStateTransition(nextState,c) != functionId) {
                token -> attribute.string = tokenDataAdr;
                
                //v pripade ze je dataType uvedeny bez "?" 
                if (isDataType(token -> attribute.string ->str)){
                    token -> type = type_dataTypeID;
                    ungetc(c,stdin);
                    return lexAnalysisExit(NO_ERR);
                }

                if (isKeyword(token -> attribute.string -> str)){
                    //token je keyword
                    token -> type = type_keyword;
                    ungetc(c,stdin);
                    return lexAnalysisExit(NO_ERR);
                }
                
                token -> type = type_functionID;
                ungetc(c,stdin);
                return lexAnalysisExit(NO_ERR);
            }

            ungetc(c,stdin);
        }
        //data type
        else if (nextState == dataTypeId){
            c = getc(stdin);

            if (automatStateTransition(nextState,c) != dataTypeId) {

                token -> attribute.string = tokenDataAdr;

                if (isDataType(token -> attribute.string ->str)){
                    token -> type = type_dataTypeID;
                    ungetc(c,stdin);
                    return lexAnalysisExit(NO_ERR);
                }
                else{
                    return lexAnalysisExit(LEX_ERR);
                }
            }
            ungetc(c, stdin);
        }

        //cele cislo 
        else if (nextState == number){
            if (dynamicStrAdd(tokenDataAdr, c) == false){
                return lexAnalysisExit(INTERNAL_ERR);
            }

            c = getc(stdin);
            if (automatStateTransition(nextState,c) != number && automatStateTransition(nextState,c) != floatNumber) {
                token -> attribute.integerNum = makeInteger(tokenDataAdr -> str);
                token -> type = type_integer;
                ungetc(c,stdin);
                return lexAnalysisExit(NO_ERR);
            }

            ungetc(c,stdin);
        }

        // float cislo
        else if (nextState == floatNumber0){
            if (dynamicStrAdd(tokenDataAdr, c) == false){
                return lexAnalysisExit(INTERNAL_ERR);
            }
        }
        else if (nextState == floatNumber){
            if (dynamicStrAdd(tokenDataAdr, c) == false){
                return lexAnalysisExit(INTERNAL_ERR);
            }
            c = getc(stdin);
            if (automatStateTransition(nextState,c) != floatNumber){
                token -> attribute.floatNum = makeFloat(tokenDataAdr -> str);
                token -> type = type_float;
                ungetc(c,stdin);
                return lexAnalysisExit(NO_ERR);
            }

            ungetc(c,stdin);
        }

        //string
        else if (nextState == stringBeginning){
            if (c != '"'){
                if (dynamicStrAdd(tokenDataAdr, c) == false){
                    return lexAnalysisExit(INTERNAL_ERR);
                }
            }
            c = getc(stdin);
            if (automatStateTransition(nextState,c) == stringEnding){
                token -> attribute.string = tokenDataAdr;
                token -> type = type_string;
                return lexAnalysisExit(NO_ERR);
            }
            ungetc(c,stdin);
        }
        else if (nextState == stringSlash){
            char tmp = c;
            c = getc(stdin);
            if (automatStateTransition(nextState,c) == stringBeginning){
                if (dynamicStrAdd(tokenDataAdr, tmp) == false){
                    return lexAnalysisExit(INTERNAL_ERR);
                }
            }
            ungetc(c, stdin);
        }
        else if (nextState == stringSlashOkToAdd){
            if (c == 'n'){
                c = 10;
                if (dynamicStrAdd(tokenDataAdr, c) == false){
                    return lexAnalysisExit(INTERNAL_ERR);
                }
            }
            else if (c == 't'){
                c = 9;
                if (dynamicStrAdd(tokenDataAdr, 9) == false){
                    return lexAnalysisExit(INTERNAL_ERR);
                }
            }
            else {
                if (dynamicStrAdd(tokenDataAdr, c) == false){
                return lexAnalysisExit(INTERNAL_ERR);
                }
            }
            
            c = getc(stdin);
            if (automatStateTransition(nextState,c) == stringEnding){
                token -> attribute.string = tokenDataAdr;
                token -> type = type_string;
                return lexAnalysisExit(NO_ERR);
            }
            ungetc(c, stdin);
        }

        //koniec suboru
        else if (c == EOF){
            token -> type = type_EOF;
            return lexAnalysisExit(NO_ERR);
        }

        //pokracovanie na nasledujuci stav
        currentState = nextState;
    }
}
