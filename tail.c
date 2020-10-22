/**
 * @file tail.c
 * @author Natalia Holkova, FIT
 * @date 5.4.2018
 * @brief Implementation of function tail in C
 *
 * IJC-DU2, a)
 */

//TODO Fix memory leak - OK
//TODO Add comments inside functions
//TODO Change all text to english - OK
//TODO Clean up code, remove all unnecessary
//TODO Add error handling
//TODO Fix memory leak on error

#include "tail.h"

void error_exit(const char *fmt, ...) {
    va_list argumenty;

    /* vystup na stderr */
    fprintf(stderr, "ERROR: ");
    va_start(argumenty, fmt);
    vfprintf(stderr, fmt, argumenty);
    va_end(argumenty);

    /* Ukoncenie programu s kodom 1 */
    exit(1);
}

/* Initializes the list structure */
List * list_ctor() {
    List * list = malloc(sizeof(List));
    list->pLast = NULL;
    return list;
}

void addToEmpty(List *list, char *data) {
    // This function is only for empty list
    if (list->pLast != NULL)
        return ;

    // Creating a node dynamically.
    Element *temp = malloc(sizeof(Element) + MAX_LINE*sizeof(char));

    // Assigning the data.
    strcpy(temp->data, data);

    list->pLast = temp;

    // Creating the link.
    list->pLast->pNext = list->pLast;
}

void addEnd(List *list, char *data)
{
    if (list->pLast == NULL) return ;
    Element *temp = malloc(sizeof(Element) + MAX_LINE*sizeof(char));

    strcpy(temp->data, data);
    temp->pNext = list->pLast->pNext;
    list->pLast->pNext = temp;
    list->pLast = temp;
}

void deleteBegin(List *list) {
    if (list->pLast == NULL) return ;
    if (list->pLast->pNext == list->pLast) {
        free(list->pLast);
        list->pLast = NULL;
        return ;
    }

    Element *temp = list->pLast->pNext; // temp --> begin
    list->pLast->pNext = temp->pNext;
    free(temp);
}

void addElement(List *list, char *data, int capacity, int *current) {
    if (*current == 0) {
        addToEmpty(list, data);
        *current = *current + 1;
    }
    else if (*current < capacity) {
        addEnd(list, data);
        *current = *current + 1;
    }
    else {
        addEnd(list, data);
        deleteBegin(list);
    }
}

void print_list(List *list) {
    Element *p;

    // If list is empty, return.
    if (list->pLast == NULL)
        return;

    // Pointing to first Node of the list.
    p = list->pLast->pNext;

    // Traversing the list.
    do {
        printf("%s\n",p->data);
        p = p->pNext;

    } while(p != list->pLast->pNext);
}

void free_list(List *list) {
    if (list->pLast != NULL) {
        Element *begin = list->pLast->pNext;
        while (begin != list->pLast) {
            deleteBegin(list);
            begin = list->pLast->pNext;
        }
    }
    free(list);
}

int convert_str_to_int(int *p_number, char *str) {
    char *e_ptr = NULL;
    *p_number = (int) strtol(str, &e_ptr, 10);
    return (strlen(e_ptr) == 0) ? 0 : 1;
}

int load_lines(FILE *f, List *list, int toPrint) {
    int current = 0;
    int c;
    char line[MAX_LINE];
    int index = 0;
    while ( (c = getc(f)) != EOF) {
        if ( c != '\n' && index >= MAX_LINE) {
            line[index] = '\0';
            index = 0;
            addElement(list, line, toPrint, &current);
            fprintf(stderr, "Error! Line %s is longer than allowed.\n", line);
            // prechod na novy riadok
            while ((c = getc(f)) != EOF) {
                if (c == '\n')
                    break;
            }
        }
        else {
            if (c == '\n') {
                line[index] = '\0';
                index = 0;
                addElement(list, line, toPrint, &current);
            }
            else {
                line[index] = (char) c;
                index++;
            }
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
    int lines_to_print = 10, err_code;
    List *list = list_ctor();
    FILE *f;

    if (argc == 1)
        f = stdin;
    else if (argc == 2) {
        f = fopen(argv[1], "r");
        if (f == NULL) {
            fprintf(stderr, "Error! Could not open file %s.\n", argv[1]);
            return -1;
        }
    }
    else if (argc == 3) {
        if (strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n') {
            fprintf(stderr, "Error! %s is not -n.\n", argv[1]);
            return -1;
        }
        else {
            if (convert_str_to_int(&lines_to_print, argv[2]) != 0) {
                fprintf(stderr, "Error! %s is not an integer.\n", argv[2]);
                return -1;
            } else if (lines_to_print <= 0) {
                fprintf(stderr, "Error! %d is not greater than 0.\n", lines_to_print);
                return -1;
            } else
                f = stdin;
        }
    }
    else if (argc == 4) {
        // Kontrola -n
        if (strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n') {
            fprintf(stderr, "Error! %s is not -n.\n", argv[1]);
            return -1;
        }
        else {
            if (convert_str_to_int(&lines_to_print, argv[2]) != 0) {
                fprintf(stderr, "Error! %s is not an integer.\n", argv[2]);
                return -1;
            } else if (lines_to_print <= 0) {
                fprintf(stderr, "Error! %d is not greater than 0.\n", lines_to_print);
                return -1;
            } else {
                f = fopen(argv[3], "r");
                if (f == NULL) {
                    fprintf(stderr, "Error! Could not open file %s.\n", argv[3]);
                    return -1;
                }
            }
        }
    }
    else {
        printf("Error! Wrong arguments.\n");
        return -1;
    }

    err_code = load_lines(f, list, lines_to_print);
    if (f != stdin) fclose(f);
    if (err_code != 0) return err_code;
    print_list(list);
    free_list(list);

    return 0;
}