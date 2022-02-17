/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavičkový súbor pre scanner
 * @file scanner.h
 *
 * @author Natália Holková (xholko02)
 * @author Albert Szöllösi (xszoll02)
 */

#ifndef _SCANNER_H
    #define _SCANNER_H

    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>

    #include "stringlib.h"
    #include "error.h"


    /** Typ tokenu **/
    typedef enum token_type {
        //Nazov tokenu       //ID_cislo
        NO_TYPE,             //0
        IDENTIFIER,          //1
        INTEGER,             //2
        FLOAT,               //3
        STRING,              //4
        KEYWORD,             //5
        ASSIGN,              //6
        ADDITION,            //7
        MULTIPLICATION,      //8
        SUBTRACTION,         //9
        DIVISION,            //10
        LESS,                //11
        LESS_OR_EQUALS,      //12
        GREATER,             //13
        GREATER_OR_EQUALS,   //14
        EQUALS,              //15
        NOT_EQUALS,          //16
        LEFT_ROUND_BRACKET,  //17
        RIGHT_ROUND_BRACKET, //18
        COMMA,               //19
        EOL,                 //20
        FUNCTION_ONLY_ID,    //21
        TYPE_EOF,            //22
        LEX_ERROR,           //23
    } Token_Type;

    /**
     * @struct Token
     */
    typedef struct token {
        Token_Type type; // Typ tokenu
        char *attribute; // Atribút tokenu
    } Token;

    /** Stavy konečného automatu **/
    typedef enum state {
        // Počiatočný stav
        START,

        // Koncové stavy
        F_ID,
        F_FUNCTION_ONLY_ID,
        F_INT,
        F_INT_0,
        F_FLOAT,
        F_FLOAT_WITH_E,
        F_FLOAT_WITH_E_0,
        F_STRING,
        F_ASSIGN, // =
        F_ADDITION, // +
        F_MULTIPLICATION, // *
        F_SUBTRACTION, // -
        F_DIVISION, // /
        F_LESS, // <
        F_GREATER, // >
        F_EQUALS, // ==
        F_NOT_EQUALS, // !=
        F_LESS_OR_EQUALS, // <=
        F_GREATER_OR_EQUALS, // >=
        F_LEFT_ROUND_BRACKET,  // (
        F_RIGHT_ROUND_BRACKET, // )
        F_COMMA, // ,
        F_EOL, // \n

        // Prechodné stavy
        Q_ID_UNDERSCORE,

        Q_LINE_COMMENT,

		Q_BLOCK_COMMENT_BEGIN_0,
        Q_BLOCK_COMMENT_BEGIN_1, // =b
        Q_BLOCK_COMMENT_BEGIN_2, // =be
        Q_BLOCK_COMMENT_BEGIN_3, // =beg
        Q_BLOCK_COMMENT_BEGIN_4, // =begi
        Q_BLOCK_COMMENT_BEGIN_5, // =begin
		Q_BLOCK_COMMENT_CONTENT, //obsah blokoveho komentara
        Q_BLOCK_COMMENT_END_1, // =
        Q_BLOCK_COMMENT_END_2, // =e
        Q_BLOCK_COMMENT_END_3, // =en
		Q_BLOCK_COMMENT_END_4, // =end
        Q_BLOCK_COMMENT_POSTEND, //obsah komentaru v riadku za "=end"

        Q_NOT_EQUALS,

        Q_FLOAT_1, // x.
        Q_FLOAT_2, // xe / xE
        Q_FLOAT_3, // xe+ / xE+ / xe- / xE-

        Q_STRING,
        Q_STRING_ESCAPE,
        Q_STRING_HEX_1,
        Q_STRING_HEX_2,

        F_LEX_ERROR // Lexikálna chyba
    } TState;

    // Reťazec, do ktorého sa bude načítavať
    TString *read_string;

    /**
     * @brief Inicializácia tokenu
     *
     * @return Ukazovateľ na inicializovaný token
     */
    Token *token_initialize();

    /**
     * @brief Uvoľnenie tokenu
     *
     * @param token Ukazovateľ na token
     */
    void token_free(Token *token);

    /**
     * @brief Zistenie, či je string kľúčové slovo
     *
     * @param str Reťazec
     * @return True ak je kľúčové slovo, inak false
     */
    bool is_keyword(char *str);

    /**
     * @brief Nastavenie typu a atribútu tokenu
     *
     * @param token Ukazovateľ na token
     * @param type Nový typ tokenu
     * @param attribute Nový atribút tokenu
     * @return Chybový kód
     */
    int token_set_type_attribute(Token *token, Token_Type type, char *attribute);

    /**
     * @brief Pomocná debugovacia funkica na výpis tokenu
     *
     * @param token Ukazovateľ na token
     */
    void print_token(Token *token);

    /**
     * @brief Vráti správny tvar float literálu, ktorý sa už môže priamo zapísať do inštrukcií
     *
     * @param floatStr Float reťazec
     * @return Správny formát float reťazca
     */
    char *get_correct_float_format(char *floatStr);

    /**
     * @brief Získanie ďalšieho tokenu zo stdin
     *
     * @param token Ukazovateľ na token, kde sa uloží výsledok
     * @return Chybový kód
     */
    int get_next_token(Token *token);

    /**
     * @brief Inicializácia scannera
     *
     * @return Chybový kód
     */
    int scanner_initialize();

    /**
    *  Pretypuje string na integer
    * @param x String na pretypovanie
    * @return Identicka hodnota ako integer
    */
    int string_to_integer(char* x);

    /**
    * Pretypuje integer na string
    * @param x Integerova hodnota na pretypovanie
    * @return Identicka hodnota v stringu
    */
    char* integer_to_string(int x);

    /**
     * @brief Dvojica hexa znakov na INT v desiatkovej sústave
     * @param c1 Prvý hexa znak
     * @param c2 Druhý hexa znak
     * @return Dekadický kód
     */
    int hex_chr_to_decadic_int(char c1, char c2);

    /**
     * @brief Číslo na string ako trojčíslie
     * @param n Dekadické číslo
     * @return Reťazec v tvare aaa, ked a je číslica
     */
    char *int_to_decadic_three(int n);
#endif
