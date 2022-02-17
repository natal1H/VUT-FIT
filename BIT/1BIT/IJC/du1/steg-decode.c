/**
 * @file steg-decode.c
 * @author Natalia Holkova, FIT
 * @date 20.3.2018
 * @brief Testovaci program na vypisanie tajnej spravy
 *
 * Riesenie IJC-DU1, prklad b)
 * Prelozene: gcc 5.4
 */

#include "ppm.h"
#include "eratosthenes.h"
#include <ctype.h>

int main(int argc, char *argv[]) {
    /* Kontrola spravneho poctu argumentov prikazoveho riadku */
    if (argc != 2) {
        error_exit("Nespravny pocet argumentov.\n");
    }

    /* Vytvorenie struktury ppm */
    struct ppm * ppm_image;
    ppm_image = ppm_read(argv[1]);

    /* Kontrola spravnosti nacitanie struktury */
    if (ppm_image == NULL) {
        free(ppm_image);
        return -1;
    }

    unsigned long size = 0;
    size = 3 * ppm_image->xsize * ppm_image->ysize;

    bit_array_create(p, 3*SIZE_LIMIT*SIZE_LIMIT);

    Eratosthenes(p);

    unsigned long start = 11; // na ktorom prvocisle je prvy znak sifry
    int shift_counter = 0;
    unsigned char tmp = 0;
    for (unsigned long i = start; i < size; i++) {
        if (bit_array_getbit(p, i) == 0) { // i je prvocislo

            unsigned char c = ppm_image->data[i];

            /* Bitove posuny, cielom je vynulovat vsetky bity okrem LSB, ktory treba presunut na spravny index */
            c = c << 7; // dostan LSB na poziciu MSB
            c = c >> (7 - shift_counter); // dostan MSB na pozadovanu poziciu, ostatne bity su 0
            shift_counter++; // zvysenie pocitadla posunov

            /* Pripocitanie bajtu s jedinym bitom na urcenej pozicii */
            tmp += c;

            /* vypisanie tmp tmp ak boli vsetky bity naplnene */
            if (shift_counter == 8) {
                /* Koniec tajnej spravy */
                if (tmp == 0)
                    break;

                if (!isprint(tmp)) { // Chyba: nevypisatelny znak
                    free(ppm_image);
                    error_exit("V sprave sa nachadza znak, ktory sa neda vypisat.\n");
                }

                /* Vypisanie znaku */
                printf("%c", tmp);
                shift_counter = 0;
                tmp = 0;
            }
        }
    }

    printf("\n");

    free(ppm_image); // uvolnenie pamata

    if (tmp != '\0') {
        error_exit("Zasifrovana sprava nie je spravne ukoncena.\n");
    }

    return 0;
}