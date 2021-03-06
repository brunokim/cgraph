#ifndef _SET_H
#define _SET_H

#include <stdio.h>
#include <stdbool.h>
#include "error.h"

typedef struct set_entry_t set_entry_t;
struct set_entry_t{
	int key;
	set_entry_t *next;
};

typedef struct set_t set_t;

/**** Allocation and deallocation ****/
set_t *new_set(int minimum);
void delete_set(set_t *set);

/**** Insertion and retrieval ****/
error_t set_put(set_t *set, int v);

bool set_contains(const set_t *set, int v);
int set_get(const set_t *set, int pos);
int set_index(const set_t *set, int v);

/**** Random picking ****/
int set_get_random(const set_t *set);
int set_get_random_r(const set_t *set, unsigned int *seedp);

/**** Removing ****/
bool set_remove(set_t *set, int v);
void set_clean(set_t *set);

/**** Set operations ****/
error_t set_union(set_t *dest, const set_t *other);
void set_difference(set_t *dest, const set_t *other);
void set_intersection(set_t *dest, const set_t *other);

/**** Data structure querying ****/
int set_size(const set_t *set);
int set_table_size(const set_t *set);
set_entry_t *set_head(const set_t *set);

/**** Optimize linked list ****/
void set_optimize(set_t *set);

/**** Printing ****/
void set_print(const set_t *set);
void set_fprint(FILE *stream, const set_t *set);

/**** Copying ****/
set_t *set_copy(const set_t *set);
void set_to_array(const set_t *set, int *arr);
int* set_to_dynamic_array(const set_t *set, int *n);

#endif
