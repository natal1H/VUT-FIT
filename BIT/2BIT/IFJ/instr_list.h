/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavičkový súbor pre prácu so inštrukciami a zoznamom inštrukcií
 * @file instr_list.h
 *
 * @author Natália Holková (xholko02)
 */

// Použitie zoznamu na uloženie inštrukcií bolo inšpirované z http://www.fit.vutbr.cz/study/courses/IFJ/public/project/ z ukážky jednoduchého interpretu

#ifndef _INSTR_LIST_H

    #define _INSTR_LIST_H

    #include <stdlib.h>
    #include <string.h>
    #include "error.h"

    /** Výpočet všetkých typov inštrukcií **/
    typedef enum {
        I_UNDEFINED, // Špeciálny typ - po inicializácii pred nastavením skutočného typu

        I_HEADER, // Hlavička
        I_COMENT, // Komentár
        
        // Práca s rámcami, volanie funkcií
        I_MOVE,
        I_CREATEFRAME,
        I_PUSHFRAME,
        I_POPFRAME,
        I_DEFVAR,
        I_CALL,
        I_RETURN,
        // Práca s datovým zásobníkom
        I_PUSHS,
        I_POPS,
        I_CLEARS,
        // Aritmetické, relačné, booleovské a konverzné inštrukcie
        I_ADD, I_SUB, I_MUL, I_DIV, I_IDIV,
        I_ADDS, I_SUBS, I_MULS, I_DIVS, I_IDIVS,
        I_LT, I_GT, I_EQ,
        I_LTS, I_GTS, I_EQS,
        I_AND, I_OR, I_NOT,
        I_ANDS, I_ORS, I_NOTS,
        I_INT2FLOAT, I_FLOAT2INT, I_INT2CHAR, I_STRI2INT,
        I_INT2FLOATS, I_FLOAT2INTS, I_INT2CHARS, I_STRI2INTS,
        // Vstupno-výstupné inštrukcie
        I_READ,
        I_WRITE,
        I_CONCAT,
        I_STRLEN,
        I_GETCHAR,
        I_SETCHAR,
        // Práca s typy
        I_TYPE,
        // Inštrukcie pre riadenie toku programu
        I_LABEL,
        I_JUMP,
        I_JUMPIFEQ,
        I_JUMPIFNEQ,
        I_JUMPIFEQS, I_JUMPIFNEQS,
        // Ladiace inštrukcie
        I_BREAK,
        I_DPRINT
    } tInstruction_type;

    /** @struct Inštrukcia **/
    typedef struct {
        tInstruction_type instType;  // typ instrukce
        char *addr1; // adresa 1
        char *addr2; // adresa 2
        char *addr3; // adresa 3
    } tInstr;

    /** @struct Položka zoznamu inštrukcií **/
    typedef struct listItem {
        tInstr *Instruction;
        struct listItem *nextItem;
    } tListItem;

    /** @struct Jednosmerný zoznam inštrukcií **/
    typedef struct {
        struct listItem *first;
        struct listItem *last;
        struct listItem *active;
    } tListOfInstr;

    /**
     * @brief Inicializácia zoznamu
     *
     * @param L Ukazovateľ na zoznam inštrukcií
     */
    void listInit(tListOfInstr *L);

    /**
     * @brief Uvoľnenie zoznamu inštrukcií
     *
     * @param L Ukazovateľ na zoznam inštrukcií
     */
    void listFree(tListOfInstr *L);

    /**
     * @brief Nastavenie aktívneho prvku na prvý prvok
     *
     * @param L Ukazovateľ na zoznam inštrukcií
     */
    void listFirst(tListOfInstr *L);

    /**
     * @brief Nastavenie aktívneho prvku na nasledujúci prvok
     *
     * @param L Ukazovateľ na zoznam inštrukcií
     */
    void listNext(tListOfInstr *L);

    /**
     * @brief Nastavenie aktívneho prvku na posledný prvok
     *
     * @param L Ukazovateľ na zoznam inštrukcií
     */
    void listLast(tListOfInstr *L);

    /**
     * @brief Vrátenie ukazovateľa na aktívny prvok
     *
     * @param L Ukazovateľ na zoznam inštrukcií
     * @return Ukazovateľ na aktívny prvok
     */
    tListItem *listGetActivePtr(tListOfInstr *L);

    /**
     * @brief Vytvorenie inštrukcie
     *
     * @param type Typ
     * @param addr1 Reťazec na adrese 1
     * @param addr2 Reťazec na adrese 2
     * @param addr3 Reťazec na adrese 3
     * @return Ukazovateľ na vytvorenú inštrukciu
     */
    tInstr *tInstr_create(tInstruction_type type, char *addr1, char *addr2, char *addr3);

    /**
     * @brief Výpis samotnej inštrukcie
     *
     * @param I Ukazovateľ na inštrukciu
     */
    void tInstr_print_single_instruction(tInstr *I);

    /**
     * @brief Výpis všetkých inštrukcií v zozname
     * @param L Zoznam inštrukcií
     */
    void list_print_instructions(tListOfInstr *L);

    /**
     * @brief Uvoľnenie inštrukcie
     * @param I Ukazovateľ na inštrukciu
     */
    void tInst_free_instruction(tInstr *I);

    /**
     * @brief Inicializácia inštrukcie
     * @return Ukazovateľ na inštrukciu
     */
    tInstr *tInstr_init();

    /**
     * @brief Nastavenie inštrukcie
     *
     * @param instr Ukazovateľ na inštrukciu, ktorú treba nastaviť
     * @param type Typ
     * @param addr1 Reťazec na adrese 1
     * @param addr2 Reťazec na adrese 2
     * @param addr3 Reťazec na adrese 3
     * @return Chybový kód
     */
    int tInstr_set_instruction(tInstr *instr, tInstruction_type type, char *addr1, char *addr2, char *addr3);

    /**
     * @brief Vloženie inštrukcie do zoznamu za aktívny prvok
     *
     * @param L Ukazovateľ na zoznam inštrukcií
     * @param I Ukazovateľ na inštrukciu
     */
    void listInsertPostActive(tListOfInstr *L, tInstr *I);

#endif