#include "parser.h"

#define GET_NEXT_TOKEN() if(get_next_token(token) == ERR_SCANNER) return ERR_SCANNER 

int depth_index = 0;
bool in_if_or_while = false;
bool in_def = false;
bool withBrackets = true;

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
int prog (Token *token) {

	// Príprava na generovanie while
	is_in_while = 0;
	active_before_while = NULL;
	//instr_beginning_while = NULL;
	// Koniec prípravy na generovanie while

	// Inicializácia zásobníka na parametre fukcie
	ParamStackInit(&stackParam);

/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 1: <prog> -> <stat_list> EOF |
  |_______________________________________|
*/
	if (token->type == KEYWORD) {
		if (strcmp(token->attribute, "def") == 0) {
			return stat_list(token);
		}
		else if (strcmp(token->attribute, "if") == 0) {
			return stat_list(token);
		}
		else if (strcmp(token->attribute, "while") == 0) {
			return stat_list(token);
		}
		else {
			return ERR_SYNTAX;
		}

	}
	else if (token->type == IDENTIFIER) {
		return stat_list(token);
	}
	else if (token->type == INTEGER || token->type == FLOAT || token->type == STRING || ((token->type == KEYWORD) && (strcmp(token->attribute, "nil") == 0)) || token->type == LEFT_ROUND_BRACKET) {
		return stat_list(token);
	}
	else if (token->type == TYPE_EOF) {
		return ERR_OK;
	}
	else return ERR_SYNTAX;
}





/**
	Funkcia pre stav <stat_list>.
	
	Statement list musí začínať kľúčovým slovom, identifikátorom alebo musí byť prázdny.
	V prípade, že prijatý token je typu KEYWORD a má vhodný atribút alebo je typu IDENTIFIER
	prechádza sa do stavu <stat>.
	V ostatných prípadoch nastane syntax error a funkcia vracia hodnotu ERR_SYNTAX.

	@params Token
	@return Chybový kód
*/
int stat_list (Token *token) {

	int statRetVal = 0; //Uchovava navratovu hodnotu funkcie stat

/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlá 2, 13 a 15: <stat_list> -> <stat> EOL <stat_list>                      |
  |                      <if_stat_list> -> <nested_stat> EOL <if_stat_list>         |
  |                      <nested_stat_list> -> <nested_stat> EOL <nested_stat_list> |
  |_________________________________________________________________________________|
*/
	if (token->type == KEYWORD) {
		if (strcmp(token->attribute, "def") == 0  && !in_if_or_while && !in_def) {	//Definícia funkcie sa nemôže nachádzať v zložených príkazoch ani v tele inej funkcie
			statRetVal = stat(token);
			if (statRetVal == ERR_OK) {
				GET_NEXT_TOKEN();

				if (token->type == EOL) {												//printf("eol\n");
					GET_NEXT_TOKEN();

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "if") == 0) {
			statRetVal = stat(token);
			if (statRetVal == ERR_OK) {
				GET_NEXT_TOKEN();

				if (token->type == EOL) {												//printf("eol\n");
					GET_NEXT_TOKEN();

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "while") == 0) {
			statRetVal = stat(token);
			if (statRetVal == ERR_OK) {
				GET_NEXT_TOKEN();

				if (token->type == EOL) {												//printf("eol\n");
					GET_NEXT_TOKEN();

					return stat_list(token);
				}
			}
		}

	/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
	  | Pravidlo 16: <nested_stat_list> -> epsilon |
	  |____________________________________________|
	*/
		else if (strcmp(token->attribute, "end") == 0) {
			if (in_def && !in_if_or_while) {

				// Generovanie kódu - vygenerovať návratovú hodnotu a return
				// Skontrolovať finalVar
				if (finalVar == NULL) // Vráti nil@nil
				    set_and_post_instr(&instr_list, curr_instr, I_PUSHS, "nil@nil", NULL, NULL);
				else {// Vráti premennú, v ktorej bol posledný výpočet
					gen_push_var(finalVar, T_UNDEFINED, true);
					// Nastaviť returnValue na typ premennej, ktorá sa vracia
					set_fuction_return_type(global_table, actual_function_name,typeFinal);
				}

				// Sémantická akcia - presunúť ukazovať na aktuálnu lokálnu tabuľku naspäť na MAIN
				tGlobalTableNodePtr main_global_node = get_function_node(global_table, MAIN); // Vráti ukazovvateľ na uzol s MAIN v GTS
				actual_function_name = MAIN;
				actual_function_ptr = (main_global_node->data->function_table); // Aktuálne lokálna tabuľka je nová lokálna tabuľka
				// Koniec sémantickej akcie

				end_function(); // Vygeneruje return
				// Koniec generovania kódu
			}
			if (in_if_or_while || in_def) {																//printf("end ");
				return ERR_OK;
			}
		}

	  /*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
		| Pravidlo 14: <if_stat_list> -> epsilon |
		|________________________________________|
	  */
		else if (strcmp(token->attribute, "else") == 0) {
			if (in_if_or_while) {																		//printf("else ");
				return ERR_OK;
			}
		}
	}
	else if (token->type == IDENTIFIER || token->type == FUNCTION_ONLY_ID) {
		statRetVal = stat(token);
		if (statRetVal == ERR_OK) {
			if (token->type == EOL) {																	//printf("eol\n");
				GET_NEXT_TOKEN();

				return stat_list(token);
			}
			else if (check_if_function_already_defined(global_table, func_id_copy)) {
				return stat_list(token);
			}
			
		}
		else {
			return statRetVal;
		}
	}
	else if (token->type == INTEGER || token->type == FLOAT || token->type == STRING || ((token->type == KEYWORD) && (strcmp(token->attribute, "nil") == 0)) || token->type == LEFT_ROUND_BRACKET) {
		return stat(token);
	}


/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 3: <stat_list> -> epsilon |
  |____________________________________|
*/
	else if (token->type == TYPE_EOF) {
		if (depth_index == 0) {																			//printf("eof\n");
			return ERR_OK;
		}
		else return ERR_SYNTAX;
	}
	
	return statRetVal;
}






/**
	Funkcia pre stav <stat>.
	
	Statement musí začínať kľúčovým slovom, identifikátorom alebo literálom.

	@params Token
	@return Chybový kód
*/
int stat (Token *token) {
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 4: <stat> -> DEF ID ( <params> ) EOL <nested_stat_list> END |
  |______________________________________________________________________|
*/

int retVal = 0;

	if (token->type == KEYWORD) {
		if (strcmp(token->attribute, "def") == 0) {														//rintf("def ");
			GET_NEXT_TOKEN();

			if (token->type == IDENTIFIER || token->type == FUNCTION_ONLY_ID) {															//printf("%s ", token->attribute);

				// Sémantická kontrola - nebola už definovaná funkcia s takýmto názvom?
				if (function_definition(&global_table, token->attribute) != ERR_OK) {
					// Funkcia bola predtým definovaná - došlo k redefinícii - sémantická chyba
					return ERR_SEM_UNDEF;
				}

				// Vytvoriť lokálnu tabuľku symbolov
				tLocalTableNodePtr *new_local_table_ptr = malloc(sizeof(tLocalTableNodePtr));
				local_table_init(new_local_table_ptr); // Inicializácia novej lokálnej tabuľky
				tGlobalTableNodePtr function_global_node = get_function_node(global_table, token->attribute); // Vráti ukazovateľ na uzol s token->attribute v GTS
				set_function_table(&function_global_node, new_local_table_ptr); // Naviazanie uzla v globálnej na novú lokálnu
				// Bude nasledovať definícia funkcie - preto treba zmeniť ukazovateľ na aktuálnu lokálnu TS z MAIN na tabuľku novej funkcie
				// Nastaviť actual_function_name (ID funkcie, v ktorej sa práve nachádza program) na token->attribute
				actual_function_name = token->attribute;
				// Nastaviť actual_function_table
				actual_function_ptr = new_local_table_ptr; // Aktuálne lokálna tabuľka je nová lokálna tabuľka
				// Koniec sémantickej kontroly

				// Generovanie kódu
				prepare_for_func(); // Presunie ukazovateľ na aktívny prvok zoznamu inštrukcií na začiatok za hlavičku a JUMP $main
				gen_function_header(token->attribute);
				// Koniec generovania kódu

				// Nastaviť finalVar na NULL, kvôli návratovej hodnote funkcie
				finalVar = NULL;

				GET_NEXT_TOKEN();

				if (token->type == LEFT_ROUND_BRACKET) {												//printf("( ");
					GET_NEXT_TOKEN();
                /*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
				  | Pravidlo 10: <params> -> epsilon |
				  |__________________________________|
                 */
					if (token->type == RIGHT_ROUND_BRACKET) {											//printf(") ");
						GET_NEXT_TOKEN();	
					}
					else {
						if (params(token) == ERR_OK) {
							GET_NEXT_TOKEN();
						}
					}

					if (token->type == EOL) {															//printf("eol\n");
						GET_NEXT_TOKEN();

						depth_index++;
						in_def = true;

						int stat_list_ret = stat_list(token);
						if (stat_list_ret == ERR_OK) {
							if (token->type == KEYWORD && strcmp(token->attribute, "end") == 0){
								depth_index--;
								in_def = false;

								return ERR_OK;
							}
						}
						else {
							return stat_list_ret;
						}
					}
				}
			}
		}
    /*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
	  | Pravidlá 5 a 17: <stat> -> IF <expr> THEN EOL <if_stat_list> ELSE EOL <nested_stat_list> END        |
	  |                  <nested_stat> -> IF <expr> THEN EOL <if_stat_list> ELSE EOL <nested_stat_list> END |
	  |_____________________________________________________________________________________________________|
     */
		else if (strcmp(token->attribute, "if") == 0) {													//printf("if ");
			GET_NEXT_TOKEN();

			// Získaj unikátny label ELSE
			char *label_else = get_and_set_unique_label(&label_table, "else");
			char *label_if_done = get_and_set_unique_label(&label_table, "id_end");
			if ( (retVal = CallExpressionParser(token)) == ERR_OK) {
				// Generovanie kódu: JUMPIFEQ
				gen_jumpifneq(label_else, finalVar);
				// Koniec generovania kódu
				if (strcmp(token->attribute, "then") == 0) {											//printf("then ");
					GET_NEXT_TOKEN();

					if (token->type == EOL) {															//printf("eol\n");
						GET_NEXT_TOKEN();
						depth_index++;
						in_if_or_while = true;

						if (stat_list(token) == ERR_OK) {
							if (strcmp(token->attribute, "else") == 0) {								//printf("else ");
								// Generovanie kódu: JUMP $if_end, LABEL $else
								gen_jump(label_if_done);
								gen_label(label_else);
								// Koniec generovania kódu
								GET_NEXT_TOKEN();

								if (token->type == EOL) {												//printf("eol\n");
									GET_NEXT_TOKEN();

									if (stat_list(token) == ERR_OK) {
										if (strcmp(token->attribute, "end") == 0) {
											// Generovanie kódu: LABEL $if_done
											gen_label(label_if_done);
											free(label_else); // Uvoľnenie pamäte
											free(label_if_done); // Uvoľnenie pamäte
											// Koniec generovania kódu
											depth_index--;
											if (depth_index == 0 || (in_def && depth_index == 1)) {
												in_if_or_while = false;
											}
											return ERR_OK;
										}
									}
								}
							}	
						}
					}
				}
			}
			return retVal;
		}
    /*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
	  | Pravidlá 6 a 18: <stat> -> WHILE <expr> DO EOL <nested_stat_list> END        |
	  |                  <nested_stat> -> WHILE <expr> DO EOL <nested_stat_list> END |
	  |______________________________________________________________________________|
     */
		else if (strcmp(token->attribute, "while") == 0) {												//printf("while ");
			// Nastavenie inštrukcie tesne pred while
			if (is_in_while == 0) {
				active_before_while = listGetActivePtr(&instr_list);
			}

			// Je v cykle, v cykle nemôžu byť žiadne deklarácie premenných
			is_in_while += 1;
			listInit(&while_declaration_list); // Inicializácia zoznamu deklarácií
			listFirst(&while_declaration_list);

			// Generovanie kódu
			char *label_while = get_and_set_unique_label(&label_table, "while");
			char *label_while_end = get_and_set_unique_label(&label_table, "while_end");
			char *compare_var = NULL;
			gen_label(label_while);


			// Koniec generovania kódu
			GET_NEXT_TOKEN();

			if (CallExpressionParser(token) == ERR_OK) {
				// Záloha finalVar, kvôli porovnávaniu na konci
				compare_var = malloc(sizeof(char) * strlen(finalVar));
				strcpy(compare_var, finalVar);

				// Tuto vygenerovať kontrolu splnenia podmienky
				gen_jumpifneq(label_while_end, compare_var);

				if (strcmp(token->attribute, "do") == 0) {												//printf("do ");
					GET_NEXT_TOKEN();

					if (token->type == EOL) {															//printf("eol\n");
						GET_NEXT_TOKEN();
						depth_index++;
						in_if_or_while = true;

						if (stat_list(token) == ERR_OK) {
							if (strcmp(token->attribute, "end") == 0) {
								// Generovanie kódu
								//gen_jumpifeq(label_while, compare_var);
								gen_jump(label_while);
								gen_label(label_while_end);
								free(label_while);
								free(compare_var);
								// Koniec generovania kódu

								// Vychádza z cyklu, zníž zanorenie
								is_in_while -= 1;

								// Vychádza zo všetkých cyklov
								if (is_in_while == 0) {
									// Dopísanie deklarácií pred LABEL $while
									listFirst(&while_declaration_list);

									tListItem *main_list_active_copy = (instr_list.active);
									main_list_active_copy = (instr_list.active);

									instr_list.active = active_before_while;

									while (while_declaration_list.active != NULL) {
										listInsertPostActive(&instr_list, while_declaration_list.active->Instruction);
										//listNext(&instr_list);
										listNext(&while_declaration_list);
									}

									instr_list.active = main_list_active_copy;

									// Koniec cyklu, upratanie
									listFree(&while_declaration_list);

									active_before_while = NULL;
								}

								depth_index--;
								if (depth_index == 0 || (in_def && depth_index == 1)) {
									in_if_or_while = false;
								}
								return ERR_OK;
							}
						}
					}
				}
			}
		}
	}
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlá 7 a 19: <stat> -> ID <after_id>        |
  |                  <nested_stat> -> ID <after_id> |
  |_________________________________________________|
*/
	else if (token->type == IDENTIFIER || token->type == FUNCTION_ONLY_ID) {																//printf("%s ", token->attribute);

        expression_token = *token;

        // Potrebujem zálohovať ID, lebo budem brať ďalšie tokeny
		id_copy = (char *) realloc(id_copy, sizeof(char) * strlen(token->attribute)+END_OF_STRING);
		strcpy(id_copy, token->attribute);
		GET_NEXT_TOKEN();

		int after_id_ret = after_id(token);
		return after_id_ret;
	}

/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlá 8 a 20: <stat> -> <expression>        |
  |					 <nested_stat> -> <expression> |
  |________________________________________________|
*/
	else if (token->type == INTEGER || token->type == FLOAT || token->type == STRING || ((token->type == KEYWORD) && (strcmp(token->attribute, "nil") == 0)) || token->type == LEFT_ROUND_BRACKET) {
		// Zavolať parser výrazov
		int retVal = CallExpressionParser(token);
		if (retVal == ERR_OK) {
			return ERR_OK;
		}
		else {
			return retVal;
		}
	}

	return ERR_SYNTAX;
}




/**
	Funkcia pre stav <params>.

	Kontroluje správnosť zápisu prvého parametru pri definícii funkcie s parametrami.
	Následne volá funkciu params_next.

	@params Token
	@return Chybový kód
*/
int params (Token *token) {
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 9: <params> -> ID <params_next> |
  |__________________________________________|
*/
	if (token->type == IDENTIFIER) {																	//printf("%s ", token->attribute);
		// Sémantická akcia - nastaviť počet params funkcie na 1, vložiť param do lok. TS funkcie s typom T_PARAM
		function_set_number_params(global_table, actual_function_name, 1);
		variable_set_defined(actual_function_ptr, token->attribute); /* Vloženie param do loc. TS funkcie*/
		variable_set_type(*actual_function_ptr, token->attribute, T_PARAM);
		function_add_param_id_to_list(global_table, actual_function_name, token->attribute);
		// Koniec sémantickej akcie

		// Vložiť prvý parameter na zásobník parametrov
		ParamStackPush(&stackParam, token->attribute);

		GET_NEXT_TOKEN();

		return params_next(token);
	}
	return ERR_SYNTAX;
}




/**
	Funkcia pre stav <params_next>.

	Kontroluje správnosť zápisu ďalších parametrov pri definícii funkcie.
	Parametre musia byť oddelené čiarkou a zoznam parametrov musí byť ukončený pravou zátvorkou.

	@params Token
	@return Chybový kód
*/

int params_next (Token *token) {
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 11: <params_next> -> , ID <params_next> |
  |__________________________________________________|
*/
	if (token->type == COMMA) {																			//printf(", ");
		GET_NEXT_TOKEN();

		if (token->type == IDENTIFIER) {																//printf("%s ", token->attribute);

			// Sémantická akcia - inkrementovať počet params funkcie, vložiť param do lok. TS funkcie s typom T_PARAM
			function_increase_number_params(global_table, actual_function_name); /* Inc. počet params */
			variable_set_defined(actual_function_ptr, token->attribute); /* Vloženie param do loc. TS funkcie*/
			variable_set_type(*actual_function_ptr, token->attribute, T_PARAM);
			function_add_param_id_to_list(global_table, actual_function_name, token->attribute);
			// Koniec sémantickej akcie

			// Pridanie parametra na zásobník
			ParamStackPush(&stackParam, token->attribute);

			GET_NEXT_TOKEN();

			return params_next(token);
		}
	}
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 12: <params_next> -> epsilon |
  |_______________________________________|
*/
	else if (token->type == RIGHT_ROUND_BRACKET) {														//printf(") ");

		// Generovanie kódu - popovanie parametrov zo zásobníka
		while (!ParamStackEmpty(&stackParam)) {
			// Kým nie je zásobník prázdny, POPS
			char *param_id = ParamStackTop(&stackParam); // ID parametra
			if (is_in_while > 0) {
				// Je vo while, pridávať na zvlášť zoznam
				gen_defvar(param_id, &while_declaration_list); // DEFVAR LF@%param_id
			}
			else {
				gen_defvar(param_id, &instr_list); // DEFVAR LF@%param_id
			}
			gen_pop_var(param_id); // POPS LF@%param_id
			ParamStackPop(&stackParam); // Odstráň parameter zo zásobníka
		}

		// Koniec generovania kódu

		return ERR_OK;
	}

	return ERR_SYNTAX;
}





/**
Funkcia pre stavy <arg_with_brackets> a <arg_without_brackets>.

Kontroluje správnosť zápisu prvého argumentu pri volaní funkcie s argumentmi.
Následne volá funkciu arg_next.

@params Token
@return Chybový kód
*/
int arg (Token *token) {
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlá 21 a 25: <arg_with_brackets> -> <value> <arg_next_with_brackets>       |
  |	                  <arg_without_brackets> -> <value> <arg_next_without_brackets> |
  |_________________________________________________________________________________|
*/
	int retVal = value(token);

	if (retVal != ERR_OK) {
		return retVal;
	}

	retVal = arg_next(token);

	if (retVal != ERR_OK) {
		return retVal;
	}
	
	//expected_params--;																					//printf("\n- decreasing exp params of function %s\n", func_id_copy);

	/* Sémantická kontrola */																			//printf("\n -expected number of params of function %s: %d\n", func_id_copy, expected_params);
	// Kontrola počtu načítaných parametrov
	if ((strcmp(func_id_copy, "print") != 0) && expected_params != 0) {
		fprintf(stderr, "Chyba! Nesprávny počet argumentov pri volaní funkcie.\n");
		return ERR_SEM_PARAM;
	}
	// Koniec sémantickej kontroly

	if (strcmp(func_id_copy, "print") != 0) {
		//printf("\n- generating CALL %s\n", func_id_copy);
		// Generovanie kódu - vygenerovanie CALL funkcie
		gen_call(func_id_copy);
		// Koniec generovania kódu
	}

	return ERR_OK;
}





/**
Funkcia pre stavy <arg_next_with_brackets> a <arg_next_without_brackets>.

Kontroluje správnosť zápisu ďaľších argumentov pri volaní funkcie s argumentmi.
Ak bola funkcia volaná so zátvorkou pred prvým argumentom, muí byť za posledným argumentom pravá zátvorka.
V opačnom prípade za ním musí byť EOL.

@params Token
@return Chybový kód
*/
int arg_next (Token *token) {
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 23 a 27: <arg_next_with_brackets> -> , <value> <arg_next_with_brackets>      |
  |                   <arg_next_without_brackets -> , <value> <arg_next_without_brackets> |
  |_______________________________________________________________________________________|
*/
	if (token->type == COMMA) {																			//printf(", ");
		GET_NEXT_TOKEN();

		int retVal = value(token);
		if (retVal == ERR_OK) {
			//expected_params--;																			//printf("\n- decreasing exp params of %s in arg_next\n", func_id_copy);
			return arg_next(token);
		}
		else {
			return retVal;
		}
	}
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 24: <arg_next_with_brackets> -> epsilon |
  |__________________________________________________|
*/

	else if (withBrackets && token->type == RIGHT_ROUND_BRACKET) {										//printf(") ");
		GET_NEXT_TOKEN();

		return ERR_OK;
	}
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 24: <arg_next_without_brackets> -> epsilon |
  |_____________________________________________________|
*/
	else if (!withBrackets && token->type == EOL) {
		return ERR_OK;
	}
	return ERR_SYNTAX;
}





/**
Funkcia pre stavy <after_id> a <after_func_call>.

Kontroluje syntaktickú správnosť volania funkcie, priraďovania do premennej a detekuje samostatné výrazy na riadku.

@params Token
@return Chybový kód
*/
int after_id (Token *token) {

	int retVal = 0;
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 29: <after_id> -> <after_func_call> |
  |______________________________________________|
*/

	// Sémantická akcia
	// Pozriem sa, či sa nejedná o vstavanú funkciu
	if (is_built_in_function(id_copy)) {
		//printf("Jedná sa o vstavanú funkciu.\n");
		// Pozrieť sa, či už bola predtým definovaná
		if (check_if_function_already_defined(global_table, id_copy) == false) {
			// Pridať funkciu do globálnej tabuľky symbolov
			function_set_defined(&global_table, id_copy);
			// Nastaviť správny počet parametrov
			built_in_function_set_param(global_table, id_copy);

			// Vygenerovať definíciu pred main
			if (in_def) returnPlaceCopy = listGetActivePtr(&instr_list);
			prepare_for_func();
			generate_built_in_function(id_copy);
			end_function();
			if (in_def) instr_list.active = returnPlaceCopy;
		}
	}
	// Koniec sémantickej akcie

/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 33: <after_func_call> -> ( <arg_with_brackets> ) |
  |___________________________________________________________|
*/
	if (token->type == LEFT_ROUND_BRACKET) {															//printf("( ");
		func_id_copy = realloc(func_id_copy, sizeof(char)*strlen(id_copy)+END_OF_STRING);
		strcpy(func_id_copy, id_copy);

		withBrackets = true;

		// Sémantická kontrola
		if (!check_if_function_already_defined(global_table, func_id_copy)) {
			// Funkcia ešte nebola definovaná - chyba
			fprintf(stderr, "Chyba! Funkcia volaná pred jej definíciou.\n");
			return ERR_SEM_UNDEF;
		}
		expected_params = function_get_number_params(global_table, func_id_copy); /* Získaj počet params funkcie*/ //printf("\nExpected number params: %d\n", expected_params);

		// Nastavenie ukazovateľa na aktívny prvok zoznamu parametrov na first
		function_param_list_set_first_active(global_table, func_id_copy);
		// Koniec sémantickej kontroly

		GET_NEXT_TOKEN();

    /*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  	  | Pravidlo 22: <arg_with_brackets> -> epsilon |
  	  |_____________________________________________|
     */
		if (token->type == RIGHT_ROUND_BRACKET) {														//printf(") ");
			// Sémantická kontrola
			if ((strcmp(func_id_copy, "print") != 0) && expected_params != 0) {
			    // Sémantická chyba - nesprávny počet parametrov
			    return ERR_SEM_PARAM;
			}

			// Generovanie kódu - vygenerovanie CALL funkcie
			gen_call(func_id_copy);
			// Koniec generovania kódu

			GET_NEXT_TOKEN();

			return ERR_OK;

		}
		else {
			retVal = arg(token);
	
			if (retVal == ERR_OK) {
				GET_NEXT_TOKEN();

				return ERR_OK;
			}
		}

	}
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 34: <after_func_call> -> <arg_without_brackets> |
  |__________________________________________________________|
*/
	else if (token->type == IDENTIFIER || token->type == INTEGER || token->type == FLOAT || token->type == STRING) {
		func_id_copy = realloc(func_id_copy, sizeof(char)*strlen(id_copy)+END_OF_STRING);
		strcpy(func_id_copy, id_copy);
		withBrackets = false;

     	expected_params = function_get_number_params(global_table, func_id_copy); /* Získaj počet params funkcie*/ //printf("\nVolanie bez zátvoriek: Expected number params: %d\n", expected_params);

		// Nastavenie ukazovateľa na aktívny prvok zoznamu parametrov na first
		function_param_list_set_first_active(global_table, func_id_copy);

		return arg(token);
	}
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 29: <after_id> -> epsilon |
  |____________________________________|
*/
	else if (token->type == EOL) {
		func_id_copy = realloc(func_id_copy, sizeof(char)*strlen(id_copy));
		strcpy(func_id_copy, id_copy);

		expected_params = function_get_number_params(global_table, func_id_copy); // Získaj počet params funkcie
		
		if (expected_params == 0) {

			// Generovanie kódu - vygenerovanie CALL funkcie
			gen_call(func_id_copy);
			// Koniec generovania kódu

			return ERR_OK;
		}
		else return ERR_SEM_PARAM;
	}

/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 30: <after_id> = <def_value> |
  |_______________________________________|
*/
	else if (token->type == ASSIGN) {																	//printf("= "); printf("\n\tID COPY: %s\n", id_copy);
		// Sémantická akcia
		// Pozrieť, či je premenná už definovaná
        if (get_variable_node(*actual_function_ptr, id_copy) == NULL) {
            // Premenná ešte nebola definovaná
            /* Definovať premennú */																		//printf("\n\t---PRIDAVAM DO STROMU %s\n", id_copy);
            variable_set_defined(actual_function_ptr, id_copy);
            // Generovanie kódu
            //gen_defvar(id_copy);																			//printf("\n\t---Teraz by sa definovalo %s\n", id_copy);
			if (is_in_while > 0) {
				// Je vo while, pridávať na zvlášť zoznam
				gen_defvar(id_copy, &while_declaration_list); // DEFVAR LF@%param_id
			}
			else {
				gen_defvar(id_copy, &instr_list); // DEFVAR LF@%param_id
			}
        }

		GET_NEXT_TOKEN();

		return def_value(token);

    } else if (
            token->type == ADDITION ||
            token->type == SUBTRACTION ||
            token->type == MULTIPLICATION ||
            token->type == DIVISION ||
            token->type == EQUALS ||
            token->type == NOT_EQUALS ||
            token->type == GREATER ||
            token->type == GREATER_OR_EQUALS ||
            token->type == LESS ||
            token->type == LESS_OR_EQUALS
            ) {
        expression = true;
        retVal = CallExpressionParser(token);

	} else { retVal = ERR_SYNTAX; } //Nie je najdene pravidlo automaticky ERR_SYNTAX
	
	return retVal;
}





/**
Funkcia pre stav <def_value>.

Zabezpe4uje syntaktickú správnosť hodnot priradených do premennej.

@params Token
@return Chybový kód
*/
int def_value (Token *token) {

	int retVal = 0;

/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 35: <def_value> -> <expression> |
  |__________________________________________|
*/
	if (token->type == INTEGER || token->type == FLOAT || token->type == STRING || token->type == LEFT_ROUND_BRACKET || 
			(token->type == KEYWORD && strcmp(token->attribute, "nil") == 0 )) {						//printf("expr ");

		retVal = CallExpressionParser(token);															//printf("\n\t\tId copy: %s\n", id_copy);
		if (retVal == ERR_OK) {
			/* Sémantická akcia: */																		//printf("\n\tNASTAVENIE HODNOTY %s na %d\n", id_copy, typeFinal);
			variable_set_type(*actual_function_ptr, id_copy, typeFinal);								//printf("\n\tFinalVar: %s\n", finalVar);
			// Generovanie kódu
			// Presun výsledku výrazu do premennej (MOVE id_copy finalVar)
			gen_move_var(id_copy, finalVar);
		}
		else {
			return retVal;
		}
	}

	else if (token->type == IDENTIFIER || token->type == FUNCTION_ONLY_ID) {																//printf("%s ", token->attribute);
    /*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  	  | Pravidlo 36: <def_value> -> ID <after_func_call> |
  	  |__________________________________________________|
     */
		// Sémantická akcia
		// Pozriem sa, či sa nejedná o vstavanú funkciu
		if (is_built_in_function(token->attribute)) {
			//printf("Jedná sa o vstavanú funkciu.\n");
			// Pozrieť sa, či už bola predtým definovaná
			if (check_if_function_already_defined(global_table, token->attribute) == false) {
				// Pridať funkciu do globálnej tabuľky symbolov
				function_set_defined(&global_table, token->attribute);
				// Nastaviť správny počet parametrov
				built_in_function_set_param(global_table, token->attribute);

				// Vygenerovať definíciu pred main
				if (in_def) returnPlaceCopy = listGetActivePtr(&instr_list);
				prepare_for_func();
				generate_built_in_function(token->attribute);
				end_function();
				if (in_def) instr_list.active = returnPlaceCopy;

			}

			// Nastaviť správny nový typ premennej
			//tDataType retType = built_in_function_get_return_type(token->attribute);
			//variable_set_type(*actual_function_ptr, id_copy, retType );
		}
		// Koniec sémantickej akcie

		// Kontrola, či sa jedná o definovanu funkciu. Ak nie poslať hneď parseru výrazov
        if (check_if_function_already_defined(global_table, token->attribute)) {
        	expected_params = function_get_number_params(global_table, token->attribute); /* Získaj počet params funkcie */ //printf("\nExpected number params: %d\n", expected_params);
			// Potrebujem zálohovať ID, lebo budem brať ďalšie tokeny
			func_id_copy = (char *) realloc(func_id_copy, sizeof(char) * strlen(token->attribute));
			strcpy(func_id_copy, token->attribute);

			// Nastavenie ukazovateľa na aktívny prvok zoznamu parametrov na first
			function_param_list_set_first_active(global_table, func_id_copy);
			// Koniec sémantickej kontroly

            GET_NEXT_TOKEN();
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 33: <after_func_call> -> ( <arg_with_brackets> ) |
  |___________________________________________________________|
*/
			if (token->type == LEFT_ROUND_BRACKET) {													//printf("( ");
				withBrackets = true;

				GET_NEXT_TOKEN();	
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 22: <arg_with_brackets> -> epsilon |
  |_____________________________________________|
*/
				if (token->type == RIGHT_ROUND_BRACKET) {												//printf(") ");
					// Sémantická kontrola
					if ((strcmp(func_id_copy, "print") != 0) && expected_params != 0) {
					    // Sémantická chyba - nesprávny počet parametrov
					    return ERR_SEM_PARAM;
					}

					// Generovanie kódu - vygenerovanie CALL funkcie
					gen_call(func_id_copy);
					// Vygenerovať POPVAR do premennej
					gen_pop_var(id_copy);
					// Koniec generovania kódu

					// Nastavenie nového datového typu premennej
					if (!is_built_in_function(func_id_copy))
						variable_set_type(*actual_function_ptr, id_copy, get_fuction_return_type(global_table, func_id_copy) );
					else
						variable_set_type(*actual_function_ptr, id_copy, built_in_function_get_return_type(func_id_copy));

					GET_NEXT_TOKEN();

					return ERR_OK;
				}
				else {
					retVal = arg(token);

					if (retVal == ERR_OK) {
						GET_NEXT_TOKEN();

						// Vygenerovanie vrátenia návratovej hodnoty funkcie
						gen_pop_var(id_copy);

						// Nastavenie nového datového typu premennej
						if (!is_built_in_function(func_id_copy))
							variable_set_type(*actual_function_ptr, id_copy, get_fuction_return_type(global_table, func_id_copy) );
						else
							variable_set_type(*actual_function_ptr, id_copy, built_in_function_get_return_type(func_id_copy));

						return ERR_OK;
					}
				}
			}

/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 34: <after_func_call> -> <arg_without_brackets> |
  |__________________________________________________________|
*/
			else if (token->type == IDENTIFIER || token->type == INTEGER || token->type == FLOAT || token->type == STRING) {
				withBrackets = false;	
	       		expected_params = function_get_number_params(global_table, func_id_copy); /* Získaj počet params funkcie */ //printf("\nVolanie bez zátvoriek: Expected number params: %d\n", expected_params);

	       		int argRet = arg(token);
				// Vygenerovať POPVAR do premennej
				gen_pop_var(id_copy);
				// Nastavenie nového datového typu premennej
				if (!is_built_in_function(func_id_copy))
					variable_set_type(*actual_function_ptr, id_copy, get_fuction_return_type(global_table, func_id_copy) );
				else
					variable_set_type(*actual_function_ptr, id_copy, built_in_function_get_return_type(func_id_copy));

	       		return argRet;
			}
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 26: <arg_without_brackets> ->epsilon |
  |_______________________________________________|
*/
			else if (token->type == EOL) {
				expected_params = function_get_number_params(global_table, func_id_copy); // Získaj počet params funkcie
			
				if (expected_params == 0) {
					// Generovanie kódu - vygenerovanie CALL funkcie
					gen_call(func_id_copy);
					// Vygenerovať POPVAR do premennej
					gen_pop_var(id_copy);
					// Koniec generovania kódu

					// Nastavenie nového datového typu premennej
					if (!is_built_in_function(func_id_copy))
						variable_set_type(*actual_function_ptr, id_copy, get_fuction_return_type(global_table, func_id_copy) );
					else
						variable_set_type(*actual_function_ptr, id_copy, built_in_function_get_return_type(func_id_copy));

					return ERR_OK;
				}
				else return ERR_SEM_PARAM;
			}
	    }
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 35: <def_value> -> <expression> |
  |__________________________________________|
 */
        else {
            // Výraz začínajúci premennou

            //Kontrola, ze PRVA premenna existuje, vzdy musi pred priradenim kamkolvek
            //Ostatne premenne si osetruje vyrazovy parser
            tDataType token1 = T_UNDEFINED;
            if (get_type_from_token(actual_function_ptr, token->attribute, &token1) != ERR_OK) {
                // Sémantická chyba - riešiť
                fprintf(stderr, "Chyba! Nedefinovaná premenná \n");
                return ERR_SEM_UNDEF;
            }

            //Ak premenna existuje, posielam vyrazovemu parseru
            retVal = CallExpressionParser(token);											            //printf("\n\t\tId copy: %s\n", id_copy);
            /* Sémantická akcia: */															            //printf("Type final: %d\n", typeFinal);
            variable_set_type(*actual_function_ptr, id_copy, typeFinal);
            // Vygenerovanie priradenia výslednej premennej z parsera výrazov do id_copy
            gen_move_var(id_copy, finalVar);
        }
	}
	else if (token->type == LEFT_ROUND_BRACKET) {														//printf("expr ");
		// Výraz začínajúci Zátvorkou        
		retVal = CallExpressionParser(token);															//printf("\n\t\tId copy: %s\n", id_copy);
        // Sémantická akcia:
        variable_set_type(*actual_function_ptr, id_copy, typeFinal);
	} else {retVal = ERR_SYNTAX;}

	
	return retVal;
}

/**
Funkcia pre stav <>.

Kontroluje syntaktickú správnosť argumentov pri volaní funkcie.

@params Token
@return Chybový kód
*/
int value (Token *token) {
/*|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
  | Pravidlo 37 - 41: <value> -> INTEGER | FLOAT | STRING | NIL | ID |
  |__________________________________________________________________|
*/
	// Sémantická akcia

	expected_params--; // Zníženie počtu ešte zostávajúcich parametrov

	if (expected_params < 0 && (strcmp(func_id_copy, "print") != 0)) {
		// Chyba - nesprávny počet parametrov
		return ERR_SEM_PARAM;
	}

    // Získať lokálnu tabuľku volanej funkcie
    tGlobalTableNodePtr called_function_node = get_function_node(global_table, func_id_copy); // Vráti ukazovvateľ na uzol s func_id_copy v GTS
    if(called_function_node == NULL) { // Fix ak je riadok "a b = vyraz" inak segfault pretoze called_function_node je NULL
        return ERR_SYNTAX;
    }
    tLocalTableNodePtr * called_function_table_ptr = (called_function_node->data->function_table); // Ukazovateľ na lokálnu tabuľku funkcie func_id_copy

	// Získať identifikátor parametra prislúchajúci argumentu
	char *actual_parameter = function_param_list_get_active(global_table, func_id_copy);
	function_param_list_next(global_table, func_id_copy); // Posunúť aktívny na ďalší
    // Koniec sémantickej akcie

	//printf("\n- argument %s pre funkciu %s prislúcha parametru %s\n", token->attribute, func_id_copy,actual_parameter);
 	if (token->type == KEYWORD && strcmp(token->attribute, "nil") == 0) {								//printf("nil ");
 		// Vygenerovanie PUSHS nil@nil
 		gen_push_var("nil", T_NIL, false);

 		if (!is_built_in_function(func_id_copy)) {
			// Nastavenie typu parametra actual_parametre na korešpondujúci typ argumentu
			variable_set_type(*called_function_table_ptr, actual_parameter, T_NIL);
		}
 		else if (strcmp(func_id_copy, "print") == 0) {
			// Generovanie kódu - vygenerovanie CALL funkcie
			gen_call(func_id_copy);
			// Koniec generovania kódu
 		}

		GET_NEXT_TOKEN();
		return ERR_OK;
	}
 	switch (token->type) {
		case INTEGER: {                                                                                    //printf("integer ");
			// Vygenerovanie PUSHS int@
			gen_push_var(token->attribute, T_INT, false);

			if (!is_built_in_function(func_id_copy)) {
				// Nastavenie typu parametra actual_parametre na korešpondujúci typ argumentu
				variable_set_type(*called_function_table_ptr, actual_parameter, T_INT);
			} else if (strcmp(func_id_copy, "print") == 0) {
				// Generovanie kódu - vygenerovanie CALL funkcie
				gen_call(func_id_copy);
				// Koniec generovania kódu
			}

			GET_NEXT_TOKEN();
			return ERR_OK;
			break;
		}
		case FLOAT: {                                                                                    //printf("float ");
			// Vygenerovanie PUSHS float@
			gen_push_var(token->attribute, T_FLOAT, false);

			if (!is_built_in_function(func_id_copy)) {
				// Nastavenie typu parametra actual_parametre na korešpondujúci typ argumentu
				variable_set_type(*called_function_table_ptr, actual_parameter, T_FLOAT);
			} else if (strcmp(func_id_copy, "print") == 0) {
				// Generovanie kódu - vygenerovanie CALL funkcie
				gen_call(func_id_copy);
				// Koniec generovania kódu
			}

			GET_NEXT_TOKEN();
			return ERR_OK;
			break;
		}
 		case STRING: {                                                                                    //printf("string ");
			// Vygenerovanie PUSHS string@
			gen_push_var(token->attribute, T_STRING, false);

			if (!is_built_in_function(func_id_copy)) {
				// Nastavenie typu parametra actual_parametre na korešpondujúci typ argumentu
				variable_set_type(*called_function_table_ptr, actual_parameter, T_STRING);
			} else if (strcmp(func_id_copy, "print") == 0) {
				// Generovanie kódu - vygenerovanie CALL funkcie
				gen_call(func_id_copy);
				// Koniec generovania kódu
			}

			GET_NEXT_TOKEN();
			return ERR_OK;
			break;
		}
 		case IDENTIFIER: {                                                                                //printf("%s ", token->attribute);
			// Sémantická kontrola
			// Premenná musí byť už definovaná
			tLocalTableNodePtr var_node = get_variable_node(*(actual_function_ptr), token->attribute);
			if (var_node == NULL) {
				// Neexistuje uzol s premennou -> nebola ešte definovaná - chyba
				fprintf(stderr, "Chyba! Nedefinovaná premenná.\n");
				return ERR_SEM_UNDEF;
			}
			// Vygenerovanie PUSHS LF@%
			gen_push_var(token->attribute, T_UNDEFINED, true);

			if (!is_built_in_function(func_id_copy)) {
				// Nastavenie typu parametra actual_parametre na korešpondujúci typ argumentu
				variable_set_type(*called_function_table_ptr, actual_parameter, var_node->data->type);
			} else if (strcmp(func_id_copy, "print") == 0) {
				// Generovanie kódu - vygenerovanie CALL funkcie
				gen_call(func_id_copy);
				// Koniec generovania kódu
			}

			GET_NEXT_TOKEN();

			return ERR_OK;
			break;
		}
 		default: {
			return ERR_SYNTAX;
		}
	}
}
