/**
 * @file ppm.c
 * @author Natalia Holkova (xholko02), FIT
 * @date 20.3.2018
 * @brief implementacia modulu na pracu s .ppm obrazkami
 *
 * Riesenie IJC-DU1, prklad b)
 * Prelozene: gcc 5.4
 */

#include "ppm.h"


struct ppm * ppm_read(const char * filename) {
    FILE *f = fopen(filename, "rb");

    /* Kontrola nacitania suboru */
    if (f == NULL) {
        /* Chyba: nepodarilo sa nacitat subor, netreba uvolnit pamat */
        warning_msg("Nepodarilo sa nacitat subor %s.\n", filename);
        goto vratenie_NULL;
    }

    /* Kontrola spravnosti formatu na 1. riadku */
    if ( getc(f) != 'P' || getc(f) != '6') {
        warning_msg("Nespravny format.\n");
        goto zatvorenie_suboru;
    }

    int xsize, ysize;
    if ( fscanf(f, "%d %d", &xsize, &ysize) != 2) {
        warning_msg("Nepodarilo sa nacitat velkost obrazka.\n");
        goto zatvorenie_suboru;
    }

    /* Kontrola spravnosti velkosti obrazka */
    if (xsize <= 0 || ysize <= 0 || xsize > SIZE_LIMIT || ysize > SIZE_LIMIT) {
        warning_msg("Velkost obrazka (%d x %d px) je mimo povoleny interval 0..%d\n", xsize, ysize, SIZE_LIMIT);
        goto zatvorenie_suboru;
    }

    /* Dalsia kontrola spravnosti formatu */
    int color_range;
    if (fscanf(f, "%d", &color_range) != 1) {
        warning_msg("Nepodarilo sa nacitat farebny rozsah.\n");
        goto zatvorenie_suboru;
    }
    if (color_range != 255) {
        warning_msg("%d je nespravny rozsah farieb\n", color_range);
        goto zatvorenie_suboru;
    }

    /* Predchod na novy riadok */
    while ( getc(f) != '\n')
        ;

    int size = 3*xsize*ysize;

    /* Alokovanie pamate pre strukturu */
    struct ppm * p_ppm; // pointer to ppm
    p_ppm = (struct ppm *) malloc(sizeof(struct ppm) + size * sizeof(char) );
    if (p_ppm == NULL) {
        warning_msg("Nepodarilo sa alokovat pamat pre strukturu\n");
        goto zatvorenie_suboru;
    }

    /* Nastavenie atributov struktury */
    p_ppm->xsize = xsize;
    p_ppm->ysize = ysize;

    /* Nacitanie samotneho obsahu obrazka */
    int i = 0; // index bajtu v poli
    int c; // tmp character
    while (i < size) {
        c = getc(f); // nacitanie bajtu
        if (c == EOF) break; // koniec suboru
        if (c > color_range || c < 0) {
            warning_msg("%d nepatri do povoleneho farebneho intervalu 0..%d\n", c, color_range);
            goto uvolnenie_pamate;
        }
        p_ppm->data[i] = (char) c; // zapisanie do pola

        i++;
    }

    /* Kontrola nacitanie spravneho poctu bajtov */
    if (c == EOF && i != size) {
        warning_msg("Nacitanych menej farebnych bajtov ako je velkost obrazka %d\n", xsize*ysize);
        goto uvolnenie_pamate;
    }

    /* Zatvorenie suboru */
    if ( fclose(f) == EOF) {
        warning_msg("Nepodarilo sa zatvorit subor %s\n", filename);
        goto uvolnenie_pamate;
    }

    /* Vratenie ukazovatela na strukturu */
    return p_ppm;

    /* Riesenie chybovych situacii */
    uvolnenie_pamate:
        free(p_ppm);
    zatvorenie_suboru:
        fclose(f);
    vratenie_NULL:
        return NULL;
}

int ppm_write(struct ppm *p, const char * filename) {
    /* Kontrola spravnosti struktury ppm */
    if (p == NULL) {
        warning_msg("Ukazovatel na strukturu je NULL\n");
        return -1;
    }

    FILE * f = fopen(filename, "wb"); // vytvorenie suboru filename

    /* Kontrola vytvorenie suboru */
    if (f == NULL) {
        warning_msg("Nepodarilo sa vytvorit subor %s\n", filename);
        return -1;
    }

    /* Vytvorenie hlavicky suboru */
    fprintf(f, "P6\n%d %d\n255\n", p->xsize, p->ysize);

    /* Zapis do suboru */
    unsigned int i = 0;
    while (i < p->xsize *p->ysize * 3) {
        putc( (int) p->data[i], f);
        i++;
    }

    if (fclose(f) == EOF) {
        warning_msg("Nepodarilo sa zavretie subor %s\n", filename);
        return -1;
    }

    return 0;
}