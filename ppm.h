/**
 * @file ppm.h
 * @author Natalia Holkova, FIT
 * @date 8.3.2018
 * @brief Rozhranie modulu na pracu s .ppm obrazkami
 *
 * IJC-DU1, prklad b)
 */

#include "error.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _PPM_H_

    /**
     * @brief Limit velkosti strany obrazka v px
     */
    #define SIZE_LIMIT 1000

    #define _PPM_H_
    /**
     * @brief PPM obrazok
     */
    struct ppm {
        /** Sirka obrazku v pixeloch */
        unsigned xsize;
        /** Vyska obrazku v pixeloch */
        unsigned ysize;
        /** Informacie o farbe jednotlivych pixeloch */
        char data[]; // RGB bajty, velkost 3*xsize*ysize
    };

    /**
     * @brief Nacitanie .ppm obrazku do struktury
     *
     * @param filename nazov obrazka na precitanie
     * @return ukazovatel na strukturu PPM obrazku
     */
    struct ppm * ppm_read(const char * filename);

    /**
     * @brief Zapise obsah struktury do suboru vo formate .ppm
     *
     * @param p ukazovatel na PPM strukturu
     * @param filename nazov obrazku na vytvorenie
     * @return chybovy kod
     */
    int ppm_write(struct ppm *p, const char * filename);

#endif