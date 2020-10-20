/* 
 * Subor:   proj2.c
 * Datum:   2017
 * Autor:   Natalia Holkova, xholko02@stud.fit.vutbr.cz
 * Projekt: Iteracne vypocty, projekt c. 2 pre predmet IZP
 */

#include <stdio.h>  // kvoli vystupu
#include <string.h> // kvoli funkcii strcmp
#include <stdlib.h> // kvoli funkcii strtod, strtol
#include <math.h>   // kvoli kontrole tan
#include <stdbool.h> 

#define N_MIN 0  // bez 0
#define M_MAX 14 // bez 14
#define ANGLE_MIN 0 // bez 0
#define ANGLE_MAX 1.4 // vratane 1.4
#define C_MIN 0 // bez 0
#define C_MAX 100 // vratane 100
#define NUMBER_ITER 9 // pocet iteracii cfrac potrebnych pri merani vzdialenosti/vysky

/* Kody chyb programu */
enum tecodes
{
  EOK = 0,       // Bez chyby
  EWRONGARG,     // Zle argumenty prikazoveho riadku
  EOUTOFRANGE,   // N, M mimo povoleny rozsah
  ECONVERSION    // Chyba pri prevode retazca na cislo
};

/* Funkcia na vypis chybovych hlaseni */
void printECode(int ecode) {
    /* Chybove hlasenia odpovedajuce chybovym kodom */
    const char *ECODEMSG[] = {
        [EOK] =            "Vsetko v poriadku.\n",
        [EWRONGARG] =      "Chyba! Nespravne argumenty.\n",
        [EOUTOFRANGE] =    "Chyba! Zadane hodnoty su mimo povoleny rozsah.\n",
        [ECONVERSION] =    "Chyba! Nepodarilo sa spravne previest retazec na cislo.\n"
    };
    fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/* Funkcia na vypocet tan pomocou Taylorovho polynomu */
double taylor_tan(double x, unsigned int n) {
    double taylor = 0.;
    double x_pow = x;
    double x2 = x*x;

    unsigned long long koef_cit[13] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604};
    unsigned long long koef_men[13] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};

    for (unsigned int k = 1; k < n; k++) {
        x_pow = x_pow * x2;
        taylor += (koef_cit[k] * x_pow) / koef_men[k] ;
    }
    return x + taylor;
}

/* Funkcia na vypocet tan pomocou zretazenych zlomkov */
double cfrac_tan(double x, unsigned int n) {
    double cf = 0.;
    int a; 
    double b = x*x;
    for (int k = n-1; k > 0; k--) {
	    a = 2*k + 1; 
        cf = b / (a - cf);
    }
    return x / (1 - cf);
}

/* Funkcia, ktora vrati vzdialenost od objektu */ 
double ret_distance(double tan_A, double c) {
    return c / tan_A;
}

/* Funkcia, ktora vrati vysku objektu */
double ret_height(double tan_B, double c, double distance) {
    double v1 = tan_B * distance;
    return v1 + c;
}

/* Funkcia na kontrolu, ci je double v dovolenom rozsahu */
int checkRangeInt(int num, int min, int max) {
    return (num >= min && num <= max) ? EOK : EOUTOFRANGE;
}

/* Funkcia na vypisanie napovedy na pouzitie programu */
void printHelp() {
    printf("Napoveda na pouzitie programu:\n"
            " - spustite program v tvare:\n"
            " ./proj2 --help <-- pre zobrazenie napovedy\n"
            " ./proj2 --tan A N M <-- pre porovnanie presnosti tan\n"
            " ./proj2 [-c X] -m A [B] <-- pre urcenie vzdialenosti (vysky)\n"
            " - pricom:\n"
            "   A - uhol v radianoch (0 < A <= 1.4  < pi/2)\n"
            "   B - volitelny argument, uhol v radianoch, program vypocita aj vysku (0 < B <= 1.4 < pi/2)\n"
            "   N, M - udavaju, v ktorych iteraciach iteracneho vypoctu ma prebiehat porovnavanie\n"
            "   -c - volitelny argument, nastavi vysku meracieho pristroja na X (0 < X <= 100), implicitna vyska je 1.5 m\n"
            "Autor: Natalia Holkova\n");

}

/* Funkcia, ktora vypise vystup pri zadani argumentu --tan */
void printCompTan(double alpha, int N, int M) {
    double lib_tan = tan(alpha);
    for (int i = N; i <= M; i++) {
        double taylor = taylor_tan(alpha, i);
        double cfrac = cfrac_tan(alpha, i);
        printf("%d %10e %10e %10e %10e %10e\n", i, lib_tan, taylor, lib_tan - taylor, cfrac, lib_tan - cfrac);
    }
}

/* Funkcia, ktora vypise vzdialenost od objektu */
void printDist(double alpha, double c) {
    double tan_alpha = cfrac_tan(alpha, NUMBER_ITER);
    double dist = ret_distance(tan_alpha, c);
    printf("%.10e\n", dist);
}

/* Funkcia, ktora vypise vysku objektu */
void printHeight(double beta, double c, double dist) {
    double tan_beta = cfrac_tan(beta, M_MAX-1);
    double height = ret_height(tan_beta, c, dist);
    printf("%.10e\n", height);
}

/* Funkcia, ktora nacita double, skontroluje spravnost prevodu a spravnost rozsahu */
int readDouble(double *p_number, char str[], double min, double max) {
    char* p_end; // na kontrolu spravneho priebehu prevodu
    *p_number = strtod(str, &p_end);
    if (*p_end != '\0') // nastala chyba pri prevode
        return ECONVERSION;
    else if (*p_number > min && *p_number <= max)
        return EOK;
    else // inak je mimo rozsah
        return EOUTOFRANGE;
}

/* Funkcia, ktora nacita int a skontroluje spravnost prevodu */
int readInt(int *p_number, char str[]) {
    char *end_string;
    *p_number = strtol(str, &end_string, 10);
    if (end_string[0] != '\0') // kontrola ci cislo neobsahovalo nepovolene znaky
        return ECONVERSION;
    else
        return EOK;
}

int main(int argc, char *argv[]) {
    bool help_arg = false, tan_arg = false, m_arg = false, B_arg = false;
    double alpha, beta, c = 1.5;
    int N, M, tmp, exit_code = EOK;

    /* --help */
    if (argc == 2 && (strcmp(argv[1], "--help")  == 0) )
        help_arg = true;
    /* -m A alebo -m A B */
    else if ( (argc == 3 && (strcmp(argv[1], "-m")  == 0) ) || (argc == 4 && (strcmp(argv[1], "-m") == 0)) ) {
        m_arg = true;
        if ( (tmp = readDouble(&alpha, argv[2], ANGLE_MIN, ANGLE_MAX)) != EOK) exit_code = tmp; // nacitanie alpha
        if (argc == 4) { // volitelny argument B
            if ( (tmp = readDouble(&beta, argv[3], ANGLE_MIN, ANGLE_MAX)) != EOK) exit_code = tmp; // nacitanie beta
            B_arg = true;
        }
    }
    /* --tan A N M */
    else if (argc == 5 && (strcmp(argv[1], "--tan")  == 0) ) {
        tan_arg = true;
        if ( (tmp = readDouble(&alpha, argv[2], ANGLE_MIN, ANGLE_MAX)) == ECONVERSION) exit_code = tmp; // nacitanie alpha
        if ( ( tmp = readInt(&N, argv[3])) != EOK ) exit_code = tmp; // nacitanie N a kontrola spravnosti prevodu
        if ( ( tmp = readInt(&M, argv[4])) != EOK ) exit_code  = tmp; // nacitanie M a kontrola spravnosti prevodu
        if ( exit_code == EOK && (checkRangeInt(N, N_MIN, M) || checkRangeInt(M, N, M_MAX)) ) exit_code = EOUTOFRANGE; // kontrola spravnosti rozsahu N a M
    }
    /* -c X M A alebo -c X M A B */
    else if ( (argc == 5 && (strcmp(argv[1], "-c")  == 0) && (strcmp(argv[3], "-m")  == 0)) || (argc == 6 && (strcmp(argv[1], "-c") == 0) && (strcmp(argv[3], "-m")  == 0)) ) {
        m_arg = true;
        if ( (tmp = readDouble(&c, argv[2], C_MIN, C_MAX)) != EOK) exit_code = tmp; // nacitanie vysky pristroja
        if ( (tmp = readDouble(&alpha, argv[4], ANGLE_MIN, ANGLE_MAX)) != EOK) exit_code = tmp; // nacitanie alpha
        if (argc == 6) { // volitelby argument B
            if ( (tmp = readDouble(&beta, argv[5], ANGLE_MIN, ANGLE_MAX)) != EOK) exit_code = tmp; // nacitanie B
            B_arg = true;
        }
    }
    /* Nespravny tvar argumentov */
    else
        exit_code = EWRONGARG;

    /* Kontrola ci pri spracovavani argumentov nenastala chyba */
    if (exit_code > EOK) { 
        printECode(exit_code);
        return exit_code;
    }
    /* Vystup programu */
    if (help_arg)  // vypisat napovedu
        printHelp();
    else if (tan_arg) // vypisat porovnanie tan
        printCompTan(alpha, N, M);
    else if (m_arg) { // vypisat vzdialenost
        printDist(alpha, c);
        if (B_arg) // vypisat aj vysku
            printHeight(beta, c, ret_distance(cfrac_tan(alpha, NUMBER_ITER), c));
    }
    return EOK;
}
/* Koniec proj2.c */
