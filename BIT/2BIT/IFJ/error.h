/**
 * IFJ - Projekt Team 11
 *
 * @brief Chybové kódy, ktoré môže vracať program
 *
 */


#ifndef _ERROR_H

    #define _ERROR_H

    #define ERR_OK          0 // Nenastala chyba
    #define ERR_SCANNER     1 // Chyba v rámci lexikálnej analýzy
    #define ERR_SYNTAX      2 // Chyba v rámci syntaktickej analýzy
    #define ERR_SEM_UNDEF   3 // Sémantická chyba v programe - nedefinovaná funkcia/premenná,...
    #define ERR_SEM_TYPE    4 // Sémantická/behová chyba typovej kompatibility
    #define ERR_SEM_PARAM   4 // Sémantická chyba v programe - nesprávny počet parametrov pri volaní funkcie
    #define ERR_SEM_ELSE    6 // Ostatné sémantické chyby
    #define ERR_ZERO_DIV    9 // Behová chyba delenia 0
    #define ERR_INTERNAL    99 // Interná chyba prekladača

#endif