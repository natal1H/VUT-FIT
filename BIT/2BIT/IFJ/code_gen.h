/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavičkový súbor pre generovanie kódu
 * @file code_gen.h
 *
 * @author Natália Holková (xholko02)
 * @author Matej Novák (xnovak2f)
 */

#ifndef _CODE_GEN_H

    #define _CODE_GEN_H

    #include <stdbool.h>
    #include <stdio.h>

    #include "stringlib.h"
    #include "error.h"
    #include "instr_list.h"
    #include "semantic_analysis.h"
    #include "symtable.h"

    #define VAR_PREFIX_LEN 4 // LF@%
    #define VAR_PREFIX "LF@%"
    #define LABEL_PREFIX "$"
    #define FUNCTION_PREFIX "?"

    // Štruktúra - špeciálna lokálna tabuľka na názvy návestí
    tLocalTableNodePtr label_table;

    // štruktúra - zoznam inštrukcií
    tListOfInstr instr_list;

    // Pomocná štruktúra - aktuálna inštrukcia
    tInstr *curr_instr;

    // Kam sa vrátiť z funkcie
    tListItem *returnPlaceCopy;

    // Generovanie while
    tListOfInstr while_declaration_list; // Zoznam inštrukcií deklarácií, ktoré by inak boli vo while
    tListItem *active_before_while; // Inštrukcia tesne pred začiatkom while

    // Zásobník parametrov a základné funkcie preň
    /** @struct Element zásobníka parametrov **/
    typedef struct tparam {
       char *id; // Identifikátor parametra
       struct  tparam *next; // Nasledujúci parameter
    } TParam;

    /** @struct Zásobník parametrov **/
    typedef struct {
        TParam *top;
    } TParamStack;

    // Zásobník na parametre
    TParamStack stackParam;

    /**
     * @brief Inicializácia zásobníka parametrov
     *
     * @param s Zásobník parametrov
     */
    void ParamStackInit(TParamStack *s);

    /**
     * @brief Vloženie parametra na zásobník parametrov
     *
     * @param s Zásobník parametrov
     * @param id Identifikátor parametra
     */
    void ParamStackPush(TParamStack *s, char *id);

    /**
     * @brief Odstránenie hodnoty z vrcholu zásobníka parametrov
     *
     * @param s Zásobník parametrov
     */
    void ParamStackPop(TParamStack *s);

    /**
     * @brief Vrátenie hodnoty z vrcholu zásobníka parametrov
     *
     * @param s Zásobník parametrov
     * @return Identifikátor parametra
     */
    char * ParamStackTop(TParamStack *s);

    /**
     * @brief Pozrie, či je zásobník parametrov prázdny
     *
     * @param s Zásobník parametrov
     * @return True ak je prázdny, false ak nie je
     */
    bool ParamStackEmpty(TParamStack *s);

    /**
     * @brief Funkcia na prípravu generovania kódu

     * @return chybový kód
     */
    int code_gen_start();

    /**
     * @brief Funkcia, ktorá uprace po generovaní kódu
     *
     * @return Chybový kód
     */
    int code_gen_end();

    // Pomocné funkcie pre generovanie rôznych situácií
    // Aritmetické inštrukcie

    /**
     * @brief Vygenerovanie inštrukcie ADD
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_add(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    /**
     * @brief Vygenerovanie inštrukcie SUB
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_sub(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    /**
     * @brief Vygenerovanie inštrukcie MUL
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_mul(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    /**
     * @brief Vygenerovanie inštrukcie DIV
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_div(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    /**
     * @brief Vygenerovanie inštrukcie IDIV
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_idiv(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    // Operácie porovnávania

    /**
     * @brief Vygenerovanie inštrukcie LT
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_lt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    /**
     * @brief Vygenerovanie inštrukcie GT
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_gt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    /**
     * @brief Vygenerovanie inštrukcie EQ
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_eq(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    // Logické operácie

    /**
     * @brief Vygenerovanie inštrukcie OR
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec prvého operandu
     * @param symbol2 Reťazec druhého operandu
     * @return Chybový kód
     */
    int gen_or(char *var_name, char *symbol1, char *symbol2);

    /**
     * @brief Vygenerovanie inštrukcie NOT
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec prvého operandu
     * @return Chybový kód
     */
    int gen_not(char *var_name, char *symbol);

    // Inštrukcie na prevody

    /**
     * @brief Vygenerovanie inštrukcie na prevod INT na FLOAT
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol Reťazec operandu
     * @param is_var Je premenná?
     * @return Chybový kód
     */
    int gen_int2float(char *var_name, char *symbol, bool is_var);

    /**
     * @brief Pretypovanie INT na FLOAT
     *
     * @param actual_function_ptr Ukazovateĺ na aktuálnu lokálnu tabuľku symbolov
     * @param symbol Reťazec operandu
     * @param converted_name Názov premennej, do ktorej sa pretypuje
     * @param is_in_while Nachádza sa v cykle?
     */
    void convert_int_2_float(tLocalTableNodePtr *actual_function_ptr, char *symbol, char **converted_name, int is_in_while);

    // Práca s reťazcami

    /**
     * @brief Vygenerovanie inštrukcie CONCAT
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @return Chybový kód
     */
    int gen_concat(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    // Skoky

    /**
     * @brief Vygenerovanie nepodmieneného skoku
     *
     * @param label Návestie
     */
    void gen_jump(char *label);

    /**
     * @brief Vygenerovanie podmieného skoku JUMPIFNEQ
     *
     * @param label Návestie
     * @param var Premenná, ktorá sa porovnáva s true
     */
    void gen_jumpifneq(char *label, char *var);

    void gen_jumpifneq_general(char *label, char *var1, char *var2_complete);

    /**
     * @brief Vygenerovanie podmieného skoku JUMPIFNEQ
     *
     * @param label Návestie
     * @param var Premenná, ktorá sa porovnáva s true
     */
    void gen_jumpifeq(char *label, char *var);

    /**
     * @brief Vygenerovanie návestia
     *
     * @param label Názov návestia
     */
    void gen_label(char *label);

    /**
     * @brief Vygenerovanie deklarovania premennej
     *
     * @param var_name Názov premennej
     * @param L Do ktorého zoznamu inštrukcií generovať kód
     * @return Chybový kód
     */
    int gen_defvar(char *var_name, tListOfInstr *L);

    /**
     * @brief Vygenerovanie MOVE
     *
     * @param var_name Názov premennej
     * @param symbol Reťazec symbolu
     * @return Chybový kód
     */
    int gen_move_var(char *var_name, char *symbol);

    /**
     * @brief Vygenerovanie všeobecného MOVE (môžu byť aj literály)
     * @param var_name Názov premennej
     * @param symbol Reťazec symbolu
     * @return Chybový kód
     */
    int gen_move_general(char *var_name, char *symbol);

    // Ďalšie pomocné funkcie

    /**
     * @brief Vytvorenie reťazca s prefixom
     * @param str Reťazec
     * @param prefix Prefix na pridanie
     * @return Reťazec s prefixom
     */
    char *get_string_with_prefix(char *str, char *prefix);

    /**
     * @brief Vygenerovanie inštrukcie s tromi operandmi
     *
     * @param type Typ inštrukcie
     * @param var_name Názov premennej, kam sa priradí výsledok
     * @param symbol1 Reťazec prvého symbolu
     * @param symbol2 Reťazec druhého symbolu
     * @param symbol1_prefix Prefix prvého symbolu
     * @param symbol2_prefix Prefix druhého symbolu
     * @return Chybový kód
     */
    int add_instruction_ternal(tInstruction_type type, char *var_name, char *symbol1, char *symbol2, char *symbol1_prefix, char *symbol2_prefix);

    /**
    * @brief Vygenerovanie inštrukcie s dvoma operandmi
    *
    * @param type Typ inštrukcie
    * @param var_name Názov premennej, kam sa priradí výsledok
    * @param symbol Reťazec prvého symbolu
    * @param symbol_prefix Prefix prvého symbolu
    * @return Chybový kód
    */
    int add_instruction_binary(tInstruction_type type, char *var_name, char *symbol, char *symbol_prefix);

    /**
    * @brief Vygenerovanie inštrukcie s jedným operandom
    *
    * @param type Typ inštrukcie
    * @param symbol Reťazec symbolu
    * @param symbol_prefix Prefix symbolu
    * @return Chybový kód
    */
    int add_instruction_unary(tInstruction_type type, char *symbol, char *symbol_prefix);

    /**
     * @brief Určenie prefixu na základe typu
     *
     * @param type Datový typ
     * @param is_var Jedná sa o premennú?
     * @return Prefix
     */
    char *determine_prefix(tDataType type, bool is_var);

    /**
     * @brief Vytvorenie unikátneho názvu návestia
     *
     * @param label_table Odkaz na tabuľku s návestiami
     * @param prefix Prefix návestia
     * @return Názov výsledného návestia
     */
    char *get_and_set_unique_label(tLocalTableNodePtr *label_table, char *prefix);

    /**
     * @brief Nastavenie a vloženie inštrukcie do zoznamu
     *
     * @param L Ukazovateľ na zoznam s inštrukciami
     * @param I Ukazovateľ na pomocnú inštrukciu
     * @param type Typ inštrukcie
     * @param addr1 Reťazec na adrese 1
     * @param addr2 Reťazec na adrese 2
     * @param addr3 Reťazec na adrese 3
     */
    void set_and_post_instr(tListOfInstr *L, tInstr *I, tInstruction_type type, char *addr1, char *addr2, char *addr3);

    // Generovanie funkcií

    /**
     * @brief Vygenerovanie PUSHS
     *
     * @param symbol Reťazec symbolu
     * @param type Typ
     * @param is_var Je premenná?
     * @return Chybový kód
     */
    int gen_push_var(char *symbol, tDataType type, bool is_var);

    /**
     * @brief Vygenerovanie POPS
     *
     * @param var_name Názov premennej kam sa uloží
     * @return Chybový kód
     */
    int gen_pop_var(char *var_name);

    /**
     * @brief Vygenerovanie volania funkcie
     *
     * @param function_name Názov funkcie
     * @return Chybový kód
     */
    int gen_call(char *function_name);

    /**
     * @brief Príprava na definíciu funkcie (presun na vhodné miesto v zozname)
     */
    void prepare_for_func();

    /**
     * @brief Skončenie definície funkcie, vygenerovanie POPFRAME, RETURN, presun späť
     */
    void end_function();

    /**
     * @brief Vygenerovanie hlavičky funkcie (názov návestia + vytvorenie LF)
     *
     * @param function_name Názov funkcie
     * @return Chybový kód
     */
    int gen_function_header(char *function_name);

    // Vstavané funkcie

    /**
     * @brief Na základe identifikátora funkcie určí, ktorú vstavanú funkciu má vygenerovať
     * @param function_id Identifikátor funkcie
     */
    void generate_built_in_function(char *function_id);


    /**
     * @brief Generovanie vstavanej funkcie inputs
     *
     * @return Chybový kód
     */
    void gen_inputs();

    /**
     * @brief Generovanie vstavanej funkcie inputi
     *
     * @return Chybový kód
     */
    void gen_inputi();

    /**
     * @brief Generovanie vstavanej funkcie inputf
     *
     * @return Chybový kód
     */
    void gen_inputf();

    /**
     * @brief Generovanie vstavanej funkcie print
     *
     * @return Chybový kód
     */
    void gen_print();

    /**
     * @brief Generovanie vstavanej funkcie length
     *
     * @return Chybový kód
     */
    void gen_length();

    /**
     * @brief Generovanie vstavanej funkcie substr
     *
     * @return Chybový kód
     */
    void gen_substr();

    /**
     * @brief Generovanie vstavanej funkcie ord
     *
     * @return Chybový kód
     */
    void gen_ord();

    /**
     * @brief Generovanie vstavanej funkcie chr
     *
     * @return Chybový kód
     */
    void gen_chr();

    /**
     * @brief Vygenerovanie inštrukcie TYPE
     *
     * @param var1 Symbol prvého operandu
     * @param var2 Symbol druhého operandu
     * @return Chybový kód
     */
    int gen_type(char *var1, char *var2);

    /**
     * @brief Vygenerovanie skoku JUMPIFEQ
     *
     * @param label Návestie
     * @param var1 Premenná na porovnanie
     * @param var2_complete Hodnota na porovnanie
     */
    void gen_jumpifeq_general(char *label, char *var1, char *var2_complete);

    void automatic_conversion_generate(char *symbol1, char *symbol2, char **final_oper1, char **final_oper2);

    void generate_dynamic_division(char *symbol1, tDataType type1, char *symbol2, tDataType type2, char *finalVar);

#endif