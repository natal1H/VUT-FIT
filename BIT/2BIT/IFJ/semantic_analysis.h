/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavičkový súbor pre sémantické funkcie
 * @file semantic_analysis.h
 *
 * @author Natália Holková (xholko02)
 */

#ifndef _SEMANTIC_ANALYSIS_H

    #define _SEMANTIC_ANALYSIS_H

    #include <stdbool.h>
    #include <string.h>
    #include "symtable.h"
    #include "error.h"

    #include "expression_parser.h" // Kvôli funkcie na prevod int na string

    // Funkcie na semantickú analýzu v hlavnom parseri

    /**
     * @brief Pozrie, či funkcia už bola definovaná v globálnej tabuľke symbolov
     *
     * @param global_table Globálna tabuľka symbolov
     * @param function_id Názov funkcie
     * @return True ak už bola definovaná, false ak nebola definovaná
     */
    bool check_if_function_already_defined(tGlobalTableNodePtr global_table, char *function_id);

    /**
     * @brief Vytvorí uzol pre funkciu v globálnej tabuľke symbolov
     *
     * @param global_table Ukazovateľ na globálnu tabuľku symbolov
     * @param function_id Názov funkcie
     * @return Chybový kód (napr. redefinícia)
     */
    int function_definition(tGlobalTableNodePtr *global_table, char *function_id);

    // Funkcie na semantickú analýzu v parseri výrazov

    /**
     * @brief Zistí, či môže byť string INT
     *
     * @param str Skúmaný Reťazec
     * @return True ak môže byť INT, false ak nemôže
     */
    bool is_int(char *str);

    /**
     * @brief Zistí, či môže byť string FLOAT
     * @param str Skúmaný reťazec
     * @return True ak môže byť FLOAT, false ak nemôže
     */
    bool is_float(char *str);

    /**
     * @brief Zistí, či môže byť string NIL
     * @param str Skúmaný reťazec
     * @return True ak môže byť NIL, false ak nemôže
     */
    bool is_nil(char *str);

    /**
     * @brief Zistí, či môže byť reťazec premennou
     *
     * @param current_function_root Aktuálna lokálna tabuľka symbolov
     * @param str Skúmaný reťazec
     * @return True ak môže byť premennou, false ak nemôže
     */
    bool is_variable(tLocalTableNodePtr current_function_root, char *str);

    /**
     * @brief Zistí, či je reťazec reťazcový literál (začína sa " a končí " )
     *
     * @param str Skúmaný reťazec
     * @return True ak môže byť reťazcový literál, false ak nemôže
     */
    bool is_string_literal(char *str);

    /**
     * @brief Vráti string bez úvodných a koncových "
     * @param string_literal Reťazcový literál
     * @return Reťazec
     */
    char *get_string_without_quotation_marks(char *string_literal);

    /**
     * @brief Vytvára unikátne meno pre premennú
     *
     * @param local_table Lokálna tabuľka
     * @return Reťazec s unikátny menom alebo NULL pri chybe pri alokovaní
     */
    char *expr_parser_create_unique_name(tLocalTableNodePtr local_table);

    /**
     * @brief Vytvára unikátne meno pre premennú
     *
     * @param local_table Lokálna tabuľka
     * @param prefix Prefix identifikátora
     * @return Reťazec s unikátny menom alebo NULL pri chybe pri alokovaní
     */
    char *expr_parser_create_unique_name_with_prefix(tLocalTableNodePtr local_table, char *prefix);

    /**
     * @brief Uloží typ tokenu podľa reťazca, kotroluje zároveň sémantické chyby
     *
     * @param current_function_root Lokálna tabuľka symbolov
     * @param token_ID Reťazec
     * @param type Ukazovateľ na typ, kde sa uloží výsledný typ
     * @return Chybový kód (ERR_OK/ERR_SEM_UNDEF)
     */
    int get_type_from_token(tLocalTableNodePtr *current_function_root, char *token_ID, tDataType *type);

    /**
     * @brief Skúma kompatibilitu typov pri aritmetický operáciách
     *
     * @param type1 Datový typ prvého operanda
     * @param type2 Datový typ druhého operanda
     * @return Chybový kód
     */
    int arithmetic_check_compatibility(tDataType type1, tDataType type2);

    /**
     * @brief Vráti výsledný typ aritmetickej operácie
     * @param token1 Datový typ prvého operanda
     * @param token2 Datový typ druhého operanda
     * @return Výsledný typ
     */
    tDataType aritmetic_get_final_type(tDataType token1, tDataType token2);

    /**
     * @brief Skúma kompatibilitu typov pri operáciách porovnania
     *
     * @param type1 Datový typ prvého operandu
     * @param type2 Datový typ druhého operandu
     * @return Chybový kód
     */
    int comparison_check_compatibility(tDataType type1, tDataType type2);

    /**
    * @brief Podľa identifikátora funkcie zistí, či sa náhodou nejedná o vstavanú funkciu
    *
    * @param function_id Identifikátor funkcie
    * @return True ak ide o vstavanú funkciu, inak false
    */
    bool is_built_in_function(char *function_id);

    /**
    * @brief Nastaví správny počet parametrov pre danú vstavanú funkciu
     *
    * @param rootPtr Koreň globálnej tabuľky symbolov
    * @param function_id
    */
    void built_in_function_set_param(tGlobalTableNodePtr rootPtr, char *function_id);

    /**
     * @brief
     *
     * @param function_id
     * @param number_of_param
     * @param type
     * @return
     */
    int built_in_function_check_arg(char *function_id, int number_of_param, tDataType type);

    /**
     * @brief Vráti návratový typ vstavanej funkcie
     *
     * @param function_id Názov vstavanej funkcie
     * @return Návratový typ vstavanej funkcie
     */
    tDataType built_in_function_get_return_type(char *function_id);

#endif