/**
 * @file tail.h
 * @author Natalia Holkova, FIT
 * @date 5.4.2018
 * @brief Defines interface for module implementing function tail
 *
 * IJC-DU2, a)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Defines max length of a line
 */
#define MAX_LINE 1024


typedef struct element Element;
/**
 * @brief Defines structure of single element in list
 */
struct element {
    Element *pNext;
    char data[];
};

/**
 * @brief Defines structure linked list
 */
typedef struct {
    Element *pLast;
} List;

/**
 * @brief Allocates memory space for a list and sets first element to NULL
 * @return Pointer to allocated list
 */
List *list_ctor();

/**
 * @brief Adds element to the front of the list
 * @param list Pointer to list to which add element
 * @param data String representing line of input or string in general
 */
void push(List *list, char data[]);

/**
 * @brief Removes first element from linked list
 * @param list Pointer to list from which to remove element
 */
void pop(List *list);

/**
 * @brief Frees list
 * @param list Pointer to list to free
 */
void free_list(List *list);

/**
 * @brief Converts string to integer
 * @param p_number Pointer to number where to store output of conversion
 * @param str String which to convert to integer
 * @return Error code (0 = Success, 1 = Failure)
 */
int convert_str_to_int (int *p_number, char *str);

/**
 * @brief Reads lines from file and stores them into linked list as elements
 * @param filename Name of file from which get input
 * @param list Pointer to list in which store lines
 * @return Error code
 */
int load_from_file(char *filename, List *list, int toPrint);

/**
 * @brief Reads lines from stdin and stores them into linked list as elements
 * @param list Pointer to list in which store lines
 * @return Error code
 */
int load_from_stdin(List *list, int toPrint);