/**
 * IFJ Projekt - Team 11
 *
 * @brief Súbor pre prácu so inštrukciami a zoznamom inštrukcií
 * @file instr_list.c
 *
 * @author Natália Holková (xholko02)
 */

// Použitie zoznamu na uloženie inštrukcií bolo inšpirované z http://www.fit.vutbr.cz/study/courses/IFJ/public/project/ z ukážky jednoduchého interpretu

#include "instr_list.h"
#include "error.h"
#include "main.h"

#include <stdio.h>

/** Inicializácia zoznamu **/
void listInit(tListOfInstr *L) {
    // Nastavenie všetkých ukazovateľov na NULL
    L->first  = NULL;
    L->last   = NULL;
    L->active = NULL;
}

/** Uvoľnenie zoznamu inštrukcií **/
void listFree(tListOfInstr *L) {
    if(L != NULL && L->first != NULL) {
        tListItem *ptr = NULL;

        // Postupný prechod zoznamom až na koniec
        while (L->first != NULL) {
            ptr = L->first;
            L->first = L->first->nextItem;

            // Uvolníme inštrukciu v ptr
            tInst_free_instruction(ptr->Instruction);

            // Uvolnime celú položku
            free(ptr);
        }
    }
}

/** Vloženie inštrukcie do zoznamu za aktívny prvok **/
void listInsertPostActive(tListOfInstr *L, tInstr *I) {
    // Vytvorenie nového prvku
    tListItem *newItem = NULL;
    newItem = malloc(sizeof (tListItem));
    newItem->Instruction = NULL;
    newItem->nextItem = NULL;

    newItem->Instruction = tInstr_create(I->instType, I->addr1, I->addr2, I->addr3); // Vytvorenie novej inštrukcie podľa vzoru parametre
    if (newItem->Instruction == NULL) {
        // Chyba
        return ;
    }

    if (L->active == NULL) {
        // Prázdny zoznam ešte, tak nech pridá na začiatok
        L->first = newItem;
        L->first->nextItem = NULL;
        L->last = L->first;
        listFirst(L);
    }
    else if (L->active == L->last) {
        // Vložiť za poslednú
        L->last->nextItem=newItem;
        L->last = newItem;
        listNext(L);
    }
    else {
        // Vložiť niekde do stredu zoznamu
        newItem->nextItem = L->active->nextItem;
        L->active->nextItem = newItem;
        listNext(L);
    }
}

/** Nastavenie aktívneho prvku na prvý prvok **/
void listFirst(tListOfInstr *L) {
    L->active = L->first;
}

/** Nastavenie aktívneho prvku na nasledujúci prvok **/
void listNext(tListOfInstr *L) {
    if (L->active != NULL)
        L->active = L->active->nextItem;
}

/** Nastavenie aktívneho prvku na posledný prvok **/
void listLast(tListOfInstr *L) {
    L->active = L->last;
}

/** Inicializácia inštrukcie **/
tInstr *tInstr_init() {
    // Alokovanie miesta
    tInstr *i = (tInstr *) malloc(sizeof(tInstr));
    if (i == NULL) {
        return NULL;
    }

    // Nastavenie typu
    i->instType = I_UNDEFINED;

    // Nastavenie počiatočných adries na NULL
    i->addr1 = NULL;
    i->addr2 = NULL;
    i->addr3 = NULL;

    return i;
}

/** Vytvorenie inštrukcie **/
tInstr *tInstr_create(tInstruction_type type, char *addr1, char *addr2, char *addr3) {

    // Alokovanie miesta
    tInstr *i = (tInstr *) malloc(sizeof(tInstr));
    if (i == NULL) {
        return NULL;
    }

    // Nastavenie typu
    i->instType = type;

    // Nastavenie addr1
    if (addr1 != NULL) {
        i->addr1 = malloc(sizeof(char) * strlen(addr1)+END_OF_STRING);
        if (i->addr1 == NULL) {
            // Chyba
            return NULL;
        }
        strcpy(i->addr1, addr1);
    }
    else
        i->addr1 = NULL;

    // Nastavenie addr2
    if (addr2 != NULL) {
        i->addr2 = malloc(sizeof(char) * strlen(addr2)+END_OF_STRING);
        if (i->addr2 == NULL) {
            // Chyba
            return NULL;
        }
        strcpy(i->addr2, addr2);
    }
    else
        i->addr2 = NULL;

    // Nastavenie addr3
    if (addr3 != NULL) {
        i->addr3 = malloc(sizeof(char) * strlen(addr3)+END_OF_STRING);
        if (i->addr3 == NULL) {
            // Chyba
            return NULL;
        }
        strcpy(i->addr3, addr3);
    }
    else
        i->addr3 = NULL;

    return i;
}

/** Vrátenie ukazovateľa na aktívny prvok **/
tListItem *listGetActivePtr(tListOfInstr *L) {
    if (L != NULL) { // Neprázdny zoznam
        return L->active;
    }
    else
        return NULL; // Prázdny zoznam
}

/** Výpis samotnej inštrukcie **/
void tInstr_print_single_instruction(tInstr *I) {
    if (I == NULL) {
        return; // Chyba
    }

    if (I->instType == I_HEADER) {
        // Header má trochu iný výpis, preto je samostatne
        printf(".IFJcode18");
        return ;
    }
    else if (I->instType == I_COMENT) {
        // Jednoduchší výpis komentára takto
        printf("\n# %s", I->addr1);
        return ;
    }
    else {
        printf("\n");
    }

    // Vypíš opcode
    switch (I->instType) {
        // Kvôli odstráneniu warnings pri preklade
        case I_UNDEFINED:
        case I_HEADER:
        case I_COMENT:
            break;

        case I_MOVE:
            printf("%s", "MOVE");
            break;
        case I_CREATEFRAME:
            printf("%s", "CREATEFRAME");
            break;
        case I_PUSHFRAME:
            printf("%s", "PUSHFRAME");
            break;
        case I_POPFRAME:
            printf("%s", "POPFRAME");
            break;
        case I_DEFVAR:
            printf("%s", "DEFVAR");
            break;
        case I_CALL:
            printf("%s", "CALL");
            break;
        case I_RETURN:
            printf("%s", "RETURN");
            break;
        case I_PUSHS:
            printf("%s", "PUSHS");
            break;
        case I_POPS:
            printf("%s", "POPS");
            break;
        case I_CLEARS:
            printf("%s", "CLEARS");
            break;
        case I_ADD:
            printf("%s", "ADD");
            break;
        case I_SUB:
            printf("%s", "SUB");
            break;
        case I_MUL:
            printf("%s", "MUL");
            break;
        case I_DIV:
            printf("%s", "DIV");
            break;
        case I_IDIV:
            printf("%s", "IDIV");
            break;
        case I_ADDS:
            printf("%s", "ADDS");
            break;
        case I_SUBS:
            printf("%s", "SUBS");
            break;
        case I_MULS:
            printf("%s", "MULS");
            break;
        case I_DIVS:
            printf("%s", "DIVS");
            break;
        case I_IDIVS:
            printf("%s", "IDIVS");
            break;
        case I_LT:
            printf("%s", "LT");
            break;
        case I_GT:
            printf("%s", "GT");
            break;
        case I_EQ:
            printf("%s", "EQ");
            break;
        case I_LTS:
            printf("%s", "LTS");
            break;
        case I_GTS:
            printf("%s", "GTS");
            break;
        case I_EQS:
            printf("%s", "EQS");
            break;
        case I_AND:
            printf("%s", "AND");
            break;
        case I_OR:
            printf("%s", "OR");
            break;
        case I_NOT:
            printf("%s", "NOT");
            break;
        case I_ANDS:
            printf("%s", "ANDS");
            break;
        case I_ORS:
            printf("%s", "ORS");
            break;
        case I_NOTS:
            printf("%s", "NOTS");
            break;
        case I_INT2FLOAT:
            printf("%s", "INT2FLOAT");
            break;
        case I_FLOAT2INT:
            printf("%s", "FLOAT2INT");
            break;
        case I_INT2CHAR:
            printf("%s", "INT2CHAR");
            break;
        case I_STRI2INT:
            printf("%s", "STRI2INT");
            break;
        case I_INT2FLOATS:
            printf("%s", "INT2FLOATS");
            break;
        case I_FLOAT2INTS:
            printf("%s", "FLOAT2INTS");
            break;
        case I_INT2CHARS:
            printf("%s", "INT2CHARS");
            break;
        case I_STRI2INTS:
            printf("%s", "STRI2INTS");
            break;
        case I_READ:
            printf("%s", "READ");
            break;
        case I_WRITE:
            printf("%s", "WRITE");
            break;
        case I_CONCAT:
            printf("%s", "CONCAT");
            break;
        case I_STRLEN:
            printf("%s", "STRLEN");
            break;
        case I_GETCHAR:
            printf("%s", "GETCHAR");
            break;
        case I_SETCHAR:
            printf("%s", "SETCHAR");
            break;
        case I_TYPE:
            printf("%s", "TYPE");
            break;
        case I_LABEL:
            printf("%s", "LABEL");
            break;
        case I_JUMP:
            printf("%s", "JUMP");
            break;
        case I_JUMPIFEQ:
            printf("%s", "JUMPIFEQ");
            break;
        case I_JUMPIFNEQ:
            printf("%s", "JUMPIFNEQ");
            break;
        case I_JUMPIFEQS:
            printf("%s", "JUMPIFEQS");
            break;
        case I_JUMPIFNEQS:
            printf("%s", "JUMPIFNEQS");
            break;
        case I_BREAK:
            printf("%s", "BREAK");
            break;
        case I_DPRINT:
            printf("%s", "DPRINT");
            break;
    }

    // Vypíš addr1
    if (I->addr1 != NULL) {
        printf(" %s", I->addr1);
    }

    // Vypíš addr2
    if (I->addr2 != NULL) {
        printf(" %s", I->addr2);
    }

    // Vypíš addr3
    if (I->addr3 != NULL) {
        printf(" %s", I->addr3);
    }

}

/** Výpis všetkých inštrukcií v zozname **/
void list_print_instructions(tListOfInstr *L) {
    if (L == NULL) {
        return ;
    }

    tListItem *temp = L->first;
    while (temp != NULL) {
        // Postupný výpis inštrukcií
        tInstr_print_single_instruction(temp->Instruction);
        temp = temp->nextItem;
    }
}

/** Uvoľnenie inštrukcie **/
void tInst_free_instruction(tInstr *I) {
    if (I != NULL) {
        free(I->addr1);
        free(I->addr2);
        free(I->addr3);
        free(I);
    }
}

/** Nastavenie inštrukcie **/
int tInstr_set_instruction(tInstr *instr, tInstruction_type type, char *addr1, char *addr2, char *addr3) {
    if (instr == NULL) {
        return ERR_INTERNAL;
    }

    // Nastavenie nového typu
    instr->instType = type;

    // Nastavenie addr1
    if (addr1 != NULL) {
        // Pozrieť či predtým niečo bolo na addr1
        if (instr->addr1 != NULL) {
            // uvoľniť a znovu alokovať
            free(instr->addr1);
        }
        instr->addr1 = malloc(sizeof(char) * strlen(addr1)+END_OF_STRING);
        if (instr->addr1 == NULL) {
            // Chyba - nepodarilo sa alokovať
            return ERR_INTERNAL;
        }
        strcpy(instr->addr1, addr1);
    }
    else {
        if (instr->addr1 != NULL) {
            free(instr->addr1);
        }
        instr->addr1 = NULL;
    }
    // Nastavenie addr2
    if (addr2 != NULL) {
        // Pozrieť či predtým niečo bolo na addr2
        if (instr->addr2 != NULL) {
            // uvoľniť a znovu alokovať
            free(instr->addr2);
        }
        instr->addr2 = malloc(sizeof(char) * strlen(addr2)+END_OF_STRING);
        if (instr->addr2 == NULL) {
            // Chyba - nepodarilo sa alokovať
            return ERR_INTERNAL;
        }
        strcpy(instr->addr2, addr2);
    }
    else {
        if (instr->addr2 != NULL) {
            free(instr->addr2);
        }
        instr->addr2 = NULL;
    }
    // Nastavenie addr3
    if (addr3 != NULL) {
        // Pozrieť či predtým niečo bolo na addr3
        if (instr->addr3 != NULL) {
            // uvoľniť a znovu alokovať
            free(instr->addr3);
        }
        instr->addr3 = malloc(sizeof(char) * strlen(addr3)+END_OF_STRING);
        if (instr->addr3 == NULL) {
            // Chyba - nepodarilo sa alokovať
            return ERR_INTERNAL;
        }
        strcpy(instr->addr3, addr3);
    }
    else {
        if (instr->addr3 != NULL) {
            free(instr->addr3);
        }
        instr->addr3 = NULL;
    }

    return ERR_OK;
}
