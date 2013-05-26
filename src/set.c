#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "set.h"
#include "list.h"

struct set_t {
	list_t *list;
};

/**** Allocation and deallocation ****/
set_t *new_set(int minimum){
	set_t *set = malloc(sizeof(*set));
	if (!set){ return NULL; }
	
	set->list = new_list(minimum);
	if (!set->list){ free(set); return NULL; }
	
	return set;
}

void delete_set(set_t *set){
	assert(set);
	delete_list(set->list);
	free(set);
}

/**** Insertion and retrieval ****/
error_t set_put(set_t *set, int v){
	assert(set);
	if (list_find(set->list, v) < 0){
		return list_insert(set->list, v);
	}
	return ERROR_SUCCESS;
}

bool set_contains(const set_t *set, int v){
	assert(set);
	if (list_find(set->list, v) < 0){ return false; }
	else                            { return true; }
}

bool set_remove(set_t *set, int v){
	assert(set);
	if (list_remove_element(set->list, v) >= 0){ return true; }
	else                                       { return false; }
}

void set_clean(set_t *set){
	assert(set);
	list_clean(set->list);
}

/**** Set operations ****/
error_t set_union(set_t *dest, const set_t *other){
	assert(dest);
	assert(other);
	
	int i, n = list_size(other->list);
	for (i=0; i < n; i++){
		error_t error = set_put(dest, set_get(other, i));
		if (error){ return error; }
	}
	return ERROR_SUCCESS;
}

void set_difference(set_t *dest, const set_t *other){
	assert(dest);
	assert(other);
	
	int i, n = list_size(other->list);
	for (i=0; i < n; i++){
		int v = set_get(other, i);
		set_remove(dest, v);
	}
}

error_t set_intersection(set_t *dest, const set_t *other){
	assert(dest);
	assert(other);
	
	int i, n = list_size(other->list);
	for (i=0; i < n; i++){
		int v = set_get(dest, i);
		if (!set_contains(other, v)){
			error_t error = list_remove_element(dest->list, v);
			if (error){ return error; }
			i--; n--;
		}
	}
	return ERROR_SUCCESS;
}

/**** Data structure querying ****/
int set_get(const set_t *set, int pos){
	assert(set);
	assert(pos >= 0 && pos < list_size(set->list));
	return list_get(set->list, pos);
}

int set_size(const set_t *set){
	assert(set);
	return list_size(set->list);
}

int set_index(const set_t *set, int v){
	assert(set);
	return list_find(set->list, v);
}

/**** Printing ****/
void set_print(const set_t *set){
	set_fprint(stdout, set);
}

void set_fprint(FILE *stream, const set_t *set){
	assert(stream);
	assert(set);
	
	int i, n = list_size(set->list);
	fprintf(stream, "{");
	for (i=0; i < n; i++){
		fprintf(stream, "%d", list_get(set->list, i));
		if (i < n-1){ fprintf(stream, ", "); }
	}
	fprintf(stream, "}");
}

/**** Copying ****/
void set_to_array(const set_t *set, int *arr){
	assert(set);
	assert(arr);
	list_to_array(set->list, arr);
}

int* set_to_dynamic_array(const set_t *set, int *n){
	assert(set);
	return list_to_dynamic_array(set->list, n);
}
