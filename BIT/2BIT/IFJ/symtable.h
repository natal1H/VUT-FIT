/**
 * IFJ - Projekt Team 11
 *
 * @brief Hlavičkový súbor tabuľky symbolov
 *
 * @author Natália Holková (xholko02)
 */

#ifndef _SYMTABLE_H
    #define _SYMTABLE_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <stdbool.h>

    #include "stringlib.h"
    #include "error.h"

    // Datový typ premennej
    typedef enum {
        T_INT,      //0
        T_FLOAT,    //1
        T_STRING,   //2
        T_BOOLEAN,  //3
        T_NIL,      //4
        T_UNDEFINED,//5
        T_PARAM,    //6
    } tDataType;

    /**
     * @struct Dáta uzla lokálnej tabuľky symbolov
     */
    typedef struct {
        tDataType type; // Datový typ premennej
        bool defined; // Bola definovaná premenná?
    } tDataNodeLocal;

    /**
     * @struct Lokálna tabuľka symbolov
     */
    typedef struct tLocalTableNode {
        char *id; // Identifikátor premennej
        tDataNodeLocal *data; // Dáta

        struct tLocalTableNode * lPtr; // Ukazovateľ na ľavý podstrom
        struct tLocalTableNode * rPtr; // Ukazovateľ na pravý podstrom
    } * tLocalTableNodePtr;

    /**
     * @struct Prvok zoznamu parametrov
     */
    typedef struct paramListItem {
        char *param_id;
        struct paramListItem *next;
    } TParamListItem;

    /**
     * @struct Zoznam parametrov funkcie
     */
    typedef struct paramlist {
        TParamListItem *first;
        TParamListItem *act;
        TParamListItem *last;
    } TParamList;

    /**
     * @brief Inicializácia zoznamu parametrov
     *
     * @param L Zoznam parametrov
     */
    void ParamListInit(TParamList *L);

    /**
     * @brief Vloženie parametru na koniec zoznamu
     *
     * @param L Zoznam parametrov
     * @param id Identifikátor parametru
     */
    void ParamListInsertLast(TParamList *L, char *id);

    /**
     * @brief Nastavenie aktívneho prvku na prvý prvok
     *
     * @param L Ukazovateľ na zoznam parametrov
     */
    void ParamListFirst(TParamList *L);

    /**
     * @brief Nastavenie aktívneho prvku na nasledujúci prvok
     *
     * @param L Ukazovateľ na zoznam parametrov
     */
    void ParamListNext(TParamList *L);

    /**
     * @brief Vráti identifikátor aktívneho prvku
     *
     * @param L Ukazovateľ na zoznam
     * @return Identifikátor aktívneho prvku
     */
    char *ParamListGetActive(TParamList *L);

    /**
     * @brief Uvoľnenie zoznamu parametrov
     *
     * @param L Ukazovateľ na zoznam
     */
    void ParamListDispose(TParamList *L);

    /**
     * @struct Dáta uzla globálnej tabuľky symbolov
     */
    typedef struct {
        bool defined; // Bola definovaná funkcia?
        int params; // Počet parametrov funkcie
        TParamList *paramList; // Zoznam identifikátorov parametrov
        tLocalTableNodePtr  *function_table; // Odkaz na lokálnu tabuľku symbolov funkcie
        tDataType returnType; // Typ návratovej hodnoty
    } tDataNodeGlobal;

    /**
     * @struct Globálna tabuľka symbolov
     */
    typedef struct tGlobalTableNode {
        char *id; // Identifikátor funkcie
        tDataNodeGlobal *data; // Dáta

        struct tGlobalTableNode * lPtr; // Ukazovateľ na ľavý podstrom
        struct tGlobalTableNode * rPtr; // Ukazovateľ na pravý podstrom
    } * tGlobalTableNodePtr;

    // Funkcie pre prácu s tabuľkami symbolov
    // Globálna
    // Všeobecné funkcie

    /**
     * @brief Inicializácia globálnej tabuľky symbolov
     *
     * @param rootPtr Ukazovateľ na globálnu tabuľku symbolov
     */
    void global_table_init(tGlobalTableNodePtr *rootPtr);

    /**
     * @brief Vloženie uzla (funkcie) do globálnej tabuľky symbolov
     *
     * @param rootPtr Ukazovateľ na koreň globálnej tabuľky symbolov
     * @param id Názov funkcie
     * @param data Ukazovateľ na dáta
     * @return Chybový kód
     */
    int global_table_insert(tGlobalTableNodePtr *rootPtr, char *id, tDataNodeGlobal *data);

    /**
     * @brief Vyhľadanie uzla (funkcie) v globálnej tabuľke symbolov podľa identifikátora
     *
     * @param rootPtr Ukazovateľ na koreň globálnej tabuľky symbolov
     * @param id Názov funkcie
     * @param data Ukazovateľ kam uložiť dáta nájdenej funkcie
     * @return True ak našlo uzol (funkciu), false ak nenašlo
     */
    bool global_table_search(tGlobalTableNodePtr rootPtr, char *id, tDataNodeGlobal **data);

    /**
     * @brief Nahradenie uzla najpravejším
     *
     * @param ptrReplaced Uzol, ktorý sa má nahradiť
     * @param rootPtr Ukazovateľ na koreň
     */
    void global_table_replace_by_rightmost(tGlobalTableNodePtr ptrReplaced, tGlobalTableNodePtr *rootPtr);

    /**
     * @brief Vymazanie uzla z globálnej tabuľky symbolov podľa identifikátora
     * @param rootPtr Ukazovateľ na koreň stromu
     * @param id Identifikátor uzla (funkcie), ktorý sa má vymazať
     */
    void global_table_delete(tGlobalTableNodePtr *rootPtr, char *id);

    /**
     * @brief Vymazanie celej globálnej tabuľky symbolov
     *
     * @param rootPtr Ukazovateľ na koreň stromu
     */
    void global_table_dispose(tGlobalTableNodePtr *rootPtr);

    /**
     * @brief Pomocná funkcia na výpis binárneho stromu
     *
     * @param TempTree
     * @param sufix
     * @param fromdir
     */
    void global_table_print_tmp(tGlobalTableNodePtr TempTree, char* sufix, char fromdir);

    /**
     * @brief Výpis štruktúry binárneho stromu
     *
     * @param TempTree Koreň binárneho stromu, ktorý chceme vypísať
     */
    void global_table_print(tGlobalTableNodePtr TempTree);

    // Špeciálne pre globálnu tabuľku

    /**
     * @brief Pridá identifikátor parametra do zoznamu parametrov funkcie
     *
     * @param rootPtr Koreň globálnej tabuľky symbolov
     * @param function_id Názov funkcie
     * @param param_id Identifikátor parametra
     */
    void function_add_param_id_to_list(tGlobalTableNodePtr rootPtr, char *function_id, char *param_id);

    /**
     * @brief Nastavenie prvého prvku v zozname parametrov funkcie na aktívny
     *
     * @param rootPtr Ukazovateľ na globálnu tabuľku symbolov
     * @param function_id Názov funkcie
     */
    void function_param_list_set_first_active(tGlobalTableNodePtr rootPtr, char *function_id);

    /**
    * @brief Nastavenie aktívneho prvku v zozname parametrov funkcie na nasledujúci
    *
    * @param rootPtr Ukazovateľ na globálnu tabuľku symbolov
    * @param function_id Názov funkcie
    */
    void function_param_list_next(tGlobalTableNodePtr rootPtr, char *function_id);

    /**
     * @brief Vrátenie názvu aktívneho parametru funkcie
     * @param rootPtr Ukazovateľ na globálnu tabuľku symbolov
     * @param function_id Názov funkcie
     * @return Identifikátor aktívneho parametra
     */
    char *function_param_list_get_active(tGlobalTableNodePtr rootPtr, char *function_id);

    /**
     * @brief Funkcia na vrátenie počtu parametrov funkcie
     *
     * @param rootPtr Koreň globálnej tabuľky symbolov
     * @param function_id Názov funkcie
     * @return Počet parametrov alebo -1 ak nenašlo funkciu
     */
    int function_get_number_params(tGlobalTableNodePtr rootPtr, char *function_id);

    /**
     * @brief Zvýšenie počtu parametrov funkcie o 1
     *
     * @param rootPtr Koreň globálnej tabuľky symbolov
     * @param function_id Názov funkcie
     */
    void function_increase_number_params(tGlobalTableNodePtr rootPtr, char *function_id);

    /**
     * @brief Nastavenie počtu parametrov funkcie na určitú hodnotu
     *
     * @param rootPtr Koreň globálnej tabuľky symbolov
     * @param function_id Názov funkcie
     * @param number Nový počet parametrov
     */
    void function_set_number_params(tGlobalTableNodePtr rootPtr, char *function_id, int number);

    /**
     * @brief Nastavanie funkcie ako definovanej
     *
     * @param rootPtr Ukazovateľ na globálnu tabuľku symbolov
     * @param id Názov funkcie
     * @return Chybový kód
     */
    int function_set_defined(tGlobalTableNodePtr *rootPtr, char *id);

    /**
     * @brief Získanie uzla s funkciou podľa identifikátora
     * @param rootPtr Koreň stromu
     * @param id Identifikátor funkcie
     * @return Ukazovateľ na uzol funkcie
     */
    tGlobalTableNodePtr get_function_node(tGlobalTableNodePtr rootPtr, char *id);

    /**
     * @brief Nastavanie odkazu na lokálnu tabuľku symbolov funkcie
     *
     * @param function_node_ptr Ukazovateľ na uzol s funkciou v globálnej tabuľke symbolov
     * @param local_table_ptr Uzovateľ na lokálnu tabuľku, ktorá sa má previazať s funkciou
     */
    void set_function_table(tGlobalTableNodePtr *function_node_ptr, tLocalTableNodePtr *local_table_ptr);

    /**
     * @brief Nastaví návratový typ funkcie
     *
     * @param rootPtr Koreň globálnej tabuľky symbolov
     * @param id Názov funkcie
     * @param returning_type Návratový typ na nastavenie
     */
    void set_fuction_return_type(tGlobalTableNodePtr rootPtr, char *id, tDataType returning_type);

    /**
     * @brief Vráti typ návratovej hodnoty funkcie
     *
     * @param rootPtr Koreň globálnej tabuľky symbolov
     * @param id Názov funkcie
     * @return Návratový typ funkcie
     */
    tDataType get_fuction_return_type(tGlobalTableNodePtr rootPtr, char *id);


// Lokálna
    // Všeobecné funkcie

    /**
     * @brief Inicializácia lokálnej tabuľky symbolov
     *
     * @param rootPtr Ukazovateľ na koreň lokálnej tabuľky symbolov
     */
    void local_table_init(tLocalTableNodePtr *rootPtr);

    /**
     * @brief Vloženie uzla (premennej) do lokálnej tabuľky symbolov
     *
     * @param rootPtr Ukazovateľ na lokálnu tabuľku symbolov
     * @param id Názov premennej
     * @param data Dáta, ktoré sa majú uložiť do uzla
     * @return Chybový kód
     */
    int local_table_insert(tLocalTableNodePtr *rootPtr, char *id, tDataNodeLocal *data);

    /**
     * @brief Vyhľadanie premennej v lokálnej tabuľke symbolov podľa identifikátora
     *
     * @param rootPtr Ukazovateľ na lokálnu tabuĺku symbolov
     * @param id Názov premennej
     * @param data Kam uložiť vyhľadané dáta
     * @return True ak našlo premennú (uzol), false ak nenašlo
     */
    bool local_table_search(tLocalTableNodePtr rootPtr, char *id, tDataNodeLocal **data);

    /**
     * @brief Nahradenie uzla najpravejším
     *
     * @param ptrReplaced Uzlo, ktorý sa má nahradiť
     * @param rootPtr Ukazovateľ na koreň stromu
     */
    void local_table_replace_by_rightmost(tLocalTableNodePtr ptrReplaced, tLocalTableNodePtr *rootPtr);

    /**
     * @brief Vymazanie premennej z lokálnej tabuľky symbolov podľa identifikátora
     * @param rootPtr Ukazovateľ na lokálnu tabuľku symbolov
     * @param id Názov premennej
     */
    void local_table_delete(tLocalTableNodePtr *rootPtr, char *id);

    /**
     * @brief Vymazanie celej lokálnej tabuľky symbolov
     *
     * @param rootPtr Ukazovateľ na lokálnu tabuľku symbolov
     */
    void local_table_dispose(tLocalTableNodePtr *rootPtr);

    /**
     * @brief Pomocná funkcia na vypísanie lokálnej tabuľky symbolov
     *
     * @param TempTree
     * @param sufix
     * @param fromdir
     */
    void local_table_print_tmp(tLocalTableNodePtr TempTree, char* sufix, char fromdir);

    /**
     * @brief Výpis lokálnej tabuľky symbolov
     *
     * @param TempTree Lokálna tabuľky symbolov
     */
    void local_table_print(tLocalTableNodePtr TempTree);

    // Špeciálne pre lokálnu tabuľku

    /**
     * @brief Nastanie premennej na defined
     *
     * @param rootPtr Ukazovateľ na lokálnu tabuľku symbolov
     * @param id Názov premennej
     * @return Chybový kód
     */
    int variable_set_defined(tLocalTableNodePtr *rootPtr, char *id);

    /**
     * @brief Nastavenie datového typu premennej
     *
     * @param function_table Lokálna tabuľka symbolov
     * @param id Názov premennej
     * @param type Typ premennej
     */
    void variable_set_type(tLocalTableNodePtr function_table, char *id, tDataType type);

    /**
     * @brief Vrátenie typu premennej
     *
     * @param function_table Lokálna tabuľka symbolov
     * @param id Názov premennej
     * @return Typ premennej
     */
    tDataType variable_get_type(tLocalTableNodePtr function_table, char *id);

    /**
     * @brief Vrátenie celého uzla s premennou
     *
     * @param rootPtr Lokálna tabuľka symbolov
     * @param id Názov premennej
     * @return Uzol s premennou
     */
    tLocalTableNodePtr get_variable_node(tLocalTableNodePtr rootPtr, char *id);

    // Upratanie

    /**
     * @brief Vymazanie všetkých lokálnych tabuliek funkcií v globálnej tabuľke symbolov
     *
     * @param global_root
     */
    void symbol_table_dispose_all_local(tGlobalTableNodePtr *global_root);

#endif