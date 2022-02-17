/**
 * IFJ - Projekt Team 11
 *
 * @brief Súbor tabuľky symbolov
 *
 * @author Natália Holková (xholko02)
 */

#include "symtable.h"
#include "main.h"

// Funkcie pre zoznam parametrov

/** Inicializácia zoznamu parametrov **/
void ParamListInit(TParamList *L) {
    L->act = NULL;
    L->first = NULL;
}

/** Vloženie parametru na koniec zoznamu **/
void ParamListInsertLast(TParamList *L, char *id) {
    // Alokácia miesta
    TParamListItem *newItem = (TParamListItem *) malloc(sizeof(TParamListItem));
    if (newItem == NULL) return ;
    newItem->param_id = malloc(sizeof(char) * strlen(id));
    if (newItem->param_id == NULL) return ;
    strcpy(newItem->param_id, id); // Nastavenie id parametra
    newItem->next = NULL;

    // Pridanie na koniec zoznamu
    if (L->first == NULL) {
        L->first = newItem;
        L->last = newItem;
    }
    else {
        TParamListItem *tmp = L->last; // ukazuje na posledný prvok
        tmp->next = newItem;
        L->last = newItem;
    }
}

/** Nastavenie aktívneho prvku na prvý prvok **/
void ParamListFirst(TParamList *L) {
    if (L != NULL)
        L->act = L->first;
}


/** Nastavenie aktívneho prvku na nasledujúci prvok **/
void ParamListNext(TParamList *L) {
    if (L != NULL)
        if (L->act != NULL)
            L->act = L->act->next;
}

/** Vráti identifikátor aktívneho prvku **/
char *ParamListGetActive(TParamList *L) {
    if (L != NULL) {
        if (L->act != NULL)
            return L->act->param_id;
        else
            return NULL;
    }
    else {
        return NULL;
    }
}

/** Uvoľnenie zoznamu parametrov **/
void ParamListDispose(TParamList *L) {
    if (L == NULL) return ;
    TParamListItem *tmp = L->first;
    while (tmp != NULL) {
        TParamListItem *toDelete = tmp;
        tmp = tmp->next;

        free(toDelete->param_id);
        free(toDelete);
    }
    free(L);
}

// Funkcie pre globálnu TS

/** Inicializácia globálnej tabuľky symbolov **/
void global_table_init(tGlobalTableNodePtr *rootPtr) {
    (*rootPtr) = NULL;
}

/** Vloženie uzla (funkcie) do globálnej tabuľky symbolov **/
int global_table_insert(tGlobalTableNodePtr *rootPtr, char *id, tDataNodeGlobal *data) {
    if ((*rootPtr) == NULL) {
        // Vytvorenie nového uzlu
        tGlobalTableNodePtr newPtr = (tGlobalTableNodePtr) malloc(sizeof(struct tGlobalTableNode));
        if (newPtr != NULL) {

            newPtr->data = NULL;
            newPtr->id = NULL;
            newPtr->lPtr = NULL;
            newPtr->rPtr = NULL;

            // Podarilo sa alokovať miesto
            // ID
            newPtr->id = (char *) malloc(sizeof(char) * strlen(id)+END_OF_STRING);
            if (newPtr == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }
            strcpy(newPtr->id, id);

            // Alokuj miesto pre data
            newPtr->data = (tDataNodeGlobal *) malloc(sizeof(tDataNodeGlobal));
            if (newPtr->data == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }

            // Alokuj miesto pre paramList
            newPtr->data->paramList = (TParamList *) malloc(sizeof(TParamList));
            if (newPtr->data->paramList == NULL) {
                // Chyba
                return  ERR_INTERNAL;
            }

            // Uložiť obsah data
            newPtr->data->defined = data->defined;
            newPtr->data->function_table = data->function_table;
            newPtr->data->params = data->params;
            newPtr->data->paramList = data->paramList;
            newPtr->data->returnType = data->returnType;

            (*rootPtr) = newPtr;
        }
    }

    else {
        if (strcmp(id, (*rootPtr)->id) < 0 ) { // id < (*rootPtr)->id
            // Ideme vľavo
            global_table_insert( &(*rootPtr)->lPtr, id, data );
        }
        else if (strcmp(id, (*rootPtr)->id) > 0 ) { // id > (*rootPtr)->id
            // Ideme vpravo
            global_table_insert( &(*rootPtr)->rPtr, id, data );
        }
        else {
            // Prepíšeme staré dáta novými
            (*rootPtr)->data->defined = data->defined;
            (*rootPtr)->data->function_table = data->function_table;
            (*rootPtr)->data->params = data->params;
            (*rootPtr)->data->paramList = data->paramList;
            (*rootPtr)->data->returnType = data->returnType;
        }
    }

    return ERR_OK;
}

/** Vyhľadanie uzla (funkcie) v globálnej tabuľke symbolov podľa identifikátora **/
bool global_table_search(tGlobalTableNodePtr rootPtr, char *id, tDataNodeGlobal **data) {
    if (rootPtr == NULL) {
        return false;
    }
    else {
        if (strcmp(rootPtr->id,id) == 0) {
            *data = rootPtr->data;
            return true;
        }
        else {
            if (strcmp(rootPtr->id, id) > 0) {
                return (global_table_search( rootPtr->lPtr, id, data ));
            }
            else {
                return (global_table_search( rootPtr->rPtr, id, data ));
            }
        }
    }
}

/** Nahradenie uzla najpravejším **/
void global_table_replace_by_rightmost(tGlobalTableNodePtr ptrReplaced, tGlobalTableNodePtr *rootPtr) {
    if ((*rootPtr) == NULL) {
        return ;
    }

    if ((*rootPtr)->rPtr == NULL) {
        // Nie je už pravý podstrom - nahraď

        // Skopíruj obsah

        // Skopírovanie id
        ptrReplaced->id = realloc(ptrReplaced->id, sizeof(char) * strlen( (*rootPtr)->id) ); // reallocovaie miesta
        strcpy(ptrReplaced->id, (*rootPtr)->id);

        free(ptrReplaced->data->paramList); // Free, lebo po priradení by sa stratila tá pamäť a nastal by leak

        ptrReplaced->data = (*rootPtr)->data;

        tGlobalTableNodePtr temp = (*rootPtr);
        (*rootPtr) = (*rootPtr)->lPtr;

        temp->data->paramList = NULL;
        free(temp->data);
        free(temp);
    }
    else {
        global_table_replace_by_rightmost(ptrReplaced, &(*rootPtr)->rPtr);
    }
}

/** Vymazanie uzla z globálnej tabuľky symbolov podľa identifikátora **/
void global_table_delete(tGlobalTableNodePtr *rootPtr, char *id) {
    if ((*rootPtr) == NULL) {
        // Nie je čo vymazať
        return;
    }

    if (strcmp(id, (*rootPtr)->id) < 0) {
        // Treba ís doľava
        global_table_delete(&(*rootPtr)->lPtr, id);
    }
    else if (strcmp(id, (*rootPtr)->id) > 0) {
        // Treba ísť doprava
        global_table_delete(&(*rootPtr)->rPtr, id);
    }
    else {
        // Treba vymazať tento uzol

        // Skontrolovať potomkov

        if ((*rootPtr)->rPtr == NULL) {
            // Nemá pravý podstrom
            tGlobalTableNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->lPtr;

            // Vymazanie uzlu
            ParamListDispose(temp->data->paramList);
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else if ((*rootPtr)->lPtr == NULL) {
            // Nemá ľavý podstrom
            tGlobalTableNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->rPtr;

            // Vymazanie uzlu
            ParamListDispose(temp->data->paramList);
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else {
            // Má dvoch potomkov
            global_table_replace_by_rightmost((*rootPtr), &(*rootPtr)->lPtr);
        }
    }
}

/** Vymazanie celej globálnej tabuľky symbolov **/
void global_table_dispose(tGlobalTableNodePtr *rootPtr) {
    if ((*rootPtr) != NULL) {
        global_table_dispose( &(*rootPtr)->lPtr );
        global_table_dispose( &(*rootPtr)->rPtr );

        // Uvoľni ParamList
        ParamListDispose((*rootPtr)->data->paramList);

        // Uvoľni dáta
        free((*rootPtr)->data);

        // Uvoľni id
        free((*rootPtr)->id);

        free(*rootPtr);
        (*rootPtr) = NULL;
    }
}

/** Pomocná funkcia na výpis binárneho stromu **/
void global_table_print_tmp(tGlobalTableNodePtr TempTree, char* sufix, char fromdir) {
    if (TempTree != NULL)
    {
        char* suf2 = (char*) malloc(strlen(sufix) + 4);
        strcpy(suf2, sufix);
        if (fromdir == 'L')
        {
            suf2 = strcat(suf2, "  |");
            printf("%s\n", suf2);
        }
        else
            suf2 = strcat(suf2, "   ");
        global_table_print_tmp(TempTree->rPtr, suf2, 'R');
        printf("%s  +-[%s]\n", sufix, TempTree->id);
        strcpy(suf2, sufix);
        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");
        global_table_print_tmp(TempTree->lPtr, suf2, 'L');
        if (fromdir == 'R') printf("%s\n", suf2);
        free(suf2);
    }
}

/** Výpis štruktúry binárneho stromu **/
void global_table_print(tGlobalTableNodePtr TempTree) {
    printf("Struktura binarniho stromu:\n");
    printf("\n");
    if (TempTree != NULL)
        global_table_print_tmp(TempTree, "", 'X');
    else
        printf("strom je prazdny\n");
    printf("\n");
    printf("=================================================\n");
}

// Špeciálne

/** Pridá identifikátor parametra do zoznamu parametrov funkcie **/
void function_add_param_id_to_list(tGlobalTableNodePtr rootPtr, char *function_id, char *param_id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        ParamListInsertLast(data->paramList, param_id);
    }
}

/** Nastavenie prvého prvku v zozname parametrov funkcie na aktívny **/
void function_param_list_set_first_active(tGlobalTableNodePtr rootPtr, char *function_id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        ParamListFirst(data->paramList);
    }
}

/** Nastavenie aktívneho prvku v zozname parametrov funkcie na nasledujúci **/
void function_param_list_next(tGlobalTableNodePtr rootPtr, char *function_id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        ParamListNext(data->paramList);
    }
}

/** Vrátenie názvu aktívneho parametru funkcie **/
char *function_param_list_get_active(tGlobalTableNodePtr rootPtr, char *function_id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        return ParamListGetActive(data->paramList);
    }
    else
        return NULL;
}

/** Funkcia na vrátenie počtu parametrov funkcie **/
int function_get_number_params(tGlobalTableNodePtr rootPtr, char *function_id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        return data->params;
    }
    else {
        return -1;
    }
}

/** Zvýšenie počtu parametrov funkcie o 1 **/
void function_increase_number_params(tGlobalTableNodePtr rootPtr, char *function_id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        data->params = data->params + 1;
    }
}

/** Nastavenie počtu parametrov funkcie na určitú hodnotu **/
void function_set_number_params(tGlobalTableNodePtr rootPtr, char *function_id, int number) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        data->params = number;
    }
}

/** Nastavanie funkcie ako definovanej **/
int function_set_defined(tGlobalTableNodePtr *rootPtr, char *id) {
    tDataNodeGlobal *data;
    if ((*rootPtr) == NULL) {
        // úplne prázdny

        // Vytvor nový uzol v tabulke vlastne
        data = (tDataNodeGlobal *) malloc(sizeof(tDataNodeGlobal));
        if (data == NULL) {
            // chyba
            return ERR_INTERNAL;
        }
        data->defined = true;
        data->function_table = NULL;
        data->params = 0;
        data->returnType = T_NIL;
        // Alokovanie miesta pre data->paramList
        data->paramList = malloc(sizeof(TParamList));
        if (data->paramList == NULL) return ERR_INTERNAL;
        ParamListInit(data->paramList); // Inicializácia zoznamu parametrov

        if (global_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
            // Chyba
            return ERR_INTERNAL;
        }

        free(data);
    }
    else {
        bool found = global_table_search((*rootPtr), id, &data);
        if (!found) {
            // Premenná neexistuje zatiaľ

            // Vytvor nový uzol v tabulke vlastne
            data = (tDataNodeGlobal *) malloc(sizeof(tDataNodeGlobal));
            if (data == NULL) {
                // chyba
                return ERR_INTERNAL;
            }
            data->defined = true;
            data->function_table = NULL;
            data->params = 0;
            data->returnType = T_NIL;
            // Alokovanie miesta pre data->paramList
            data->paramList = malloc(sizeof(TParamList));
            if (data->paramList == NULL) return ERR_INTERNAL;
            ParamListInit(data->paramList); // Inicializácia zoznamu parametrov

            if (global_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
                // Chyba
                return ERR_INTERNAL;
            }

            free(data);
        }
        else {
            // Premmenná existuje - nastav defined
            data->defined = true;
        }
    }

    return ERR_OK;
}

/**  Získanie uzla s funkciou podľa identifikátora **/
tGlobalTableNodePtr get_function_node(tGlobalTableNodePtr rootPtr, char *id) {
    if (rootPtr == NULL) {
        return NULL;
    }
    else {
        if(id != NULL) {
            if (strcmp(rootPtr->id, id) == 0) {
                return rootPtr;
            } else {
                if (strcmp(rootPtr->id, id) > 0) {
                    return (get_function_node(rootPtr->lPtr, id));
                } else {
                    return (get_function_node(rootPtr->rPtr, id));
                }
            }
        }
    }

    return NULL;
}

/** Nastavanie odkazu na lokálnu tabuľku symbolov funkcie **/
void set_function_table(tGlobalTableNodePtr *function_node_ptr, tLocalTableNodePtr *local_table_ptr) {
    (*function_node_ptr)->data->function_table = local_table_ptr;
}

/** Nastaví návratový typ funkcie **/
void set_fuction_return_type(tGlobalTableNodePtr rootPtr, char *id, tDataType returning_type) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, id, &data);
    if (found) {
        data->returnType = returning_type;
    }
}


/** Vráti typ návratovej hodnoty funkcie **/
tDataType get_fuction_return_type(tGlobalTableNodePtr rootPtr, char *id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, id, &data);
    if (found) {
        return data->returnType;
    }
    else return T_UNDEFINED;
}


// Funkcie pre lokálnu TS

/** Inicializácia lokálnej tabuľky symbolov **/
void local_table_init(tLocalTableNodePtr *rootPtr) {
    (*rootPtr) = NULL;
}

/** Vloženie uzla (premennej) do lokálnej tabuľky symbolov **/
int local_table_insert(tLocalTableNodePtr *rootPtr, char *id, tDataNodeLocal *data) {
    if ((*rootPtr) == NULL) {
        // Vytvorenie nového uzlu
        tLocalTableNodePtr newPtr = (tLocalTableNodePtr) malloc(sizeof(struct tLocalTableNode));

        if (newPtr != NULL) {

            newPtr->data = NULL;
            newPtr->id = NULL;
            newPtr->lPtr = NULL;
            newPtr->rPtr = NULL;

            // Podarilo sa alokovať miesto
            // ID
            newPtr->id = (char *) malloc(sizeof(char) * strlen(id)+END_OF_STRING);
            if (newPtr == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }
            strcpy(newPtr->id, id);

            // Alokuj miesto pre data
            newPtr->data = (tDataNodeLocal *) malloc(sizeof(tDataNodeGlobal));
            if (newPtr->data == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }

            // Uložiť obsah data
            newPtr->data->defined = data->defined;
            newPtr->data->type = data->type;

            (*rootPtr) = newPtr;
        }
    }

    else {
        if (strcmp(id, (*rootPtr)->id) < 0 ) { // id < (*rootPtr)->id
            // Ideme vľavo
            local_table_insert( &(*rootPtr)->lPtr, id, data );
        }
        else if (strcmp(id, (*rootPtr)->id) > 0 ) { // id > (*rootPtr)->id
            // Ideme vpravo
            local_table_insert( &(*rootPtr)->rPtr, id, data );
        }
        else {
            // Prepíšeme staré dáta novými
            (*rootPtr)->data->defined = data->defined;
            (*rootPtr)->data->type = data->type;
        }
    }

    return ERR_OK;
}

/** Vyhľadanie premennej v lokálnej tabuľke symbolov podľa identifikátora **/
bool local_table_search(tLocalTableNodePtr rootPtr, char *id, tDataNodeLocal **data) {
    if (rootPtr == NULL) {
        return false;
    }
    else {
        if (strcmp(rootPtr->id,id) == 0) {
            *data = rootPtr->data;
            return true;
        }
        else {
            if (strcmp(rootPtr->id, id) > 0) {
                return (local_table_search( rootPtr->lPtr, id, data ));
            }
            else {
                return (local_table_search( rootPtr->rPtr, id, data ));
            }
        }
    }
}

/** Nahradenie uzla najpravejším **/
void local_table_replace_by_rightmost(tLocalTableNodePtr ptrReplaced, tLocalTableNodePtr *rootPtr) {
    if ((*rootPtr) == NULL) {
        return ;
    }

    if ((*rootPtr)->rPtr == NULL) {
        // Nie je už pravý podstrom - nahraď

        // Skopíruj obsah

        // Skopírovanie id
        ptrReplaced->id = realloc(ptrReplaced->id, sizeof(char) * strlen( (*rootPtr)->id) ); // reallocovaie miesta
        strcpy(ptrReplaced->id, (*rootPtr)->id);

        ptrReplaced->data = (*rootPtr)->data;

        tLocalTableNodePtr temp = (*rootPtr);
        (*rootPtr) = (*rootPtr)->lPtr;

        free(temp->data);
        free(temp);
    }
    else {
        local_table_replace_by_rightmost(ptrReplaced, &(*rootPtr)->rPtr);
    }
}

/** Vymazanie premennej z lokálnej tabuľky symbolov podľa identifikátora **/
void local_table_delete(tLocalTableNodePtr *rootPtr, char *id) {
    if ((*rootPtr) == NULL) {
        // Nie je čo vymazať
        return;
    }

    if (strcmp(id, (*rootPtr)->id) < 0) {
        // Treba ís doľava
        local_table_delete(&(*rootPtr)->lPtr, id);
    }
    else if (strcmp(id, (*rootPtr)->id) > 0) {
        // Treba ísť doprava
        local_table_delete(&(*rootPtr)->rPtr, id);
    }
    else {
        // Treba vymazať tento uzol

        // Skontrolovať potomkov

        if ((*rootPtr)->rPtr == NULL) {
            // Nemá pravý podstrom
            tLocalTableNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->lPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else if ((*rootPtr)->lPtr == NULL) {
            // Nemá ľavý podstrom
            tLocalTableNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->rPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else {
            // Má dvoch potomkov
            local_table_replace_by_rightmost((*rootPtr), &(*rootPtr)->lPtr);
        }
    }
}

/** Vymazanie celej lokálnej tabuľky symbolov **/
void local_table_dispose(tLocalTableNodePtr *rootPtr) {
    if ((*rootPtr) != NULL) {
        local_table_dispose( &(*rootPtr)->lPtr );
        local_table_dispose( &(*rootPtr)->rPtr );

        // Uvoľni dáta
        free((*rootPtr)->data);

        // Uvoľni id
        free((*rootPtr)->id);

        free(*rootPtr);
        (*rootPtr) = NULL;
    }
}

/** Pomocná funkcia na vypísanie lokálnej tabuľky symbolov **/
void local_table_print_tmp(tLocalTableNodePtr TempTree, char* sufix, char fromdir) {
    if (TempTree != NULL)
    {
        char* suf2 = (char*) malloc(strlen(sufix) + 4);
        strcpy(suf2, sufix);
        if (fromdir == 'L')
        {
            suf2 = strcat(suf2, "  |");
            printf("%s\n", suf2);
        }
        else
            suf2 = strcat(suf2, "   ");
        local_table_print_tmp(TempTree->rPtr, suf2, 'R');
        printf("%s  +-[%s]\n", sufix, TempTree->id);
        strcpy(suf2, sufix);
        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");
        local_table_print_tmp(TempTree->lPtr, suf2, 'L');
        if (fromdir == 'R') printf("%s\n", suf2);
        free(suf2);
    }
}

/** Výpis lokálnej tabuľky symbolov **/
void local_table_print(tLocalTableNodePtr TempTree) {
    printf("Struktura binarniho stromu:\n");
    printf("\n");
    if (TempTree != NULL)
        local_table_print_tmp(TempTree, "", 'X');
    else
        printf("strom je prazdny\n");
    printf("\n");
    printf("=================================================\n");
}

// Špeciálne

/** Nastanie premennej na defined **/
int variable_set_defined(tLocalTableNodePtr *rootPtr, char *id) {
    tDataNodeLocal *data;
    if ((*rootPtr) == NULL) {
        // úplne prázdny

        // Vytvor nový uzol v tabulke vlastne
        data = (tDataNodeLocal *) malloc(sizeof(tDataNodeLocal));
        if (data == NULL) {
            // chyba
            return ERR_INTERNAL;
        }
        data->defined = true;
        data->type = T_UNDEFINED;

        if (local_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
            // Chyba
            return ERR_INTERNAL;
        }

    }
    else {
        bool found = local_table_search((*rootPtr), id, &data);
        if (!found) {
            // Premenná neexistuje zatiaľ

            // Vytvor nový uzol v tabulke vlastne
            data = (tDataNodeLocal *) malloc(sizeof(tDataNodeLocal));
            if (data == NULL) {
                // chyba
                return ERR_INTERNAL;
            }
            data->defined = true;
            data->type = T_UNDEFINED;

            if (local_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
                // Chyba
                return ERR_INTERNAL;
            }

        }
        else {
            // Premmenná existuje - nastav defined
            data->defined = true;
        }
    }

    return ERR_OK;
}

/** Nastavenie datového typu premennej **/
void variable_set_type(tLocalTableNodePtr function_table, char *id, tDataType type) {
    tDataNodeLocal *data;
    bool found = local_table_search(function_table, id, &data);
    if (!found) {
        // Nenájdená funkcia
        return ;
    }
    else {
        // funkcia nájdená - vráť ukazovateĺ na ňu
        data->type = type;
    }
}

/** Vrátenie typu premennej **/
tDataType variable_get_type(tLocalTableNodePtr function_table, char *id) {
    // Predpokladá, že existuje premenná
    tDataNodeLocal *data;
    local_table_search(function_table, id, &data);
    return data->type;
}

/** Vrátenie celého uzla s premennou **/
tLocalTableNodePtr get_variable_node(tLocalTableNodePtr rootPtr, char *id) {
    if (rootPtr == NULL) {
        return NULL;
    }
    else {
        if (strcmp(rootPtr->id,id) == 0) {
            return rootPtr;
        }
        else {
            if (strcmp(rootPtr->id, id) > 0) {
                return (get_variable_node( rootPtr->lPtr, id));
            }
            else {
                return (get_variable_node( rootPtr->rPtr, id));
            }
        }
    }
}

/** Vymazanie všetkých lokálnych tabuliek funkcií v globálnej tabuľke symbolov **/
void symbol_table_dispose_all_local(tGlobalTableNodePtr *global_root) {
    if ((*global_root) != NULL) {
        symbol_table_dispose_all_local( &(*global_root)->lPtr );
        symbol_table_dispose_all_local( &(*global_root)->rPtr );

        tLocalTableNodePtr *local_temp = (*global_root)->data->function_table;
        if (local_temp != NULL)
            local_table_dispose(local_temp);
    }
}
