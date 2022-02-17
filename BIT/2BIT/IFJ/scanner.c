/**
 * IFJ Projekt - Team 11
 *
 * @brief Súbor na lexikálnu analýzu
 * @file scanner.c
 *
 * @author Natália Holková (xholko02)
 * @author Albert Szöllösi (xszoll02)
 */

#include "scanner.h"
#include "main.h"

#define scanner_is_alpha(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
#define scanner_is_number(c) (c >= '0' && c <= '9')

/** Inicializácia scannera **/
int scanner_initialize() {
    // Inicializuj read_string, do ktorého sa budú pri načítaní tokenov ukladať znaky
    read_string = tstring_struct_initialize();

    if (read_string == NULL) {
        return -1;
    }
    else {
        return 0;
    }
}

/** Inicializácia tokenu */
Token *token_initialize() {
    // Alokuj miesto pre štruktúru token
    Token *token = (Token *) malloc(sizeof(Token));
    if (token == NULL) {
        return NULL;
    }

    token->attribute = NULL;
    token->type = NO_TYPE;

    return token;
}

/** Uvoľnenie tokenu */
void token_free(Token *token) {
    free(token->attribute);
    free(token);
}

/**  Zistenie, či je string kľúčové slovo */
bool is_keyword(char *str) {
    // Porovnávanie reťazcov s našimi kľúčovými slovami
    if (strcmp(str, "def") == 0)
        return true;
    else if (strcmp(str, "do") == 0)
        return true;
    else if (strcmp(str, "else") == 0)
        return true;
    else if (strcmp(str, "end") == 0)
        return true;
    else if (strcmp(str, "if") == 0)
        return true;
    else if (strcmp(str, "not") == 0)
        return true;
    else if (strcmp(str, "nil") == 0)
        return true;
    else if (strcmp(str, "then") == 0)
        return true;
    else if (strcmp(str, "while") == 0)
        return true;
    else
        return false;
}

/** Nastavenie typu a atribútu tokenu */
int token_set_type_attribute(Token *token, Token_Type type, char *attribute) {
    // Token nesmie byť neinicializovaný
    if (token == NULL) {
        return ERR_INTERNAL;
    }
    // Nastav token type
    token->type = type;

    // Nastav attribute - treba alokovať miesto pre string
    token->attribute = (char *) malloc(sizeof(char) * strlen(attribute)+END_OF_STRING);
    if (token->attribute == NULL) {
        return ERR_INTERNAL;
    }

    // Skopírovať string do token->attribute
    strcpy(token->attribute, attribute);

    return 0;
}

/** Pomocná debugovacia funkica na výpis tokenu **/
void print_token(Token *token) {
    printf("Token type: %d\n", token->type);
    printf("Token attribute: %s\n", token->attribute);
}


/** Pretypuje string na integer **/
char* integer_to_string(int x) {
    char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
    if (buffer) {
        sprintf(buffer, "%d", x);
    }

    return buffer;
}

/** Pretypuje string na integer **/
int string_to_integer(char* x) {
    char *ptr;
    long return_value = strtol(x, &ptr, 10); //Pretypovanie char -> int

    return return_value;
}

/** Vráti správny tvar float literálu, ktorý sa už môže priamo zapísať do inštrukcií **/
char *get_correct_float_format(char *floatStr) {
    char *correctFloat = malloc(sizeof(char) * (strlen("0x") + strlen(floatStr)) + 2);
    double my_double;
    sscanf(floatStr, "%lf", &my_double);
    sprintf(correctFloat, "%a", my_double);
    return correctFloat;
}

/** Dvojica hexa znakov na INT v desiatkovej sústave **/
int hex_chr_to_decadic_int(char c1, char c2) {
    int ret = 0;
    if (c2 == 0) {
        // Iba jeden znak
        if (scanner_is_number(c1)) ret = (int) c1 - (int) '0';
        else if (c1 >= (int) 'a' && c1 <= (int) 'f') ret = (int) c1 - (int) 'a' + 10;
        else if (c1 >= (int) 'A' && c1 <= (int) 'F') ret = (int) c1 - (int) 'A' + 10;
    }
    else {
        // c1
        if (scanner_is_number(c1)) ret = ((int) c1 - (int) '0') * 16;
        else if (c1 >= (int) 'a' && c1 <= (int) 'f') ret = ((int) c1 - (int) 'a' + 10) * 16;
        else if (c1 >= (int) 'A' && c1 <= (int) 'F') ret = ((int) c1 - (int) 'A' + 10) * 16;
        // c2
        if (scanner_is_number(c2)) ret += (int) c2 - (int) '0';
        else if (c2 >= (int) 'a' && c2 <= (int) 'f') ret += (int) c2 - (int) 'a' + 10;
        else if (c2 >= (int) 'A' && c2 <= (int) 'F') ret += (int) c2 - (int) 'A' + 10;
    }

    return ret;
}

/** Číslo na string ako trojčíslie **/
char *int_to_decadic_three(int n) {
    char *str = malloc(sizeof(char) * 3);
    if (str == NULL) return NULL;

    if (n < 100) {
        if (n < 10) {
            // 1 cif
            str[0] = '0';
            str[1] = '0';
            str[2] = n + (int) '0';
        }
        else {
            // 2 cif
            str[0] = '0';
            char *n_str = integer_to_string(n);
            strcpy(str + 1, n_str);
            free(n_str);
        }
    }
    else {
        char *n_str = integer_to_string(n);
        strcpy(str, n_str);
        free(n_str);
    }
    return str;
}

/** Získanie ďalšieho tokenu zo stdin **/
int get_next_token(Token *token) {

    static bool newLine = true;

    tstring_clear_string(read_string); // str := '';

    TState state = START; // Nastavenie počiatočného stavu v DKA
    int c = getc(stdin);

    char hex_chr1 = 0;
    char hex_chr2 = 0;

    do {
        switch(state) {
            case START: // Starting point

                if (c == (int) '_') {
                    // START -> Q_ID_UNDERSCORE
                    // ID sa začína _
                    state = Q_ID_UNDERSCORE;

                    tstring_append_char(read_string, c); // str := symbol
                    newLine = false;
                }
                else if (c >= (int) 'a' && c <= (int) 'z') {
                    // START -> F_ID
                    // ID sa začína malým písmenom
                    state = F_ID;

                    tstring_append_char(read_string, c); // str := symbol
                    newLine = false;
                }
                else if (scanner_is_number(c) && c != (int) '0') {
                    // START -> F_INT
                    state = F_INT;

                    tstring_append_char(read_string, c); // str := symbol
                    newLine = false;
                }
                else if (c == (int) '0') {
                    // START -> F_INT_0
                    state = F_INT_0;

                    tstring_append_char(read_string, c); // str := symbol
                    newLine = false;
                }
                else if (c == (int) '"') {
                    // START -> Q_STRING
                    state = Q_STRING;

                    tstring_append_char(read_string, '"'); // Úvodná úvodzovka stringu
                    newLine = false;
                }
                else if (c == (int) '=') {
                    // START -> F_ASSIGN

					if (newLine) {
						state = Q_BLOCK_COMMENT_BEGIN_0;
						newLine = false;
					}
                    else {
						state = F_ASSIGN;

	                    tstring_append_char(read_string, c); // str := symbol
					}
                }
                else if (c == (int) '+') {
                    // START -> F_ADDITION
                    state = F_ADDITION;
                    token_set_type_attribute(token, ADDITION, "");
                    newLine = false;
                    return ERR_OK;
                }
                else if (c == (int) '*') {
                    // START -> F_MULTIPLICATION
                    state = F_MULTIPLICATION;
                }
                else if (c == (int) '-') {
                    // START -> F_SUBSTRACTION
                    state = F_SUBTRACTION;
                }
                else if (c == (int) '/') {
                    // START -> F_DIVISION
                    state = F_DIVISION;
                }
                else if (c == (int) '<') {
                    // START -> F_LESS
                    state = F_LESS;
                    tstring_append_char(read_string, c); // str := symbol
                    newLine = false;
                }
                else if (c == (int) '>') {
                    // START -> F_GREATER
                    state = F_GREATER;
                    tstring_append_char(read_string, c); // str := symbol
                    newLine = false;
                }
                else if (c == (int) '#') {
                    // START -> Q_LINE_COMMENT
                    state = Q_LINE_COMMENT;
                    newLine = false;
                }
                else if (c == (int) '!') {
                    // START -> Q_NOT_EQUALS
                    state = Q_NOT_EQUALS;
                    newLine = false;
                }
                else if (c == (int) '(') {
                    // START -> F_LEFT_ROUND_BRACKET
                    state = F_LEFT_ROUND_BRACKET;
                    newLine = false;
                }
                else if (c == (int) ')') {
                    // START -> F_RIGHT_ROUND_BRACKET
                    state = F_RIGHT_ROUND_BRACKET;
                    newLine = false;
                }
                else if (c == (int) '\n') {
                    // Ak bol ihneď predtým EOL, tak nech ďalší ignoruje
                    if (!newLine) {
                        newLine = true;
                        // START -> F_EOL
                        state = F_EOL;
                    }
                    //else zostáva v stave START
                }
                else if (c == (int) ',') {
                    // START -> F_COMMA
                    state = F_COMMA;
                    newLine = false;
                }
                else if (c == (int) ' ' || c == (int) '\t') {
                    // START -> START
                    //state = START;
					          newLine = false;
                }
                else if (c == EOF) {
                    // Koniec súboru
                    token->type = TYPE_EOF;
                    token->attribute = NULL;
                    newLine = false;
                    return EOF;
                }
                else {
                    // START -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                    newLine = false;
                }
                break; //case START:

            case Q_ID_UNDERSCORE:
                if (scanner_is_alpha(c) || scanner_is_number(c) || c == (int) '_') {
                    // Q_ID_UNDERSCORE -> F_ID
                    state = F_ID;
                    tstring_append_char(read_string, c);
                }
                else {
                    // Q_ID_UNDERSCORE -> F_LEX_ERROR
                    // Znak mimo povolených pre identifikátor, ktorý sa skladá iba z _ --> LEX ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_ID_UNDERSCORE:

            case F_ID:
                if (scanner_is_alpha(c) || scanner_is_number(c) || c == (int) '_') {
                    // F_ID -> F_ID
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) '?' || c == (int) '!') {
                    // F_ID -> F_ONLY_FUNCTION_IF
                    state = F_FUNCTION_ONLY_ID;
                    tstring_append_char(read_string, c); // str := symbol
                }
                else {
                    // Znak mimo povolených pre identifikátor

                    // unget znak
                    ungetc(c, stdin);

                    // zisti, či ten string je medzi kľúčovými slovami
                    if (is_keyword(read_string->string)) {
                        token_set_type_attribute(token, KEYWORD, read_string->string);
                    }
                    else {
                        token_set_type_attribute(token, IDENTIFIER, read_string->string);
                    }

                    return ERR_OK;
                }
                break; //case F_ID:

            case F_FUNCTION_ONLY_ID:
                // unget znak
                ungetc(c, stdin);
                token_set_type_attribute(token, FUNCTION_ONLY_ID, read_string->string);
                return ERR_OK;

            case F_INT:
                if (scanner_is_number(c)) {
                    // F_INT -> F_INT
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) '.') {
                    // F_INT -> Q_FLOAT_1
                    state = Q_FLOAT_1;
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) 'e' || c == (int) 'E') {
                    // F_INT -> Q_FLOAT_2
                    state = Q_FLOAT_2;
                    tstring_append_char(read_string, c);
                }
                else {
                    ungetc(c, stdin);
                    token_set_type_attribute(token, INTEGER, read_string->string);
                    return ERR_OK;
                }
                break; //case F_INT:

            case F_INT_0:
                if (scanner_is_number(c) && c != (int) '0') {
                    // F_INT_0 -> F_INT
                    state = F_INT;
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) '.') {
                    // F_INT_0 -> Q_FLOAT_1
                    state = Q_FLOAT_1;
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) 'e' || c == (int) 'E') {
                    // F_INT_0 -> Q_FLOAT_2
                    state = Q_FLOAT_2;
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) '0') {
                    // F_INT_0 -> F_LEX_ERROR
                    // Nemôže sa začínať viacerými nulami
                    state = F_LEX_ERROR;
                }
                else {
                    ungetc(c, stdin);
                    token_set_type_attribute(token, INTEGER, read_string->string);
                    return ERR_OK;
                }
                break; //case F_INT_0:

            case F_MULTIPLICATION:
                ungetc(c, stdin);
                token_set_type_attribute(token, MULTIPLICATION, "");
                newLine = false;
                return ERR_OK;

            case F_ADDITION:
                ungetc(c, stdin);
                tstring_append_char(read_string, c);
                newLine = false;
                return ERR_OK;

            case F_DIVISION:
                ungetc(c, stdin);
                token_set_type_attribute(token, DIVISION, "");
                newLine = false;
                return ERR_OK;

            case Q_FLOAT_1: // x.
                if (scanner_is_number(c)) {
                    // Q_FLOAT_1 -> F_FLOAT
                    state = F_FLOAT;
                    tstring_append_char(read_string, c);
                }
                else {
                    // Q_FLOAT_1 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_FLOAT_1:

            case Q_FLOAT_2: // xe / xE
                if (scanner_is_number(c) && c != (int) '0') {
                    // Q_FLOAT_2 -> F_FLOAT_WITH_E
                    state = F_FLOAT_WITH_E;
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) '0') {
                    // Q_FLOAT_2 -> F_FLOAT_WITH_E_0
                    state = F_FLOAT_WITH_E_0;
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) '+' || c == (int) '-') {
                    // Q_FLOAT_2 -> Q_FLOAT_3
                    state = Q_FLOAT_3;
                    tstring_append_char(read_string, c);
                }
                else {
                    // Q_FLOAT_2 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_FLOAT_2:

            case Q_FLOAT_3: // xe+ / xE+ / xe- / xE-
                if (scanner_is_number(c) && c != (int) '0') {
                    // Q_FLOAT_3 -> F_FLOAT_WITH_E
                    state = F_FLOAT_WITH_E;
                    tstring_append_char(read_string, c);
                }
                else {
                    // Q_FLOAT_3 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_FLOAT_3:

            case F_FLOAT:
                if (scanner_is_number(c)) {
                    // F_FLOAT -> F_FLOAT
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) 'e' || c == (int) 'E') {
                    // F_FLOAT -> Q_FLOAT_2
                    state = Q_FLOAT_2;
                    tstring_append_char(read_string, c);
                }
                else {
                    // TOKEN FLOAT
                    ungetc(c, stdin);
                    // Získaj správny tvar FLOAT
                    char *correctFloat = get_correct_float_format(read_string->string);
                    tstring_clear_string(read_string);
                    tstring_add_line(read_string, correctFloat);

                    token_set_type_attribute(token, FLOAT, read_string->string);
                    return ERR_OK;
                }
                break; //case F_FLOAT:

            case F_FLOAT_WITH_E:
                if (scanner_is_number(c)) {
                    // F_FLOAT_WITH_E -> F_FLOAT_WITH_E
                    tstring_append_char(read_string, c);
                }
                else {
                    // TOKEN FLOAT
                    ungetc(c, stdin);
                    // Získaj správny tvar FLOAT
                    char *correctFloat = get_correct_float_format(read_string->string);
                    tstring_clear_string(read_string);
                    tstring_add_line(read_string, correctFloat);

                    token_set_type_attribute(token, FLOAT, read_string->string);
                    return ERR_OK;
                }
                break; //case F_FLOAT_WITH_E:

            case F_FLOAT_WITH_E_0:
                ungetc(c, stdin);
                // Získaj správny tvar FLOAT
                char *correctFloat = get_correct_float_format(read_string->string);
                tstring_clear_string(read_string);
                tstring_add_line(read_string, correctFloat);

                token_set_type_attribute(token, FLOAT, read_string->string);
                return ERR_OK;
                break; //case F_FLOAT_WITH_E_0:


            case Q_STRING:
                if (c != (int) '"') {

                    if (c == (int) '\\' ) {
                        // Q_STRING -> Q_STRING_ESCAPE
                        state = Q_STRING_ESCAPE;
                        tstring_append_char(read_string, c);
                    }
                    else {
                        // Q_STRING -> Q_STRING
                        if (c == (int) ' ') {
                            // medzeru treba zapísať jej kódom
                            tstring_add_line(read_string, "\\032"); // Kód medzery
                        }
                        else
                            tstring_append_char(read_string, c);
                    }
                }
                else if (c == (int) '\n') {
                    // Q_STRING -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                else {
                    // Q_STRING -> F_STRING
                    state = F_STRING;
                }
                break; //case Q_STRING:

            case F_STRING:
                ungetc(c, stdin);
                tstring_append_char(read_string, '"');
                token_set_type_attribute(token, STRING, read_string->string);
                return ERR_OK;
                break;

            case Q_STRING_ESCAPE:
                if (c == (int) 'n' || c == (int) 't' || c == (int) 's' || c == (int) '\\' || c == (int) '"') {
                    // Q_STRING_ESCAPE -> Q_STRING
                    state = Q_STRING;

                    // Vlož do tstring číselnú hodnotu c (\n \t \s \\) v tvar aaa (dekadické číslo)
                    char *dec_str;
                    if (c == (int) 'n') dec_str = int_to_decadic_three( (int) '\n' );
                    else if (c == (int) 't') dec_str = int_to_decadic_three( (int) '\t' );
                    else if (c == (int) ' ') dec_str = int_to_decadic_three( (int) ' ' );
                    else if (c == (int) '\\') dec_str = int_to_decadic_three( (int) '\\' );
                    else if (c == (int) '"') dec_str = int_to_decadic_three( (int) '"' );
                    tstring_add_line(read_string, dec_str);
                    free(dec_str);
                }
                else if (c == (int) 'x') {
                    // Q_STRING_ESCAPE -> Q_STRING_HEX_1
                    state = Q_STRING_HEX_1;

                }
                else if (c == (int) '\n') {
                    // Q_STRING_ESCAPE -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                else {
                    // Q_STRING_ESCAPE -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }

                break;

            case Q_STRING_HEX_1:
                if (scanner_is_number(c) || (c >= (int) 'a' && c <= (int) 'f') || (c >= (int) 'A' && c <= (int) 'F') ) {
                    // Q_STRING_HEX_1 -> Q_STRING_HEX_2
                    state = Q_STRING_HEX_2;

                    hex_chr1 = c;
                }
                else if (c == (int) '\n') {
                    // Q_STRING_HEX_1 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                else {
                    // Q_STRING_HEX_1 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }

                break;

            case Q_STRING_HEX_2:
                if (scanner_is_number(c) || (c >= (int) 'a' && c <= (int) 'f') || (c >= (int) 'A' && c <= (int) 'F') ) {
                    // Q_STRING_HEX_2 -> Q_STRING
                    state = Q_STRING;

                    hex_chr2 = c;

                    // Prevod hex hodnoty na dekadickú a pridanie do string
                    // Prevod hex hodnoty na dekadickú a pridanie do string (iba jeden znak)
                    int decadic_number = hex_chr_to_decadic_int(hex_chr1, hex_chr2);

                    // Kontrola či nie je > 255
                    if (decadic_number > 255) state = F_LEX_ERROR;

                    char *decadic_str = int_to_decadic_three(decadic_number);
                    tstring_add_line(read_string, decadic_str);
                    free(decadic_str);

                }
                else if (c == (int) '\n') {
                    // Q_STRING_HEX_2 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                else {
                    // Q_STRING_HEX_2 -> Q_STRING
                    state = Q_STRING;

                    // Prevod hex hodnoty na dekadickú a pridanie do string (iba jeden znak)
                    int decadic_number = hex_chr_to_decadic_int(hex_chr1, hex_chr2);

                    // Kontrola či nie je > 255
                    if (decadic_number > 255) state = F_LEX_ERROR;

                    char *decadic_str = int_to_decadic_three(decadic_number);
                    tstring_add_line(read_string, decadic_str);
                    free(decadic_str);

                    // Už normálny znak
                    if (c == (int) ' ') {
                        // medzeru treba zapísať jej kódom
                        tstring_add_line(read_string, "\\032"); // Kód medzery
                    }
                    else
                        tstring_append_char(read_string, c);
                }

                break;

            case F_SUBTRACTION:
                // TOKEN -
                ungetc(c, stdin);
                token_set_type_attribute(token, SUBTRACTION, "");
                return ERR_OK;

            case Q_LINE_COMMENT:
                if (c == (int) '\n') {
                    // Q_LINE_COMMENT -> START
                    state = START;
                }
                break; //case Q_LINE_COMMENT:

			// "\n="
			case Q_BLOCK_COMMENT_BEGIN_0:
				
                if (c == (int) 'b'){
					// F_ASSIGN -> Q_BLOCK_COMMENT_BEGIN_1
					state = Q_BLOCK_COMMENT_BEGIN_1;
				} else state = F_LEX_ERROR;
			break;

			// "=b"
			case Q_BLOCK_COMMENT_BEGIN_1:
				if (c == (int) 'e') {
					state = Q_BLOCK_COMMENT_BEGIN_2;
				}
				else state = F_LEX_ERROR;
			break;

			// "=be"
			case Q_BLOCK_COMMENT_BEGIN_2:
				if (c == (int) 'g') {
					state = Q_BLOCK_COMMENT_BEGIN_3;
				}
				else state = F_LEX_ERROR;
			break;

			// "=beg"
			case Q_BLOCK_COMMENT_BEGIN_3:
				if (c == (int) 'i') {
					state = Q_BLOCK_COMMENT_BEGIN_4;
				}
				else state = F_LEX_ERROR; 
			break;

			// "=begi"
			case Q_BLOCK_COMMENT_BEGIN_4:
				if (c == (int) 'n') {
					state = Q_BLOCK_COMMENT_BEGIN_5;
				}
				else state = F_LEX_ERROR; 
			break;

			// "=begin"
			case Q_BLOCK_COMMENT_BEGIN_5:
				if (c == (int) ' ' || c == (int) '\t' || c == (int) '\n') {
					state = Q_BLOCK_COMMENT_CONTENT;
					if (c == (int) '\n') newLine = true;
				}
				else state = F_LEX_ERROR; 
			break;

			// Stav pre obsah blokového komentára
			case Q_BLOCK_COMMENT_CONTENT:
				if (c == (int) '\n') {
					newLine = true;
				}
				else if (newLine == true && c == (int) '=') {
					state = Q_BLOCK_COMMENT_END_1;
					newLine = false;
				}
				else {
					newLine = false;
				}
			break;
			

			case Q_BLOCK_COMMENT_END_1:
				if (c == (int) 'e') {
					state = Q_BLOCK_COMMENT_END_2;
				}
				else state = Q_BLOCK_COMMENT_CONTENT;
			break;


			// "=e"
			case Q_BLOCK_COMMENT_END_2:
				if (c == (int) 'n') {
					state = Q_BLOCK_COMMENT_END_3;
				}
				else state = Q_BLOCK_COMMENT_CONTENT;
			break;

			// "=e"
			case Q_BLOCK_COMMENT_END_3:
				if (c == (int) 'd') {
					state = Q_BLOCK_COMMENT_END_4;
				}
				else state = Q_BLOCK_COMMENT_CONTENT;
			break;

			// "=end"
			case Q_BLOCK_COMMENT_END_4:
				if (c == (int) ' ' || c == (int) '\t') {
					state = Q_BLOCK_COMMENT_POSTEND;
				}
				else if (c == (int) '\n') {
					newLine = true;
					state = START;
				}
				else state = Q_BLOCK_COMMENT_CONTENT;
			break;

			//Stav pre obsah riadku, na ktorom sa nachádza "=end".
			//Tento riadok je tiež súčasťou komentára.
			case Q_BLOCK_COMMENT_POSTEND:
				if (c == (int) '\n') {
					newLine = true;
					state = START;
				}
			break;


            case F_ASSIGN:
                if (c == (int) '=') {
                    // F_ASSIGN -> F_EQUALS
                    state = F_EQUALS;
                }
				else {
                    // TOKEN =
                    ungetc(c, stdin);
                    token_set_type_attribute(token, ASSIGN, "");
                    return ERR_OK;
                }
                break; //case F_ASSIGN:

            case F_EQUALS:
                // TOKEN ==
                token_set_type_attribute(token, EQUALS, "");
                return ERR_OK;

            case Q_NOT_EQUALS:
                if (c == (int) '=') {
                    // Q_NOT_EQUALS -> F_NOT_EQUALS
                    state = F_NOT_EQUALS;
                }
                else {
                    // Q_NOT_EQUALS -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_NOT_EQUALS:


            case F_NOT_EQUALS:
                // TOKEN !=
                token_set_type_attribute(token, NOT_EQUALS, "");
                return ERR_OK; //F_NOT_EQUALS:


            case F_GREATER:
                if (c == (int) '=') {
                    // F_GREATER -> F_GREATER_OR_EQUALS
                    state = F_GREATER_OR_EQUALS;
                }
                else {
                    // TOKEN >
                    ungetc(c, stdin);
                    token_set_type_attribute(token, GREATER, "");
                    return ERR_OK;
                }
                break; //case F_NOT_EQUALS:


            case F_LESS:
                if (c == (int) '=') {
                    // F_LESS -> F_LESS_OR_EQUALS
                    state = F_LESS_OR_EQUALS;
                }
                else {
                    // TOKEN <
                    ungetc(c, stdin);
                    token_set_type_attribute(token, LESS, "");
                    return ERR_OK;
                }
                break; //case F_LESS:


            case F_GREATER_OR_EQUALS:
                // TOKEN >=
                token_set_type_attribute(token, GREATER_OR_EQUALS, "");
                return ERR_OK; //case F_GREATER_OR_EQUALS:


            case F_LESS_OR_EQUALS:
                // TOKEN >=
                token_set_type_attribute(token, LESS_OR_EQUALS, "");
                return ERR_OK; //case F_LESS_OR_EQUALS:

            case F_LEFT_ROUND_BRACKET:
                // TOKEN (
                ungetc(c, stdin);
                token_set_type_attribute(token, LEFT_ROUND_BRACKET, "");
                return ERR_OK; //case F_LESS_OR_EQUALS:

            case F_RIGHT_ROUND_BRACKET:
                // TOKEN )
                ungetc(c, stdin);
                token_set_type_attribute(token, RIGHT_ROUND_BRACKET, "");
                return ERR_OK; //case F_LESS_OR_EQUALS:

            case F_EOL:
                // TOKEN EOL
                ungetc(c, stdin);
                token_set_type_attribute(token, EOL, "");
                return ERR_OK;

            case F_COMMA:
                // TOKEN ,
                ungetc(c, stdin);
                token_set_type_attribute(token, COMMA, "");
                return ERR_OK;

            case F_LEX_ERROR:
                // LEX ERROR
                token_set_type_attribute(token, LEX_ERROR, "");
                return ERR_SCANNER;
        } //switch(state)

        //Pokym nie je EOF suboru
    } while((c = getc(stdin)) != EOF);

    // Nech pozrie na koncový stav, ak je v F_nieco, nech vytvorí token s príslušným typom a do attribute nahrá zostatok v read_string
    if (c == EOF) { //Ak koniec suboru
        switch (state) {
            case F_ID:
                if (is_keyword(read_string->string)) {
                    token_set_type_attribute(token, KEYWORD, read_string->string);
                }
                else {
                    token_set_type_attribute(token, IDENTIFIER, read_string->string);
                }
                return ERR_OK;
            case F_INT:
                token_set_type_attribute(token, INTEGER, read_string->string);
                return ERR_OK;
            case F_FUNCTION_ONLY_ID:
                token_set_type_attribute(token, FUNCTION_ONLY_ID, read_string->string);
                return ERR_OK;
            case F_FLOAT:
                token_set_type_attribute(token, FLOAT, read_string->string);
                return ERR_OK;
            case F_STRING:
                token_set_type_attribute(token, STRING, read_string->string);
                return ERR_OK;
            case F_ASSIGN:
                token_set_type_attribute(token, ASSIGN, "");
                return ERR_OK;
            case F_ADDITION:
                token_set_type_attribute(token, ADDITION, "");
                return ERR_OK;
            case F_MULTIPLICATION:
                token_set_type_attribute(token, MULTIPLICATION, "");
                return ERR_OK;
            case F_SUBTRACTION:
                token_set_type_attribute(token, SUBTRACTION, "");
                return ERR_OK;
            case F_DIVISION:
                token_set_type_attribute(token, DIVISION, "");
                return ERR_OK;
            case F_LESS:
                token_set_type_attribute(token, LESS, "");
                return ERR_OK;
            case F_GREATER:
                token_set_type_attribute(token, GREATER, "");
                return ERR_OK;
            case F_EQUALS:
                token_set_type_attribute(token, EQUALS, "");
                return ERR_OK;
            case F_NOT_EQUALS:
                token_set_type_attribute(token, NOT_EQUALS, "");
                return ERR_OK;
            case F_LESS_OR_EQUALS:
                token_set_type_attribute(token, LESS_OR_EQUALS, "");
                return ERR_OK;
            case F_GREATER_OR_EQUALS:
                token_set_type_attribute(token, GREATER_OR_EQUALS, "");
                return ERR_OK;
            case F_EOL:
                token_set_type_attribute(token, EOL, "");
                return ERR_OK;
            case F_COMMA:
                token_set_type_attribute(token, COMMA, "");
                return ERR_OK;
            case F_LEX_ERROR:
                // LEX ERROR
                token_set_type_attribute(token, LEX_ERROR, "");
                return ERR_SCANNER;
            case F_LEFT_ROUND_BRACKET:
                token_set_type_attribute(token, LEFT_ROUND_BRACKET, "");
                return ERR_OK;
            case F_RIGHT_ROUND_BRACKET:
                token_set_type_attribute(token, RIGHT_ROUND_BRACKET, "");
                return ERR_OK;
            case F_INT_0:
                token_set_type_attribute(token, INTEGER, read_string->string);
                return  ERR_OK;
            case F_FLOAT_WITH_E:
                token_set_type_attribute(token, FLOAT, read_string->string);
                return  ERR_OK;
            case F_FLOAT_WITH_E_0:
                token_set_type_attribute(token, FLOAT, read_string->string);
                return  ERR_OK;

            case START:
            case Q_BLOCK_COMMENT_POSTEND:
            case Q_LINE_COMMENT:
                token->type = TYPE_EOF;
                token->attribute = NULL;
                return EOF;

            case Q_ID_UNDERSCORE:
            case Q_BLOCK_COMMENT_BEGIN_0:
            case Q_BLOCK_COMMENT_BEGIN_1:
            case Q_BLOCK_COMMENT_BEGIN_2:
            case Q_BLOCK_COMMENT_BEGIN_3:
            case Q_BLOCK_COMMENT_BEGIN_4:
            case Q_BLOCK_COMMENT_BEGIN_5:
            case Q_BLOCK_COMMENT_CONTENT:
            case Q_BLOCK_COMMENT_END_1:
            case Q_BLOCK_COMMENT_END_2:
            case Q_BLOCK_COMMENT_END_3:
            case Q_BLOCK_COMMENT_END_4:
            case Q_NOT_EQUALS:
            case Q_FLOAT_1:
            case Q_FLOAT_2:
            case Q_FLOAT_3:
            case Q_STRING:
            case Q_STRING_ESCAPE:
            case Q_STRING_HEX_1:
            case Q_STRING_HEX_2:
                // Nemalo by sa to zastaviť v nekoncom stave - chyba
                token_set_type_attribute(token, LEX_ERROR, "");
                return ERR_SCANNER;
                break;

        }
    } //(c == EOF)


    token->type = TYPE_EOF;
    token->attribute = NULL;
    return EOF;
}
