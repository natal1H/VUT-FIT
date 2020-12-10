/**
 * @file common.c
 * @author Natalia Holkova (xholko02), FIT
 * @date 18.11.2019
 * @brief Implements function that both client and server use
 *
 * Project for ISA, task variant - HTTP dashboard
 */

#include "common.h"

/**
 * Function to check if string represents an integer
 *
 * @param str Input string
 * @return True if represents integer, false if not
 */
bool is_integer(char *str) {
    for (int i = 0; i < strlen(str); i++)
        if (str[i] < '0' || str[i] > '9')
            return false;

    return true;
}

/**
 * Function to get position of character in string
 *
 * @param str Input string
 * @param c Character to search for
 * @return Position of character or -1 if not found
 */
int get_index(char *str, char c) {
    char *tmp = str;
    int index = 0;
    while (*tmp++ != c && index < strlen(str)) index++;
    return (index == strlen(str) ? -1 : index);
}

/**
 * Function to get start position of substring within the string
 *
 * @param hay String which should contain the substring
 * @param needle Substring which should be inside the string
 * @return Position of substring within the string
 */
int strpos(char *hay, char *needle) {
    char haystack[strlen(hay)];
    strncpy(haystack, hay, strlen(hay));
    char *p = strstr(haystack, needle);
    if (p)
        return p - haystack;
    return -1;
}

/**
 * Function to check if argument <name> is valid, meaning it only contains 0-9, a-z or A-Z
 *
 * @param name String containing board name
 * @return True if name is valid, false otherwise
 */
bool check_name_validity(char *name) {
    char c;
    for (int i = 0; i < strlen(name); i++) {
        c = name[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
            return false;
        }
    }
    return true; // Name is valid
}
