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

int main(int argc, char** argv)
{
    int lines_to_print = 10;
    List *list = list_ctor();

    if (argc == 1) {
        load_from_stdin(list);
        print_last_X_reverse(list, lines_to_print);
    }
    else if (argc == 2) {
        load_from_file(argv[1], list);
        print_last_X_reverse(list, lines_to_print);
    }
    else if (argc == 3) {
        if (strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n') {
            fprintf(stderr, "Error! %s is not -n.\n", argv[1]);
        }
        else {
            if (convert_str_to_int(&lines_to_print, argv[2]) != 0) {
                fprintf(stderr, "Error! %s is not an integer.\n", argv[2]);
            } else if (lines_to_print <= 0) {
                fprintf(stderr, "Error! %d is not greater than 0.\n", lines_to_print);
            } else {
                load_from_stdin(list);
                print_last_X_reverse(list, lines_to_print);
            }
        }
    }
    else if (argc == 4) {
        // Kontrola -n
        if (strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n') {
            fprintf(stderr, "Error! %s is not -n.\n", argv[1]);
        }
        else {
            if (convert_str_to_int(&lines_to_print, argv[2]) != 0) {
                fprintf(stderr, "Error! %s is not an integer.\n", argv[2]);
            } else if (lines_to_print <= 0) {
                fprintf(stderr, "Error! %d is not greater than 0.\n", lines_to_print);
            } else {
                load_from_file(argv[3], list);
                print_last_X_reverse(list, lines_to_print);
            }
        }
    }
    else {
        printf("Error! Wrong arguments.\n");
    }
    free_list(list);

    return 0;
}

/* Initializes the list structure */
List *list_ctor() {
    List *list = malloc(sizeof(List));
    if (list == NULL) {
        return NULL;
    }
    else {
        list->pFirst = NULL;
        return list;
    }
}

/* Adds a value to the front of the list */
void push(List *list, char data[]) {
    Element *newElement;
    newElement = malloc(sizeof(struct element) + MAX_LINE*sizeof(char));
    strcpy(newElement->data, data);
    if (list->pFirst != NULL) {
        newElement->pNext = list->pFirst;
        list->pFirst->pPrev = newElement;
        newElement->pPrev = NULL;
        list->pFirst = newElement;
    }
    else {
        list->pFirst = newElement;
        newElement->pNext = NULL;
        newElement->pPrev = NULL;
    }

}

void print_all_reverse(List *list) {
    Element *tmp = list->pFirst;
    if (tmp != NULL) {
        while (tmp->pNext != NULL) {
            tmp = tmp->pNext;
        }
        while (tmp != NULL) {
            printf("%s\n", tmp->data);
            tmp = tmp->pPrev;
        }
    }
}

void print_last_X_reverse(List *list, int x) {
    if (x > 0) {
        Element *tmp = list->pFirst;
        int i = 1;
        while (tmp != NULL) {
            if (i >= x)
                break;
            i++;
            tmp = tmp->pNext;
        }
        if (tmp == NULL)
            print_all_reverse(list);
        else {
            while (tmp != NULL) {
                printf("%s\n", tmp->data);
                tmp = tmp->pPrev;
            }
        }
    }
}

/* Removes the first value of the list */
void pop(List *list) {
    if (list->pFirst != NULL) {
        Element *toRemove = list->pFirst;
        list->pFirst = list->pFirst->pNext;
        free(toRemove);
    }
}

void free_list(List *list) {
    Element *tmp = list->pFirst;
    while (tmp != NULL) {
        pop(list);
        tmp = list->pFirst;
    }
    free(list);
}

int convert_str_to_int(int *p_number, char *str) {
    char *e_ptr = NULL;
    *p_number = (int) strtol(str, &e_ptr, 10);
    return (strlen(e_ptr) == 0) ? 0 : 1;
}

int load_from_file(char *filename, List *list) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Error! Could not open file %s.\n", filename);
        return -1;
    }

    int c;
    char line[MAX_LINE];
    int index = 0;
    while ( (c = getc(f)) != EOF) {
        if ( c != '\n' && index >= MAX_LINE) {
            line[index] = '\0';
            index = 0;
            push(list, line);
            fprintf(stderr, "Error! Line %s is longer than allowed.\n", line);
            // prechod na novy riadok
            while ((c = getc(f)) != EOF) {
                if (c == '\n') {
                    break;
                }
            }
        }
        else {
            if (c == '\n') {
                line[index] = '\0';
                index = 0;
                push(list, line);
            } else {
                line[index] = (char) c;
                index++;
            }
        }
    }
    fclose(f);

    return 0;
}

int load_from_stdin(List *list) {
    int c;
    char line[MAX_LINE];
    int index = 0;
    while ( (c = getchar()) != EOF) {
        if ( c != '\n' && index >= MAX_LINE) {
            line[index] = '\0';
            index = 0;
            push(list, line);
            fprintf(stderr, "Error! Line %s is longer than allowed.\n", line);
            // prechod na novy riadok
            while ((c = getchar()) != EOF) {
                if (c == '\n') {
                    break;
                }
            }
        }
        else {
            if (c == '\n') {
                line[index] = '\0';
                index = 0;
                push(list, line);
            } else {
                line[index] = (char) c;
                index++;
            }
        }
    }

    return 0;
}