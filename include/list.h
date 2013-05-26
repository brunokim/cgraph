#ifndef _LIST_H
#define _LIST_H

#include "error.h"
#include "sorting.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct list_t list_t;

/**** Allocation and deallocation ****/
list_t *new_list(int minimum);
void delete_list(list_t * list);

/**** Stack operations ****/
error_t list_push(list_t *list, int v);
int list_peek(const list_t *list);
int list_pop(list_t *list);

/**** Array operations ****/
error_t list_put(list_t *list, int pos, int v);
int list_get(const list_t *list, int pos);
int list_remove(list_t *list, int pos);
void list_clean(list_t *list);

/**** Sorting and searching ****/
void list_sort(list_t *list);
void list_sorting(list_t *list, comparator_fn_t compar);
int list_find(const list_t *list, int v);
int list_remove_element(list_t *list, int v);

/**** Sorted array operations ****/
error_t list_insert(list_t *list, int v);

/**** Data structure queries ****/
bool list_is_sorted(const list_t *list);
int list_size(const list_t *list);

/**** Printing ****/
void list_print(const list_t *list);
void list_fprint(FILE *stream, const list_t *list);

/**** Copying ****/
void list_to_array(const list_t *list, int *arr);
int* list_to_dynamic_array(const list_t *list, int *n);

#endif
