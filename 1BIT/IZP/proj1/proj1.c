/* 
 * Subor:   proj1.c
 * Datum:   2017
 * Autor:   Natalia Holkova, xholko02@stud.fit.vutbr.cz
 * Projekt: Praca s textom, projekt c. 1 pre predmet IZP
 */

#include <stdio.h> // pre vstup a vystup
#include <string.h> // kvoli funkcii strlen, strcpy
#include <ctype.h> // kvoli funkcii toupper, isprint

#define MAX_LINE 101 // Maximalna dlzka adresy
#define ALPHABET 43 // Pocet pismen abecedy + cisla + rezerva
#define LIMIT 42 // Limit poctu adries, ktore sa mozu spracovat

/* Kody chyb programu */
enum tecodes
{
  EOK = 0,       // Bez chyby
  ENOINPUT,      // Ziadne vstupne adresy
  ECLARG,        // Prilis vela argumentov prikazového riadku
  ELONGINPUT,    // Dlzka retazca prekrocila povoleny limit
  EWRONGCHAR     // Nepovoleny (netisknutelny) znak
};

/* Chybove hlasenia odpovedajuce chybovym kodom */
const char *ECODEMSG[] =
{
  [EOK] =        "Vsetko v poriadku.\n",
  [ENOINPUT] =   "Chyba! Ziadne zadane adresy.\n",
  [ECLARG] =     "Chyba! Prilis vela argumentov prikazoveho riadku.\n",
  [ELONGINPUT] = "Chyba! Zadany retazec prekrocil povolený limit.\n",
  [EWRONGCHAR] = "Chyba! Nepovoleny znak.\n"
};

/* Funkcia na vypis chybovych hlaseni */
void printECode(int ecode) {
    fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/*  Funkcie na výpis výstupu programu */
void printOutput(int numAd, char *enabled, char *found, char *exact) {
    if (numAd == 0) // Ziadna vhodna adresa
        printf("Not found\n");
    else if (numAd == 1) // Prave jedna vhodna adresa
        printf("Found: %s\n", found);
    else if (numAd > 1 && strlen(exact) > 0) { // Presna zhoda ale moznost pokracovania 
        printf("Found: %s\n", exact);
        printf("Enable: %s\n", enabled);
    }
    else // Iba ciastocna zhoda
        printf("Enable: %s\n", enabled);
}

/* Funkcia na načítanie zadanej adresy. */
int readAddress(char str[], int max_line) {
    int c;
    int i = 0;
    while (i < max_line && (c = toupper(getchar()) ) != EOF) {
        if ( !(isprint(c)) && c != '\n' )
            return EWRONGCHAR;
        if ( c != '\n')
            str[i] = c;
        else
            break;
        i++;
    }
    if ( c == EOF) // Ziadny dalsi vstup
        return EOF;
    else if (c == '\n') { // Uspech, koniec riadku
        str[i] = '\0';
        return 0;
    }
    else // Chyba, prilis dlhy vstup
        return ELONGINPUT;
}

/* Funkcia na porovnanie, či sa reťazec začína na zadaný podreťazec */
int compareStringBeginning(char *beginning, char *str) {
    int i = 0;
    while (beginning[i] != '\0' && str[i] != '\0') {
        if (toupper(beginning[i]) == str[i])
            i++;
        else
            break;
    }
    if (beginning[i] == '\0') // adresa sa zacina na dany retazec
        return 1;
    else // adresa sa nezacina na dany retazec
        return 0;
}

/* Funkcia, ktorá zisťuje, či sa zadaný znak už nachádza v reťazci */
int isCharInString(char c, char str[]) {
    int i = 0;
    while (str[i] != '\0') {
        if (c == str[i])
            return 1;
        i++;
    }
    return 0;
}

/* Funkcia, vloží znak do reťazca na správne miesto podľa abecedy */
void insertChar(char c, char str[]) {
    if (str[0] == '\0') {
        str[0] = c;
        str[1] = '\0';
    }
    else {
        int i = 0;
        while (str[i] != '\0') { // hladanie abecedne vhodneho miesta pre znak
            if (c < str[i])
                break;
            i++;
        }
        if (str[i] == '\0') { // doplnenie znaku na koniec
            str[i] = c;
            str[i+1] = '\0';
        }
        else { // potrebne posunut nasledujuce znaky
            char temp;
            char insert = c;
            int j;
            for (j = i; str[j] != '\0'; j++) {
                temp = str[j];
                str[j] = insert;
                insert = temp;
            }
            str[j] = insert;
            str[j+1] = '\0';
        }
    }
}

int main(int argc, char *argv[]) {
    char *inputString; // Zadana adresa cez adrgument

    // Kontrola poctu argumentov
    if (argc == 2)
        inputString = argv[1];
    else if (argc == 1)
        inputString = "";
    else {
        printECode(ECLARG);
        return ECLARG;
    }
    unsigned int lenInputStr = strlen(inputString);
    int possibleAddresses = 0; // pocet adries vyhovujucich zadaniu
    char possibleCharacters[ALPHABET] = ""; // pre všetky pismena abecedy
    char foundAddress[MAX_LINE+1], address[MAX_LINE+1], exactAddress[MAX_LINE+1] = "";
    int numAddress = 0; // pocita celkovy pocet adries kvoli limitu
    int exitCode = readAddress(address, MAX_LINE);

    if (exitCode == EOF) { // ziadny vstup
        printECode(ENOINPUT);
        return ENOINPUT;
    }
    while ( (exitCode != -1) && (numAddress < LIMIT) ) {
        if (exitCode != EOK) { // Kontrola ci nastala chyba
            printECode(exitCode);
            return exitCode;
        }
        if (compareStringBeginning(inputString, address) ) { // ak sa adresa zacina na zadany retazec
            if (lenInputStr != strlen(address) ) { // nie je uplna zhoda adries
                char c = address[lenInputStr]; // nacitanie vhodného znaku

                if ( !(isCharInString(c, possibleCharacters)) ) // kontrola, ci bol znak predtym nacitany
                    insertChar(c, possibleCharacters); // vlozi abecedne znak
            }
            else { // nastala presna zhoda
                strcpy(exactAddress, address);
            }
            strcpy(foundAddress, address);
            possibleAddresses++;
        }
	exitCode = readAddress(address, MAX_LINE);
        numAddress++;
    }
    printOutput(possibleAddresses, possibleCharacters, foundAddress, exactAddress); 

    return 0;
}
/* Koniec proj1.c */
