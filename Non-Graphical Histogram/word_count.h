/*
 * linkedlist.h
 *
 * This header file contains the two structures used in the .c file, and emalloc wrapper function.
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct word_t {
    struct word_t* next;
    struct word_t* prev;
    char* the_word;
} word_t;

typedef struct category {
    struct category* next;
    struct category* prev;
    struct word_t* first_word;
    int freq;
    int len;
} category;


/* Dynamically allocate memory of size_t n and return the 
 * pointer to the memory address
 *
 * exit with return code on allocation error 
 */
void* emalloc(size_t n) {
    void *p;

    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed", n);
        exit(3);
    }

    return p;
}


// Function examples provided by the assignment that I never used:
//
//

/* Example of how to free memory used by a double-linked list
 */
/*void cleanup_list(void *list) {
    if(list == NULL)
       return;
    cleanup_list(list->left);
    free(list->word);
    cleanup_list(list->right);
    free(list);
}*/

/* Example of how to print a linked list
 */
/*void print_list(void* list)
{    
    if (list == NULL) {
        return;
    }
    
    print_list(list->left);
    fprintf(stderr, "%s ", list->word); 
    print_list(list->right);
}*/

/* Example of how to apply a function to a each node in a linked list.  This is made so that different functions/arguments can be used.
 *
 * void *list : head of linked list to apply function
 * void (*fn)(void *list, void*) : function that takes in two arguments.
 * void *arg : second argument to pass to argument
 */
/*void apply(void *list,
           void (*fn)(void *list, void *),
           void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}
*/
