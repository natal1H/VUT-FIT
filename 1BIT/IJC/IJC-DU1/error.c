/**
 * @file error.c
 * @author Natalia Holkova (xholko02), FIT
 * @date 20.3.2018
 * @brief Implementacia funkcii na vypisanie chybovych hlaseni
 *
 * Riesenie IJC-DU1, prklad b)
 * Prelozene: gcc 5.4
 */

#include "error.h"

void warning_msg(const char *fmt, ...) {
    va_list argumenty;

    /* vystup na stderr */
    fprintf(stderr, "CHYBA: ");
    va_start(argumenty, fmt);
    vfprintf(stderr, fmt, argumenty);
    va_end(argumenty);
}

void error_exit(const char *fmt, ...) {
    va_list argumenty;

    /* vystup na stderr */
    fprintf(stderr, "CHYBA: ");
    va_start(argumenty, fmt);
    vfprintf(stderr, fmt, argumenty);
    va_end(argumenty);

    /* Ukoncenie programu s kodom 1 */
    exit(1);
}