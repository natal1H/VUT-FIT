//--------------INCLUDE--------------
#include "expression_parser.h"
#include "scanner.h"
#include "stringlib.h"
# include <stdio.h>
#include <limits.h>
#include <asm/errno.h>
#include <errno.h>

/*
 * Vyuziva enum token_type zo skenera
 *
 *  Zakladna tabulka:
 *          |            |            |              Nasledujuci token(nad nimi su ID z enum token_type)            |
 *          | Podmienka/ |Moze skoncit|  7     9     8     10    17   18     2     3     4     11   12    13     14    15    16 |
 *          |  Pocitadlo |   vyraz?   |  +     -     *     /     (     )    INT  FLOAT STRING  <    <=     >     >=    ==    != | token   ID
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  +      7
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  -      9
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  *      8
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  /      10
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  (      17
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  )      18
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------------
 *          |            |            |  1  |  1  |  1  |  1  |  0  |  1  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  INT    2
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------  ID
 *          |            |            |  1  |  1  |  1  |  1  |  1  |  1  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  FLOAT  3
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------------
 *          |            |            |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  ?  |  ?  |  ?  |  ?  |  1  |  1  |  STRING 4
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?P |  1 |  0  |  0  |   0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  <      11
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  <=     12
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  >      13
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  >=     14
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  ==     15
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  !=     16
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *
 *  Legenda:
 *  1  =>  Pravda
 *  1P =>  Pravda s podmienkou
 *  0  =>  Nepravda
 */

#define SUCCESSFUL_START 24
#define SUCCESSFUL_END 23
#define COUNTER_OF_TOKEN 22


long long syntax_table[22][25]= {
        //    0      1    2      3      4      5      6      7     8     9    10    11    12    13    14    15    16    17    18    19    20      21   22  23  24|
        // NO_TYPE  ID   INT  FLOAT  STRING  KWORD  ASSIGN  +     *     -     /     <    =<     >    >=    ==    !=     (     )   COMMA  EOL     EOF   P   S   Z | Nazov vstupneho tokenu   ID_cislo
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //NO_TYPE,             //0
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    1  , 0 , 1,  1 }, //IDENTIFIER,          //1
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1,  1 }, //INTEGER,             //2
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1,  1 }, //FLOAT,               //3
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1,  1 }, //STRING,              //4
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  1  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  1 }, //KEYWORD,             //5
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //ASSIGN,              //6
        {    0   ,  1  ,  1  ,  1   ,   1  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //ADDITION,            //7
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //MULTIPLICATION,      //8
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //SUBTRACTION,         //9
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //DIVISION,            //10
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //LESS,                //11
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //LESS_OR_EQUALS,      //12
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //GREATER,             //13
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //GREATER_OR_EQUALS,   //14
        {    0   ,  1  ,  1  ,  1   ,   1  ,  1   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //EQUALS,              //15
        {    0   ,  1  ,  1  ,  1   ,   1  ,  1   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //NOT_EQUALS,          //16
        {    0   ,  1  ,  1  ,  1   ,   0  ,  1   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  1 }, //LEFT_ROUND_BRACKET,  //17
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1,  0 }, //RIGHT_ROUND_BRACKET, //18
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0 ,   0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0,  0 }, //COMMA,               //19
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    1  , 0 , 1,  0 }, //EOL,                 //20
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 1,  0 }, //TYPE_EOF,            //21
                                                                                                                                                   //  ^   ^   ^
                                                                                                                                                   //  |   |   +Moze zacinat vyraz?
                                                                                                                                                   //  |   +Moze skoncit vyraz?
                                                                                                                                                   //  |
                                                                                                                                                   //  + Pocet prijatych(napr pocitanie zatvoriek)
};

const char precedenceTable[table_size][table_size]= {
        //    0      1    2     3     4     5     6     7     8    9     10    11     12   13    14    15    16    17    18    19    20      21       22    23    24
        //  unary-  not   *     /    div   mod   and    +     -    or    xor    =     <>    <    <=     >    >=    in     (    )     ID   function  array   ,      $
        { '<'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '#' , '>' }, //unary  0
        { '<'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // not   1
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // *     2
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // /     3
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // div   4
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // mod   5
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // and   6
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // +     7
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // -     8
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // or    9
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // xor   10
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // =     11
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <>    12
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <     13
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <=    14
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // >     15
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // >=    16
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // in    17
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '=' , '<' ,   '<'   ,  '<' , '=' , '#' }, // (     18
        { '#'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '#' , '>' , '#' ,   '#'   ,  '#' , '>' , '>' }, // )     19
        { '#'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '#' , '>' , '#' ,   '#'   ,  '#' , '>' , '>' }, // ID    20
        { '#'  , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '=' , '#' , '#' ,   '#'   ,  '#' , '#' , '#' }, // func  21
        { '#'  , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '=' , '#' , '#' ,   '#'   ,  '#' , '#' , '#' }, // array 22
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '=' , '<' ,   '<'   ,  '<' , '=' , '#' }, // ,     23
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '#' , '<' ,   '<'   ,  '<' , '#' , '#' }, // $     24
};

int TransformTable[22] = {
        no_operation_pTable,       //0
        id_pTable,                 //1
        id_pTable,                 //2
        id_pTable,                 //3
        id_pTable,                 //4
        no_operation_pTable,       //5
        no_operation_pTable,       //6
        addition_pTable,           //7
        multiplication_pTable,     //8
        substraction_pTable,       //9
        division_pTable,           //10
        less_pTable,               //11
        less_or_equals_pTable,     //12
        greater_pTable,            //13
        grater_or_equals_pTable,   //14
        equals_pTable,             //15
        not_equals_pTable,         //16
        left_round_bracket_pTable, //17
        right_round_bracket_pTable,//18
        comma_pTable,              //19
        dollar_pTable,             //20
        no_operation_pTable        //21
};


/* -------------------------------------------------------------------------- */

/**
 * Funkcie na pracu s tokenovym zasobnikom 
 */

void InitStackTOKEN (tStackP *S)
/*   ------
** Inicializace zásobníku.
**/
{
    S->top = 0;
}

void PushStackTOKEN (tStackP *S, struct token ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top==MAXSTACK)
        fprintf(stderr, "Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
    else {
        S->top++;
        S->stack[S->top]=ptr;
    }
}

struct token PopStackTOKEN (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        fprintf(stderr, "Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
//        return(-1);
    }
    else {
        return (S->stack[S->top--]);
    }


    Token *t = token_initialize();
    return *t;
}

bool IsStackEmptyTOKEN (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
    return(S->top==0);
}

/* -------------------------------------------------------------------------- */

/**
 * Funkcie na pracu s integerovym zasobnikom
 */

void InitStackINT (tStackINT *S)
/*   ------
** Inicializace zásobníku.
**/
{
    S->top = 0;
}

/* -------------------------------------------------------------------------- */

/**
 * Funkcie pre pracu s charovym zasobnikom 
 */

void InitStackSTRING (tStackCHAR *S)
/*   ------
** Inicializace zásobníku.
**/
{
    S->top = 0;
}

void PushStackSTRING (tStackCHAR *S, char* ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top==MAXSTACK)
        fprintf(stderr, "Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
    else {
        S->top++;
        S->stack[S->top]=ptr;
    }
}

char* PopStackSTRING (tStackCHAR *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        fprintf(stderr, "Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
//        return(-1);
    }
    else {
        return (S->stack[S->top--]);
    }

    return NULL;
}

/* -------------------------------------------------------------------------- */


/**
 * @param Token Vystup z scannera
 * @param L Buffer/Obojsmerny zoznam
 * @note Ulozi cely kontrolovany vyraz do bufferu
 */
void LoadToBuffer(Token *Token, tDLList *ExprList) {

    tDLElemPtr Expr = malloc(sizeof(struct tDLElem)); //Alokovanie prvku
    if (Expr == NULL) { //Ak sa nepodarila alokacia
        fprintf(stderr, "Memory is full");
        return;
    } else {

        Expr->next = NULL;
        Expr->prev = NULL;
        Expr->Token.attribute = NULL;
        Expr->Token.type = 0;

        if (ExprList->First == NULL) {
            (*Expr).Token = *Token; //inicializacia noveho prvku
            ExprList->First = Expr;        //prvy v zozname ak je prvy prvok
            ExprList->Last = Expr;         //zaroven ako posledny
            ExprList->Act = Expr;
        } else {
            (*Expr).Token = *Token;         //inicializacia noveho prvku
            ExprList->Last->next = Expr;           //pridanie do zoznamu na koniec
            ExprList->Last->next->prev = ExprList->Last;  //obojsmerne previazanie
            ExprList->Last = Expr;                 //nastavenie posledneho prvku
        }
    }
}



/**
 * Vykona operaciu pri vypocitavani postfixovej notacie
 * @param token_ID1 Prvy operand
 * @param token_OP Operacia
 * @param token_ID2 Druhy operand
 * @return Vrati vysledok danej operacie
 */
char* EvaluateNow(char* token_ID1, Token token_OP, char* token_ID2,  int *ErrorStatus){

    bool was_param = false;

    if (*ErrorStatus == ERR_SEM_UNDEF || *ErrorStatus == ERR_SEM_TYPE) {
        return "ERROR";
    }

    // Sémantická kontrola
    tDataType token1 = T_UNDEFINED; tDataType token2 = T_UNDEFINED;
    // Získanie typov tokenov
    if (get_type_from_token(actual_function_ptr, token_ID1, &token1) != ERR_OK) {
        // Sémantická chyba - riešiť
        fprintf(stderr, "Chyba! Nedefinovaná premenná %s.\n", token_ID1);
        *ErrorStatus = ERR_SEM_UNDEF;
        return "ERROR";
    }
    if (get_type_from_token(actual_function_ptr, token_ID2, &token2) != ERR_OK) {
        // Sémantická chyba - riešiť
        fprintf(stderr, "Chyba! Nedefinovaná premenná %s.\n", token_ID2);
        *ErrorStatus = ERR_SEM_UNDEF;
        return "ERROR";
    }

    if (token1 == T_PARAM || token2 == T_PARAM) {
        automatic_conversion_generate(token_ID1, token_ID2, &token_ID1, &token_ID2);
        was_param = true;
    }

    char *var_name = expr_parser_create_unique_name(*actual_function_ptr); // Získam meno premennej, do ktorej sa bude ukladať "výsledok" operácie
    variable_set_defined(actual_function_ptr, var_name);

    finalVar = var_name;

    switch(token_OP.type){

        case (ADDITION) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = arithmetic_check_compatibility(token1, token2);

            if (compatibility_err_code == ERR_OK) {
                // Sémantická akcia: zistenie výsledného typu
                typeFinal = aritmetic_get_final_type(token1, token2);

                variable_set_type(*actual_function_ptr, var_name, typeFinal);
                // Vygenerovanie novej premennej v kóde
                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
                }

                // Generovanie kódu
                if (token1 == T_STRING && token2 == T_STRING) {
                    // Generovanie CONCAT

                    gen_concat(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));

                    return var_name;
                }

                if (!was_param) {
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        } else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                // Generovanie samotnej inštrukcie ADD
                gen_add(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Koniec generovania kódu
            }
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (MULTIPLICATION) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = arithmetic_check_compatibility(token1, token2);

            if (compatibility_err_code == ERR_OK) {
                // Sémantická akcia: zistenie výsledného typu
                typeFinal = aritmetic_get_final_type(token1, token2);

                variable_set_type(*actual_function_ptr, var_name, typeFinal);
                // Vygenerovanie novej premennej v kóde
                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
                }

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        } else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                // Generovanie samotnej inštrukcie MUL
                gen_mul(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Koniec generovania kódu
            }
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (SUBTRACTION) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = arithmetic_check_compatibility(token1, token2);

            if (compatibility_err_code == ERR_OK) {
                // Sémantická akcia: zistenie výsledného typu
                typeFinal = aritmetic_get_final_type(token1, token2);

                variable_set_type(*actual_function_ptr, var_name, typeFinal);
                // Vygenerovanie novej premennej v kóde
                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
                }

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        }
                        else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                // Generovanie samotnej inštrukcie SUB
                gen_sub(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Koniec generovania kódu
            }
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (DIVISION) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = arithmetic_check_compatibility(token1, token2);

            if (compatibility_err_code == ERR_OK) {
                // Sémantická akcia: zistenie výsledného typu
                typeFinal = aritmetic_get_final_type(token1, token2);

                variable_set_type(*actual_function_ptr, var_name, typeFinal);
                // Vygenerovanie novej premennej v kóde
                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
                }

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        }
                        else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                // Generovanie samotnej inštrukcie DIV/IDIV
                if (was_param) {
                    generate_dynamic_division(token_ID1, token1, token_ID2, token2, var_name);
                }
                else {
                    if (typeFinal == T_INT) // Celočíselné delenie (IDIV)
                        gen_idiv(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2,
                                 token2, is_variable(*actual_function_ptr, token_ID2));
                    else // Normálne delenie (DIV)
                        gen_div(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2,
                                token2, is_variable(*actual_function_ptr, token_ID2));
                }
                // Koniec generovania kódu
            }
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (LESS) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = comparison_check_compatibility(token1, token2);

            variable_set_type(*actual_function_ptr, var_name, typeFinal);
            if (compatibility_err_code == ERR_OK) {
                typeFinal = T_BOOLEAN;

                variable_set_type(*actual_function_ptr, var_name, typeFinal);
                // Vygenerovanie novej premennej v kóde
                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
                }

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        }
                        else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                gen_lt(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Koniec generovania kódu
            }
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (LESS_OR_EQUALS) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = comparison_check_compatibility(token1, token2);

            variable_set_type(*actual_function_ptr, var_name, typeFinal);
            if (compatibility_err_code == ERR_OK) {
                typeFinal = T_BOOLEAN;

                variable_set_type(*actual_function_ptr, var_name, typeFinal);
                // Vygenerovanie novej premennej v kóde
                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
                }

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        } else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                // Vygenerovanie LT
                char *var_name_lt = expr_parser_create_unique_name(*actual_function_ptr); // Získam meno premennej, do ktorej sa bude ukladať "výsledok" LT
                variable_set_defined(actual_function_ptr, var_name_lt);
                //gen_defvar(var_name_lt);

                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name_lt, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name_lt, &instr_list); // DEFVAR LF@%param_id
                }

                gen_lt(var_name_lt, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Vygenerovanie EQ
                char *var_name_eq = expr_parser_create_unique_name(*actual_function_ptr); // Získam meno premennej, do ktorej sa bude ukladať "výsledok" EQ
                variable_set_defined(actual_function_ptr, var_name_eq);
                //gen_defvar(var_name_eq);

                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name_eq, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name_eq, &instr_list); // DEFVAR LF@%param_id
                }

                gen_eq(var_name_eq, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Vygenerovanie OR medzi porovnaní
                gen_or(var_name, var_name_lt, var_name_eq);
                // Koniec generovania kódu
            }
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (GREATER) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = comparison_check_compatibility(token1, token2);

            if (compatibility_err_code == ERR_OK) {
                typeFinal = T_BOOLEAN;

                variable_set_type(*actual_function_ptr, var_name, typeFinal);
                // Vygenerovanie novej premennej v kóde
                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
                }

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        } else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                gen_gt(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
            }
                // Koniec generovania kódu
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (GREATER_OR_EQUALS) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = comparison_check_compatibility(token1, token2);

            variable_set_type(*actual_function_ptr, var_name, typeFinal);
            // Vygenerovanie novej premennej v kóde
            if (is_in_while > 0) {
                // Je vo while, pridávať na zvlášť zoznam
                gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
            }
            else {
                gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
            }

            if (compatibility_err_code == ERR_OK) {
                typeFinal = T_BOOLEAN;

                variable_set_type(*actual_function_ptr, var_name, typeFinal);

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        } else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                // Vygenerovanie GT
                char *var_name_gt = expr_parser_create_unique_name(*actual_function_ptr); // Získam meno premennej, do ktorej sa bude ukladať "výsledok" LT
                variable_set_defined(actual_function_ptr, var_name_gt);
                //gen_defvar(var_name_gt);

                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name_gt, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name_gt, &instr_list); // DEFVAR LF@%param_id
                }

                gen_gt(var_name_gt, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Vygenerovanie EQ
                char *var_name_eq = expr_parser_create_unique_name(*actual_function_ptr); // Získam meno premennej, do ktorej sa bude ukladať "výsledok" EQ
                variable_set_defined(actual_function_ptr, var_name_eq);
                //gen_defvar(var_name_eq);

                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name_eq, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name_eq, &instr_list); // DEFVAR LF@%param_id
                }

                gen_eq(var_name_eq, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Vygenerovanie OR medzi porovnaní
                gen_or(var_name, var_name_gt, var_name_eq);
                // Koniec generovania kódu
            }
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (EQUALS) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = comparison_check_compatibility(token1, token2);

            variable_set_type(*actual_function_ptr, var_name, typeFinal);
            // Vygenerovanie novej premennej v kóde
            if (is_in_while > 0) {
                // Je vo while, pridávať na zvlášť zoznam
                gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
            }
            else {
                gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
            }

            if (compatibility_err_code == ERR_OK) {
                typeFinal = T_BOOLEAN;

                variable_set_type(*actual_function_ptr, var_name, typeFinal);

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        } else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                gen_eq(var_name, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
            }
                // Koniec generovania kódu
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        case (NOT_EQUALS) : {

            // Sémantická kontrola: skontrolovať kompatibilitu typov
            int compatibility_err_code = comparison_check_compatibility(token1, token2);

            variable_set_type(*actual_function_ptr, var_name, typeFinal);
            // Vygenerovanie novej premennej v kóde
            if (is_in_while > 0) {
                // Je vo while, pridávať na zvlášť zoznam
                gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
            }
            else {
                gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
            }

            if (compatibility_err_code == ERR_OK) {
                typeFinal = T_BOOLEAN;

                variable_set_type(*actual_function_ptr, var_name, typeFinal);

                if (!was_param) {
                    // Generovanie kódu
                    // Zistiť či treba konverziu INT2FLOAT
                    if ((token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT)) {
                        // Treba konverziu jednej z hodnôt
                        char *converted_name;
                        if (token1 == T_INT) {
                            convert_int_2_float(actual_function_ptr, token_ID1, &converted_name, is_in_while);

                            // Nahradiť token_ID1 za converted_name a token1 za T_FLOAT
                            token1 = T_FLOAT;
                            token_ID1 = converted_name;
                        } else {
                            convert_int_2_float(actual_function_ptr, token_ID2, &converted_name, is_in_while);

                            // Nahradiť token_ID2 za converted_name a token2 za T_FLOAT
                            token2 = T_FLOAT;
                            token_ID2 = converted_name;
                        }
                    }
                }
                // Vygenerovanie EQ
                char *var_name_eq = expr_parser_create_unique_name(*actual_function_ptr); // Získam meno premennej, do ktorej sa bude ukladať "výsledok" LT
                variable_set_defined(actual_function_ptr, var_name_eq);
                //gen_defvar(var_name_eq);

                if (is_in_while > 0) {
                    // Je vo while, pridávať na zvlášť zoznam
                    gen_defvar(var_name_eq, &while_declaration_list); // DEFVAR LF@%param_id
                }
                else {
                    gen_defvar(var_name_eq, &instr_list); // DEFVAR LF@%param_id
                }

                gen_eq(var_name_eq, token_ID1, token1, is_variable(*actual_function_ptr, token_ID1), token_ID2, token2, is_variable(*actual_function_ptr, token_ID2));
                // Vygenerovanie NOT
                gen_not(var_name, var_name_eq);
                // Koniec generovania kódu
            }
            else {
                // Sémantická chyba - ERR_SEM_TYPE
                fprintf(stderr, "Chyba! Nekompatibilné typy operandov %s a %s.\n", token_ID1, token_ID2);
                // Vrátiť compatibility_err_code
                *ErrorStatus = compatibility_err_code;
                return "ERROR";
            }

            return var_name;
        }

        default:{}
    }

    return NULL;
}

/**
 * Vypocita vyraz v postfixovej notacii
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture
 * @param stackPostfix Zasobnik s ulozenou postfixou notaciou
 * @param stackTemp Pomocny zasobnik pre pretocenie poradia v postfixovom zasobniku
 * @param stackOutputINT Vystupny zasobnik v ktorom je pocitana postfixova notacia
 * @param stackOutputCHAR Vystupny zasobnik v ktorom je pocitana postfixova notacia v type char!
 * @param last_operation Posledna operacia
 * @return Vracia vysledok v datovom type long
 */
char* EvaluateFromPostfix(tDLList *ExprList, tStackP *stackPostfix, tStackP *stackCHAR, tStackINT *stackOutputINT , tStackCHAR *stackOutputCHAR, int last_operation, int *ErrorStatus) {

    //Obratenie zasobnika, najskor POPnut potrebujeme operandy ktore su na spodku zasobnika
    while(stackPostfix->top > 0){
        PushStackTOKEN(stackCHAR, PopStackTOKEN(stackPostfix));
    }
    *stackPostfix = *stackCHAR; //Prekopirovanie pretoceneho zasobnika na stary zasobnik

    while(stackPostfix->top > 0){
        Token tmp;                    //Pomocny token
        tmp = PopStackTOKEN(stackPostfix); //Zisti hodnotu na vrchole zasobniku

            if(TransformTable[tmp.type] == id_pTable) { //Ak ide o ID
//                char *ptr;  //Buffer k pretypovaniu
//                PushStackINT(stackOutputINT, strtol(tmp.attribute, &ptr, 10));

                //Pushni char do charoveho stacku
                PushStackSTRING(stackOutputCHAR, tmp.attribute);

            } else {
                //Pushni na vysledny charovy stack vysledok
                char *retChar = EvaluateNow(PopStackSTRING(stackOutputCHAR), tmp, PopStackSTRING(stackOutputCHAR), ErrorStatus);
                if (*ErrorStatus != ERR_SEM_TYPE || *ErrorStatus != ERR_SEM_UNDEF) {
                    PushStackSTRING(stackOutputCHAR, retChar);
                }
                else {
                    // Teraz by malo skončiť s vyčslovaním
                }
            }
    }
    //Vrat vysledok zo zasobnika
    return PopStackSTRING(stackOutputCHAR);
}

/**
 * Spracuje infixovy vyraz do postfixovej notacie pomocou precedencnej tabulky
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture
 * @param stack Pomocny zasobnik kde sa vyhodnocuje priorita operatorov
 * @param stackOutput Vystupny zasobnik s postfixovou notaciou
 * @param last_operation Posledna vykonana operacia
 * @return Vracia zasobnik s postfixou notaciou
 */
tStackP ParseToPostfix(tDLList *ExprList, tStackP *stack, tStackP *stackOutput, int last_operation) {

    //bool UvolnovanieZasobnika = false;
    Token tmp;              //Pomocny token
    tmp.type = EOL;         //Koniec zasobnika ako EOL
    PushStackTOKEN(stack, tmp);  //Informuje o konci zasobnika

//    ExprList->Act = ExprList->First;

    while (ExprList->Act != NULL && ExprList->Act->Token.type != EOL) { //Pokym nie je koniec vyrazu(zoznamu)

                tmp = PopStackTOKEN(stack); //Zisti hodnotu na vrchole zasobnika
                PushStackTOKEN(stack, tmp); //Ale hodnotu ponechaj ulozenu na zasobniku

                if(TransformTable[ExprList->Act->Token.type] == id_pTable){ //Ak je to operand
                    PushStackTOKEN(stackOutput, ExprList->Act->Token);           //Potom push do vysledneho zasobnika
                } else {

                    //Pokym je vacsia priorita
                    while (precedenceTable[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '>') {

                        //Ak sa token na zasobniku nerovna '(' alebo ')'
                        //Zatvorky nepushujeme do vysledneho vyrazu!
                        if(TransformTable[tmp.type] != left_round_bracket_pTable && TransformTable[tmp.type] != right_round_bracket_pTable) {
                            PushStackTOKEN(stackOutput, PopStackTOKEN(stack)); //Prehod token do vysledneho zasobnika
                        } else {
                            PopStackTOKEN(stack);    //Len odober token zo zasobnika, len zatvorky!
                        }

                        tmp = PopStackTOKEN(stack); //Zisti hodnotu na vrchole zasobnika
                        PushStackTOKEN(stack, tmp); //Ale hodnotu ponechaj ulozenu na zasobniku
                        //UvolnovanieZasobnika = true;

//                      if(precedenceTable[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '<') {
//                          PushStackTOKEN(stack, ExprList->Act->Token);
//                      }
                    }

                    //Rovnaka priorita
                    if(precedenceTable[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '=' ){
                        PopStackTOKEN(stack);
//                    } else if (1) {
                    } else {
                        PushStackTOKEN(stack, ExprList->Act->Token);
                    }
                }
                //UvolnovanieZasobnika = false;
                ExprList->Act = ExprList->Act->next; //Dalsi Token
    }


    while(stack->top > 1){
//        tmp = PopStackTOKEN(stack);
//        if (tmp.type != LEFT_ROUND_BRACKET && tmp.type != RIGHT_ROUND_BRACKET) {
            PushStackTOKEN(stackOutput, PopStackTOKEN(stack));
//        }
    }
//    for(int i = 1; i<=stackOutput->top; i++) {
//        print_token(&(stackOutput)->stack[i]);
//    }

    return *stackOutput;
}

int FindRule(tDLList *ExprList, int *ErrorStatus) {



        bool CouldBeTokenTheLastOne = true;
        bool CouldBeTokenTheFristOne = true; //Osetri pravost, ze moze byt dany token prvy znak
//        ExprList->Act = ExprList->Act->next;
        do{

            if(ExprList->Act->next != NULL) {
                if (syntax_table[ExprList->Act->Token.type][ExprList->Act->next->Token.type] == false) {
                    *ErrorStatus = ERR_SYNTAX;
                }
            }

            //Len nil moze byt prijaty ako keyword
            if(ExprList->Act->Token.type == KEYWORD && (strcmp(ExprList->Act->Token.attribute, "nil") != 0)){
                    *ErrorStatus = ERR_SYNTAX;
            }


            //Kontrola zatvoriek
            if(ExprList->Act->Token.type == LEFT_ROUND_BRACKET){
                   syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] += 1;

            } else if(ExprList->Act->Token.type == RIGHT_ROUND_BRACKET &&
                     (syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] >= syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] ) ){
                         syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] += 1;
                  } else if(ExprList->Act->Token.type == RIGHT_ROUND_BRACKET){
                         *ErrorStatus = ERR_SYNTAX;
                  } else {
                         syntax_table[ExprList->Act->Token.type][COUNTER_OF_TOKEN] += 1;
                  }


            //Vyhodnotenie pre posledny token
            if( syntax_table[ExprList->Act->Token.type][SUCCESSFUL_END]){
                     CouldBeTokenTheLastOne = true;
            } else { CouldBeTokenTheLastOne = false; }

            if(CouldBeTokenTheFristOne == true && syntax_table[ExprList->Act->Token.type][SUCCESSFUL_START] == 0 ){
                *ErrorStatus = ERR_SYNTAX;
            }
            CouldBeTokenTheFristOne = false;

            ExprList->Act = ExprList->Act->next;
        } while (!((ExprList->Act == NULL) || ExprList->Act->Token.type == EOL || ExprList->Act->Token.type == EOF ));

        //Ak sa nerovna pocet zatvoriek alebo posledny token nemoze byt posledny vo vyraze(napriklad nemoze vyraz koncit s "+")
        if( syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] != syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] ||
            CouldBeTokenTheLastOne == false ||

            //Ak sa miesaju stringy a int/float
//            ((syntax_table[STRING][COUNTER_OF_TOKEN] != 0)
//             && (syntax_table[INTEGER][COUNTER_OF_TOKEN] +
//              syntax_table[FLOAT][COUNTER_OF_TOKEN]) != 0) ||

            //Ak je vo vyraze string a pocet operandov je parny
//            ((syntax_table[STRING][COUNTER_OF_TOKEN] > 0)
//             && ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN]) == 0)
//             &&  (syntax_table[STRING][COUNTER_OF_TOKEN]%2) == 0) ||

            //Maximalne jeden porovnavaci token
            (((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) != 0) &&

              (syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN] != 1)) ||

            //Pocet operacii je neparny
            //Sucet operatov musi byt parny ak neexistuje porovnavanie
            !(

                    //Ak nie je pritomne porovnanie
                   (((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
                      syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
                      syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) == 0)

                            && //musi byt

                                      //Potom je pocet operacii neparny
                                     (
                                      //Priklad 1+2+3
                                     (((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +
                                        syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 0)&&
                                      ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] +
                                      syntax_table[IDENTIFIER][COUNTER_OF_TOKEN] + syntax_table[STRING][COUNTER_OF_TOKEN])%2 == 1))

                                     || //alebo

                                      //Priklad 1+2+3+4
                                     (((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +
                                        syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 1)&&
                                      ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] +
                                      syntax_table[IDENTIFIER][COUNTER_OF_TOKEN] + syntax_table[STRING][COUNTER_OF_TOKEN])%2 == 0)))


                   )

                     || //alebo

                    //Ak je pritomne porovnanie
                   ((((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
                       syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
                       syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) == 1))

                            && //musi byt

                                    //Sucet operacii je neparny a sucet operandov je neparny
                                   ((((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +                                                                                                                 syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 1) &&
                                   ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN] +
                                   syntax_table[STRING][COUNTER_OF_TOKEN] + syntax_table[KEYWORD][COUNTER_OF_TOKEN])%2 == 1))

                                   || //alebo

                                    //Sucet operacii je parny a sucet operandov je parny
                                   (((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +                                                                                                                   syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 0) &&
                                   ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN] +
                                   syntax_table[STRING][COUNTER_OF_TOKEN] + syntax_table[KEYWORD][COUNTER_OF_TOKEN])%2 == 0)))
                   )

            )


        ){
            *ErrorStatus = ERR_SYNTAX;
        }
    return *ErrorStatus;
}

int MainSyntaxCheck(tDLList *ExprList) {
    if (ExprList == NULL) {
        return false; //Ziadny vyraz
    } else {

        //Inicializacia temporary
        int  ErrorStatus = ERR_OK;
        int *ErrorStatusPtr = &ErrorStatus;

        //Kontrolujeme syntax od zaciatku zoznamu
        ExprList->Act = ExprList->First;

        //Kontroluje syntakticke pravidla
        ErrorStatus =  FindRule(ExprList, ErrorStatusPtr);


        int static last_operation = no_operation_pTable;

        if(ErrorStatus == ERR_OK) { //Ak je error semanticku cast preskoc

            //Alokacia zasobnika
            tStackP *stack = malloc(sizeof(tStackP));
            tStackP *stackOutput = malloc(sizeof(tStackP));
            tStackINT *stackOutputINT = malloc(sizeof(tStackINT));
            tStackCHAR *stackOutputCHAR = malloc(sizeof(tStackCHAR));

            //Inicializacia zasobnika
            InitStackTOKEN(stack);
            InitStackTOKEN(stackOutput);
            InitStackINT(stackOutputINT);
            InitStackSTRING(stackOutputCHAR);


            //Rozlozenie vyrazu do postfixovej notacie
            ExprList->Act = ExprList->First;
            *stack = ParseToPostfix(ExprList, stack, stackOutput, last_operation);
            //Re-Inicializacia(Vycistenie) zasobnika
            InitStackTOKEN(stackOutput);

            //Vysledok daneho vyrazu
            EvaluateFromPostfix(ExprList, stack, stackOutput, stackOutputINT, stackOutputCHAR, last_operation, ErrorStatusPtr);

            free(stack);
            free(stackOutput);
            free(stackOutputINT);
            free(stackOutputCHAR);
        } else {
            return ErrorStatus; //ERR_SYNTAX return
        }
        return ErrorStatus; //ERR_OK return
    }
}


/**
 * Uvolni pamat v ktorom je nacitany vyraz
 * @param ExprList Zoznam s vyrazom
 */
void FreeBuffer(tDLList *ExprList){
    if(ExprList != NULL){
        if(ExprList->First != NULL){
            do {
                ExprList->Act = ExprList->First->next;
                free(ExprList->First);
                ExprList->First = ExprList->Act;
            }while(ExprList->Act != NULL);
            free(ExprList);
        }
    }
}



/**
 * Vynuluje hodnoty SyntaxTable ktore pocitaju pocet nacitanych jednotlivych tokenov 
 */
void CleanSyntaxTable(){
    //Cislo 21 je velkost tabulky
    for(int i=0; i<=21; i++) {
        syntax_table[i][COUNTER_OF_TOKEN] = 0;
    }

}

int CallExpressionParser(Token *token) {

    // Nastavenie typeFinal - typ vráteného výrazu - zatiaľ na undefined
    //typeFinal = T_UNDEFINED;
    finalVar = token->attribute;

    Token BufferToken;

    if(expression == true) {
        BufferToken = *token; //Sem dam plus
        *token = expression_token;
    }

    //Ulozenie adresy ktoru dostanem pre zapis posledneho tokenu
    Token *SaveMyToken = token;

    // inicializuj scanner najprv cez scanner_initialize()
    int ScannerErrorCheck = 0;
    if ( (ScannerErrorCheck = scanner_initialize()) != 0 ) {
        return ScannerErrorCheck;
    }

    tDLList *ExprArray = malloc(sizeof(tDLList));
            //Inicializacia
            ExprArray->First = NULL;
            ExprArray->Symbol = NULL;
            ExprArray->SyntaxChecker = NULL;
            ExprArray->Last = NULL;



    while   (token->type != EOL &&
             token->type != TYPE_EOF &&
            !(token->type == KEYWORD && (strcmp(token->attribute, "do") == 0)) &&
            !(token->type == KEYWORD && (strcmp(token->attribute, "then") == 0)))
            {

        LoadToBuffer(token, ExprArray);

        if(expression == false) {
            ScannerErrorCheck = get_next_token(token);
        } else if(expression == true) {
            expression = false;
            token = &BufferToken;
        }



        if(ScannerErrorCheck == ERR_SCANNER && token->type != TYPE_EOF){
            return ScannerErrorCheck;
        }

        //Zachovanie posledneho tokena
        SaveMyToken->type = token->type;
        SaveMyToken->attribute = token->attribute;
    }

    int MainSyntaxStatus = ERR_OK;
    MainSyntaxStatus = MainSyntaxCheck(ExprArray);

    FreeBuffer(ExprArray);
    CleanSyntaxTable();

    //true = error
    if (MainSyntaxStatus == ERR_OK) {
        // Ak bol do parseru výrazov poslaný iba jeden operand
        if (is_int(finalVar) || is_float(finalVar) || is_nil(finalVar) || is_string_literal(finalVar)) {
            char *var_name = expr_parser_create_unique_name(*actual_function_ptr);

            // nastavenie typeFinal
            if (is_int(finalVar))
                typeFinal = T_INT;
            else if (is_float(finalVar))
                typeFinal = T_FLOAT;
            else if (is_nil(finalVar))
                typeFinal = T_NIL;
            else if (is_string_literal(finalVar))
                typeFinal = T_STRING;

            if (is_in_while > 0) {
                // Je vo while, pridávať na zvlášť zoznam
                gen_defvar(var_name, &while_declaration_list); // DEFVAR LF@%param_id
            }
            else {
                gen_defvar(var_name, &instr_list); // DEFVAR LF@%param_id
            }
            gen_move_general(var_name, finalVar);
            finalVar = realloc(finalVar, sizeof(char) * strlen(var_name)+END_OF_STRING);
            strcpy(finalVar, var_name);
        }

        return ERR_OK;

    } else if (MainSyntaxStatus == ERR_SYNTAX) {
        return ERR_SYNTAX;

    } else if (MainSyntaxStatus == ERR_SCANNER) {
        return ERR_SCANNER;

    } else if (MainSyntaxStatus == ERR_SEM_UNDEF) {
        return ERR_SEM_UNDEF;

    } else if (MainSyntaxStatus == ERR_SEM_TYPE) {
        return ERR_SEM_TYPE;

    } else if (MainSyntaxStatus == ERR_SEM_PARAM) {
        return ERR_SEM_PARAM;

    } else if (MainSyntaxStatus == ERR_SEM_ELSE) {
        return ERR_SEM_ELSE;

    } else if (MainSyntaxStatus == ERR_ZERO_DIV) {
        return ERR_ZERO_DIV;

    } else if (MainSyntaxStatus == ERR_INTERNAL) {
        return ERR_INTERNAL;

    }

    return -10;
}
