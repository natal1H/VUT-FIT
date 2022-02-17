/**
 * @file error.h
 * @author Natalia Holkova (xholko02), FIT
 * @date 20.3.2018
 * @brief Modul na chybove hlasenia
 *
 * Riesenie IJC-DU1, prklad b)
 * Prelozene: gcc 5.4
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef _ERROR_H_
    #define _ERROR_H_

    /**
     * @brief Funkcia na vypisanie varovania na stderr
     *
     * @param fmt format vypisu
     * @param ... dalsie premenne na vypisanie
     */
    void warning_msg(const char *fmt, ...);

    /**
     * @brief Funkcia na vypisanie chyboveho hlasenia na stderr a ukoncenie programu
     *
     * @param fmt format vypisu
     * @param ... dalsie premenne na vypisanie
     */
    void error_exit(const char *fmt, ...);

#endif