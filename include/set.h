#ifndef _SET_H
#define _SET_H

#include <stdio.h>
#include <stdbool.h>
#include "error.h"

typedef struct set_t set_t;

/**** Allocation and deallocation ****/
set_t *new_set(int minimum);
void delete_set(set_t *set);

/**** Insertion and retrieval ****/
error_t set_put(set_t *set, int v);
bool set_contains(const set_t *set, int v);
bool set_remove(set_t *set, int v);
void set_clean(set_t *set);

/**** Set operations ****/
error_t set_union(set_t *dest, const set_t *other);
void     set_difference(set_t *dest, const set_t *other);
error_t set_intersection(set_t *dest, const set_t *other);

/**** Data structure querying ****/
int set_get(const set_t *set, int pos);
int set_index(const set_t *set, int v);
int set_size(const set_t *set);

/**** Printing ****/
void set_print(const set_t *set);
void set_fprint(FILE *stream, const set_t *set);

/**** Copying ****/
void set_to_array(const set_t *set, int *arr);
int* set_to_dynamic_array(const set_t *set, int *n);

#endif
