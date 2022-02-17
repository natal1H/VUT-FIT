/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavičkový súbor pre prácu s dynamickým reťazcom
 * @file stringlib.h
 *
 * @author Natália Holková (xholko02)
 */

#ifndef _STRINGLIB_H

    #define _STRINGLIB_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include "error.h"

    #define TSTRING_INITIAL_SIZE 16 // Dĺžka reťazca na začiatku a zároveň prírastok pri rozširovaní

    /** @struct Dynamický reťazec TString - v prípade potreby sa môže rozširovať **/
    typedef struct tstring {
        int allocated; // Aktuálne alokované miesto
        int length; // Aktuálna dĺžka reťazca
        char *string; // Samotný reťazec
    } TString;

    /**
     * @brief Inicializácia reťazca v štruktúre TString
     *
     * @param tstr Ukazovateľ na dynamický reťazec
     * @return Chybový kód
     */
    int tstring_intialize(TString *tstr);

    /**
     * @brief Inicializácia štruktúry TString
     *
     * @return Ukazovateľ na inicializovanú štruktúru
     */
    TString *tstring_struct_initialize();

    /**
     * @brief Zvýšenie max dĺžky reťazca
     *
     * @param tstr Ukazovateľ na dynamický reťazec
     * @return Chybový kód
     */
    int tstring_increase_size(TString *tstr);

    /**
     * @brief Pridanie znaku do reťazca
     * @param tstr Ukazovateľ na dynamický reťazec
     * @param c Znak na pridanie
     * @return Chybový kód
     */
    int tstring_append_char(TString *tstr, char c);

    /**
     * @brief Vyčistenie reťazca v TString
     * @param tstr Ukazovateľ na dynamický string
     * @return Chybový kód
     */
    int tstring_clear_string(TString *tstr);

    /**
     * @brief Uvoľnenie reťazca v TString
     *
     * @param tstr Ukazovateľ na dynamický string
     */
    void tstring_free_string(TString *tstr);

    /**
     * @brief Uvoľnenie štruktúry TString
     * @param tstr Ukazovateľ na TString
     */
    void tstring_free_struct(TString *tstr);

    /**
     * @brief Vloženie reťazca do dynamického reťazca
     *
     * @param tstr Ukazovateľ na TString
     * @param line Reťazec na vloženie
     * @return Chybový kód
     */
    int tstring_add_line(TString *tstr, char *line);

#endif