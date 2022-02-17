/**
 * IFJ Projekt - Team 11
 *
 * @brief Súbor pre prácu s dynamickým reťazcom
 * @file stringlib.c
 *
 * @author Natália Holková (xholko02)
 */

#include "stringlib.h"

/** Inicializácia reťazca v štruktúre TString **/
int tstring_intialize(TString *tstr) {
  // Alokovanie miesta pre reťazec na počiatočnú veľkosť
  tstr->string = (char *) malloc(sizeof(char) * TSTRING_INITIAL_SIZE);
  if (tstr->string == NULL) {
    // Chyba pri alokácii
    return ERR_INTERNAL;
  }
  // Nastavenie počiatočných hodnôt
  tstr->allocated = TSTRING_INITIAL_SIZE;
  tstr->length = 0;
  tstr->string[0] = '\0';

  return ERR_OK;
}

/** Inicializácia štruktúry TString **/
TString *tstring_struct_initialize() {
  // Alokovanie miesta pre štruktúru
  TString *mystruct = (TString *) malloc(sizeof(TString));
  if (mystruct == NULL) {
    // Chyba pri alokácie
    return NULL;
  }
  if (tstring_intialize(mystruct) != ERR_OK) {
    // Chyba pri alokovaní reťazca
    return NULL;
  }

  return mystruct;
}

/** Zvýšenie max dĺžky reťazca **/
int tstring_increase_size(TString *tstr) {
  tstr->string = (char *) realloc(tstr->string, tstr->allocated + TSTRING_INITIAL_SIZE); // Zvýši allocated o TSTRING_SIZE
  if (tstr->string == NULL) {
    // Chyba pri realokácii
    return ERR_INTERNAL;
  }
  tstr->allocated += TSTRING_INITIAL_SIZE;

  return ERR_OK;
}

/** Pridanie znaku do reťazca **/
int tstring_append_char(TString *tstr, char c) {
  // Pozri, či je ešte miesto na pridanie znaku
  if (tstr->length + 1 > tstr->allocated - 1) {
    // Treba najskôr zväčšiť alokované miesto
    if (tstring_increase_size(tstr) != 0) {
      // Nastala chyba pri rozširovaní - zlyhala aj táto funkcia
      return ERR_INTERNAL;
    }
  }

  // Nastavenie nového znaku + nového konca
  tstr->string[tstr->length] = c;
  tstr->string[tstr->length+1] = '\0';
  tstr->length += 1;

  return ERR_OK;
}

/** Vyčistenie reťazca v TString **/
int tstring_clear_string(TString *tstr) {
  if ( tstr == NULL ) {
    // Chyba - nie je inicializované alebo niečo podobné
    return ERR_INTERNAL;
  }
  if(tstr->allocated <= 0) {
    return ERR_INTERNAL;
  }

  // "Vyčistenie" reťaca - nastavenie konca na začiatok
  tstr->string[0] = '\0';
  tstr->length = 0;

  return 0;
}

/** Uvoľnenie reťazca v TString **/
void tstring_free_string(TString *tstr) {
  if(tstr != NULL) {
      free(tstr->string);
      tstr->length = 0;
      tstr->allocated = 0;
  }
}

/** Uvoľnenie štruktúry TString **/
void tstring_free_struct(TString *tstr) {
  tstring_free_string(tstr);
  free(tstr);
}

/** Vloženie reťazca do dynamického reťazca **/
int tstring_add_line(TString *tstr, char *line) {
  // Pozri, či je ešte miesto na pridanie celeho riadku
  while (tstr->length + strlen(line) > tstr->allocated - 1) {
    // Treba najskôr zväčšiť alokované miesto
    if (tstring_increase_size(tstr) != 0) {
      // Nastala chyba pri rozširovaní - zlyhala aj táto funkcia
      return ERR_INTERNAL;
    }
  }

  // pridaj na koniec
  for (int i = 0; i < strlen(line); i++) {
    tstr->string[tstr->length + i] = line[i];
  }
  tstr->string[tstr->length + strlen(line)] = '\0';
  tstr->length += strlen(line);
  return ERR_OK;
}

