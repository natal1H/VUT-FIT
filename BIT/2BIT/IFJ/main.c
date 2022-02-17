/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavný súbor
 * @file main.c
 *
 * Na tomto projekte sa podielali:
 * @author Natália Holková (xholko02)
 * @author Matej Novák (xnovak2f)
 * @author Filip Bali (xbalif00)
 * @author Albert Szöllösi (xszoll02)
 */


#include "main.h"

int main() {

//Odkomentovat ak sa projekt spusta v CLion
//--------------CLion-------------------
//Subor input.txt sa musi nachadzat v cmake-build-debug v danej zlozke projektu
//Cize cesta input.txt bude ak je projekt ulozeny v default zlozke CLionProjects:
//   ../CLionProjects/<Nazov projektu>/cmake-build-debug/input.txt
freopen("input.txt","r",stdin);
//--------------------------------------

    // inicializuj scanner najprv cez scanner_initialize()
    if ( scanner_initialize() != ERR_OK ) {
        // chyba pri inicializácii
        return ERR_INTERNAL;
    }

    expression = false;

    Token *token =  token_initialize();
    int ret = get_next_token(token);

    // Inicializácia globálnej tabuľky symbolov (GTS)
    global_table_init(&global_table);

    // Vytvorenie tabuľky pre main
    function_set_defined(&global_table, MAIN); // Pridá uzol MAIN do GTS
    tGlobalTableNodePtr main_global_node = get_function_node(global_table, MAIN); // Vráti ukazovvateľ na uzol s MAIN v GTS
    // Vytvorenie novej lokálnej tabuľky
    tLocalTableNodePtr new_local_table;
    local_table_init(&new_local_table); // Inicializácia novej lokálnej tabuľky
    set_function_table(&main_global_node, &new_local_table); // Naviazanie uzla v globálnej na novú lokálnu
    actual_function_ptr = (main_global_node->data->function_table); // Dôležité: nastavenie ukazovateľ na aktuálnu lokálnu TS
    actual_function_name = MAIN; // Nastaviť actual_function_name na MAIN

    id_copy = NULL; // Netreba zatiaľ nijaké id zálohovať
    func_id_copy = NULL;

    // Inicializácia lokálnej tabuľky s návestiami
    local_table_init(&label_table);

    // Príprava generovania kódu
    code_gen_start();


    if (ret != ERR_SCANNER) {

        ret = prog(token);
        //printf("Výsledok: %d\n", ret); // Výpis iba pri testovaní
    }

    // Upratanie po programe

    token_free(token);

    // po skončení práce uvoľni miesto po read_string
    tstring_free_struct(read_string);

    // Postupné uvoľnenie všetkých lokálnych tabuliek symbolov

    // Uvoľnenie GTS
    global_table_dispose(&global_table);

    // uvoľnenie id_copy
    free(id_copy);

    // Uvoľnenie label_table
    local_table_dispose(&label_table);

    // Výpis inštrukcií
    list_print_instructions(&instr_list);

    // Upratanie po generovaní kódu
    code_gen_end();

    return ret;
}
