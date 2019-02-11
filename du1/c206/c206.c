// Vypracovala: Natália Holková (xholko02)

/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int errflg;
int solved;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

  // Nastavanie všetkých ukazovateľov na NULL
  L->Act = NULL;
  L->Last = NULL;
  L->First = NULL;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free.
**/

  if (L->First != NULL) { // Kontrola, či zoznam obsahuje prvky, ktoré bude treba uvolniť
    // Zoznam nie je prázdny
    tDLElemPtr temp = L->First;
    while (temp != NULL) { // Prechod všetkými prvkami
      tDLElemPtr toDelete = temp;
      temp = temp->rptr; // Presun ukazovateľa na nasledujúci element (doprava)
      free(toDelete);
    }
    L->First = NULL;
  }
  L->Last = NULL;
  L->Act = NULL;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

  tDLElemPtr newElementPtr = (tDLElemPtr) malloc (sizeof(struct tDLElem));
  if (newElementPtr == NULL) {
    // Chyba pri alokácii miesta pre element
    DLError();
    return ;
  }

  // Priradenie hodnoty dát a vytvorenie prepojenia ukazovateľov
  newElementPtr->data = val;
  newElementPtr->rptr = L->First;
  newElementPtr->lptr = NULL;

  if (L->First != NULL) { // Nie je prázdny zoznam - prepojenie prvého na nový element
    L->First->lptr = newElementPtr;
  }
  else { // Je prázdny zoznam - nový prvok je zároveň aj posledný prvok
    L->Last = newElementPtr;
  }
  L->First = newElementPtr;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

  tDLElemPtr newElementPtr = (tDLElemPtr) malloc (sizeof(struct tDLElem));
  if (newElementPtr == NULL) {
    // Chyba pri alokácii miesta pre nový element zoznamu
    DLError();
    return ;
  }

  newElementPtr->data = val;
  newElementPtr->rptr = NULL;

  if (L->First == NULL) {
    // Prázdny zoznam - novo-pridaný prvok je zároveň aj posledným prvkom

    newElementPtr->lptr = NULL;
    L->First = newElementPtr;
  }
  else {
    // Neprázdny zoznam
    L->Last->rptr = newElementPtr;
    newElementPtr->lptr = L->Last;
  }
  L->Last = newElementPtr;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
  L->Act = L->First;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
  L->Act = L->Last;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

  if (L->First == NULL) {
    // Chyba - prázdny zoznam
    DLError();
    return ;
  }

  *val = L->First->data;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

  if (L->First == NULL) {
    // Chyba - prázdny zoznam
    DLError();
    return ;
  }

  *val = L->Last->data;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

  tDLElemPtr elemPtr;
  if (L->First != NULL) {
    // Neprázdny zoznam

    elemPtr = L->First; // Záloha ukazovateľa na prvý prvok kvôli neskoršiemu vymazaniu
    if (L->Act == L->First) {
      // Aktívny prvok je prvý prvok - zoznam sa stane neaktívnym
      L->Act = NULL;
    }

    if (L->First == L->Last) {
      // Prvý prvok je zároveň sa posledným - zoznam ostane prádzny
      L->First = NULL;
      L->Last = NULL;
    }
    else {
      // Prvý nie je zároveň posledným - iba presunúť ukazovatele na nasledujúci
      L->First = L->First->rptr;
      L->First->lptr = NULL;
    }
    free(elemPtr);
  }

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

  tDLElemPtr elemPtr;
  if (L->First != NULL) {
    // Neprázdny zoznam - existuje prvok na vymazanie

    elemPtr = L->Last;
    if (L->Act == L->Last) {
      // Posledný prvok bol aktívny - zoznam sa stane neaktívnym
      L->Act = NULL;
    }

    if (L->First == L->Last) {
      // Posledný prvok bol zároveň prvý - zoznam ostane prázdny
      L->First = NULL;
      L->Last = NULL;
    }
    else {
      // Zoznam po vymazaní neostane prádzny - iba presunúť ukazovatele
      L->Last = L->Last->lptr;
      L->Last->rptr = NULL;
    }
    free(elemPtr);

  }

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/

  if (L->Act != NULL && L->Act->rptr != NULL) {
    // V zozname existuje aktívny prvok a nie je posledným prvokm

    tDLElemPtr elemPtr = L->Act->rptr; // Záloha ukazovateľa na prvok, ktorý sa ma vymazať
    L->Act->rptr = elemPtr->rptr;

    if (elemPtr == L->Last) {
      // Prvok za aktívnym je posledný - presun ukazovateľa na posledný prvok
      L->Last = L->Act;
    }
    else {
      // Prvok za aktívnym nie je posledný - prepojenie prvku doprava od neho s aktívnym
      elemPtr->rptr->lptr = L->Act;
    }
    free(elemPtr);
  }

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/

  if (L->Act != NULL && L->Act->lptr != NULL) {
    // Existuje aktívny prvok a existuje aj naľavo od neho prvok

    tDLElemPtr elemPtr = L->Act->lptr; // Záloha ukazovateľa na prvok, ktorý budeme vymazávať
    L->Act->lptr = elemPtr->lptr;

    if (elemPtr == L->First) {
      // Prvok, ktorý sa vymaže bol prvý - presun ukazovateľa na prvý prvok
      L->First = L->Act;
    }
    else {
      // Prvok, ktorý sa vymaže nebol prvý
      elemPtr->lptr->rptr = L->Act;
    }
    free(elemPtr);
  }

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

  if (L->Act != NULL) {
    // Existuje aktívny prvok v zozname

    tDLElemPtr newElementPtr = (tDLElemPtr) malloc (sizeof(struct tDLElem));
    if (newElementPtr == NULL) {
      // Chyba - nepodarilo sa alokovať miesto na nový prvok
      DLError();
      return ;
    }

    newElementPtr->data = val;
    newElementPtr->rptr = L->Act->rptr;
    newElementPtr->lptr = L->Act;
    L->Act->rptr = newElementPtr;

    if (L->Act == L->Last) {
      // Aktívny prvok bol posledný, zmena ukazovateľa na posledný prvok
      L->Last = newElementPtr;
    }
    else {
      // Aktivíny prvok nebol posledný
      newElementPtr->rptr->lptr = newElementPtr;
    }
  }


  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

  if (L->Act != NULL) {
    // Existuje aktívny prvok v zozname

    tDLElemPtr newElementPtr = (tDLElemPtr) malloc (sizeof(struct tDLElem));
    if (newElementPtr == NULL) {
      // Chyba pri pokuse o alokáciu miesta pre nový prvok
      DLError();
      return ;
    }

    newElementPtr->data = val;
    newElementPtr->rptr = L->Act;
    newElementPtr->lptr = L->Act->lptr;
    L->Act->lptr = newElementPtr;

    if (L->Act == L->First) {
      // Aktívny prvok bol prvý - presun ukazovateľa na prvý prvok
      L->First = newElementPtr;
    }
    else {
      // Aktívny prvok nebol prvý v zozname
      newElementPtr->lptr->rptr = newElementPtr;
    }
  }

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

  if (L->Act == NULL) {
    // Chyba - neaktívny zoznam
    DLError();
    return ;
  }

  *val = L->Act->data;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
  if (L->Act != NULL) { // Kontrola, či je zoznam aktívny
    L->Act->data = val;
  }

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/

  if (L->Act != NULL) { // Kontrola, či je zoznam aktívny
      L->Act = L->Act->rptr;
  }

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/

  if (L->Act != NULL) { // Kontrola, či je zoznam aktívny
    L->Act = L->Act->lptr;
  }

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/

  return L->Act != NULL;

  //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

/* Konec c206.c*/
