/*
 * linkedlist.c
 *
 * Based on the implementation approach described in "The Practice 
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"


void *emalloc(size_t n) {
    void *p;

    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed", n);
        exit(1);
    }

    return p;
}


wordnode_t *new_word(char *w, int val) {
    assert( w != NULL);

    wordnode_t *temp = (wordnode_t *)emalloc(sizeof(wordnode_t));

    temp->w = strdup(w);
    temp->val = val;
    temp->next = NULL;

    return temp;
}


wordnode_t *add_front(wordnode_t *list, wordnode_t *new) {
    new->next = list;
    return new;
}


wordnode_t *add_end(wordnode_t *list, wordnode_t *new) {
    wordnode_t *curr;

    if (list == NULL) {
        new->next = NULL;
        return new;
    }

    for (curr = list; curr->next != NULL; curr = curr->next);
    curr->next = new;
    new->next = NULL;
    return list;
}


wordnode_t *peek_front(wordnode_t *list) {
    return list;
}


wordnode_t *remove_front(wordnode_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->next;
}


void free_word(wordnode_t *word) {
    assert(word != NULL);

    if (word->w != NULL) {
        free(word->w);
    }
    free(word);
}


void apply(wordnode_t *list,
           void (*fn)(wordnode_t *list, void *),
           void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}
