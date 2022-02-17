/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavičkový súbor pre main
 * @file main.h
 *
 * Na tomto projekte sa podielali:
 * @author Natália Holková (xholko02)
 * @author Matej Novák (xnovak2f)
 * @author Filip Bali (xbalif00)
 * @author Albert Szöllösi (xszoll02)
 */

#ifndef  _MAIN_H
    #define _MAIN_H

    #include <stdio.h>
    #include "scanner.h"
    #include "parser.h"
    #include "symtable.h"
    #include "semantic_analysis.h"
    #include "stringlib.h"
    #include "expression_parser.h"
    #include "code_gen.h"


    #define END_OF_STRING 1 //Znamena '\0'
    #define MAIN "main@function"

    tGlobalTableNodePtr global_table; // Globálna tabuľka symbolov, kde sú v jednotlivých uzloch uložené užívateľom definované funkcie podľa ich ID
    tLocalTableNodePtr * actual_function_ptr; // Tabuľka symbolov aktuálnej funkcie, v ktorej sa program nachádza (buď nejaká užívateľom definovaná funkcia alebo "main")
    char *actual_function_name; // ID funkcie, v ktorej sa práve program nachádza
    char *id_copy; // Reťazec na zálohu ID aj po načítaní ďalšich tokenov
    char *func_id_copy; // Retazec na zalohu ID funkcie ktora sa priraduje do premennej
    int is_in_while; // Kontrola, či je aktuálne vo while - kvôli prípadným deklaráciám premenných

#endif
