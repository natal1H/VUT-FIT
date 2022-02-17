#ifndef _PARSER_H
#define _PARSER_H

#include "main.h" // Kvôli global_table
#include "scanner.h"
#include "error.h"
#include "expression_parser.h"
#include "semantic_analysis.h"
#include "code_gen.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool expression;
Token expression_token;


int expected_params;


/**
	Funkcia pre stav <prog>.

	Program musí začínať kľúčovým slovom, identifikátorom alebo musí byť prázdny (EOF).
	V prípade, že prijatý token je typu IDENTIFIER alebo je typu KEYWORD a má vhodný atribút,
	prechádza sa do stavu <stat_list>.
	V prípade, že prijatý token je EOF, preklad sa končí s návratovou hodnotou ERR_OK.
	V ostatných prípadoch nastane syntax error a funkcia vracia hodnotu ERR_SYNTAX.

	@params Token
	@return Chybový kód
*/
int prog (Token *token);


/**
	Funkcia pre stav <stat_list>.

	Statement list musí začínať kľúčovým slovom, identifikátorom alebo musí byť prázdny.
	V prípade, že prijatý token je typu KEYWORD a má vhodný atribút alebo je typu IDENTIFIER
	prechádza sa do stavu <stat>.
	V ostatných prípadoch nastane syntax error a funkcia vracia hodnotu ERR_SYNTAX.

	@params Token
	@return Chybový kód
*/
int stat_list (Token *token);


/**
	Funkcia pre stav <stat>.

	Statement musí začínať kľúčovým slovom, identifikátorom alebo literálom.

	@params Token
	@return Chybový kód
*/
int stat (Token *token);


/**
	Funkcia pre stav <params>.

	Kontroluje správnosť zápisu prvého parametru pri definícii funkcie s parametrami.
	Následne volá funkciu params_next.

	@params Token
	@return Chybový kód
*/
int params (Token *token);


/**
	Funkcia pre stav <params_next>.

	Kontroluje správnosť zápisu ďalších parametrov pri definícii funkcie.
	Parametre musia byť oddelené čiarkou a zoznam parametrov musí byť ukončený pravou zátvorkou.

	@params Token
	@return Chybový kód
*/

int params_next (Token *token);


/**
Funkcia pre stavy <arg_with_brackets> a <arg_without_brackets>.

Kontroluje správnosť zápisu prvého argumentu pri volaní funkcie s argumentmi.
Následne volá funkciu arg_next.

@params Token
@return Chybový kód
*/
int arg (Token *token) ;


/**
Funkcia pre stavy <arg_next_with_brackets> a <arg_next_without_brackets>.

Kontroluje správnosť zápisu ďaľších argumentov pri volaní funkcie s argumentmi.
Ak bola funkcia volaná so zátvorkou pred prvým argumentom, muí byť za posledným argumentom pravá zátvorka.
V opačnom prípade za ním musí byť EOL.

@params Token
@return Chybový kód
*/
int arg_next (Token *token);


/**
Funkcia pre stavy <after_id> a <after_func_call>.

Kontroluje syntaktickú správnosť volania funkcie, priraďovania do premennej a detekuje samostatné výrazy na riadku.

@params Token
@return Chybový kód
*/
int after_id (Token *token);


/**
Funkcia pre stav <def_value>.

Zabezpe4uje syntaktickú správnosť hodnot priradených do premennej.

@params Token
@return Chybový kód
*/
int def_value (Token *token);


/**
Funkcia pre stav <>.

Kontroluje syntaktickú správnosť argumentov pri volaní funkcie.

@params Token
@return Chybový kód
*/
int value (Token *token);

#endif