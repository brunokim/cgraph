#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>

#include "stat.h"
#include "sorting.h"
#include "error.h"
#include "set.h"

#ifndef SET_UTILIZATION_RATE
	#define SET_UTILIZATION_RATE 0.75
#endif

uint64_t set_primes[] = {
	2uL, 3uL, 7uL, 13uL, 23uL, 47uL, 97uL, 193uL, 383uL, 769uL, 1531uL, 3067uL, 
	6143uL, 12289uL, 24571uL, 49157uL, 98299uL, 196613uL, 393209uL, 786433uL, 
	1572869uL, 3145721uL, 6291449uL, 12582917uL, 25165813uL, 50331653uL, 
	100663291uL, 201326611uL, 402653189uL, 	805306357uL, 1610612741uL, 
	3221225473uL, 6442450939uL, 12884901893uL, 25769803799uL, 51539607551uL, 
	103079215111uL, 206158430209uL, 412316860441uL, 824633720831uL, 
	1649267441651uL, 3298534883309uL, 6597069766657uL, 13194139533299uL, 
	26388279066623uL, 52776558133303uL, 105553116266489uL, 211106232532969uL, 
	422212465066001uL, 844424930131963uL, 1688849860263953uL, 3377699720527861uL, 
	6755399441055731uL, 13510798882111483uL, 27021597764222939uL, 
	54043195528445957uL, 108086391056891903uL, 216172782113783843uL, 
	432345564227567621uL, 864691128455135207uL, 1729382256910270481uL, 
	3458764513820540933uL, 6917529027641081903uL, 13835058055282163729uL
};

const int num_primes = sizeof(set_primes)/sizeof(set_primes[0]);

uint64_t set_primes_sqrt[] = {
	2, 2, 3, 4, 5, 7, 10, 14, 20, 28, 40, 56, 79, 111, 157, 222, 314, 444, 628, 
	887, 1255, 1774, 2509, 3548, 5017, 7095, 10034, 14189, 20067, 28378, 40133, 
	56756, 80265, 113512, 160530, 227024, 321060, 454047, 642120, 908094, 1284239,
	1816187, 2568477, 3632374, 5136953, 7264748, 10273905, 14529496, 20547810, 
	29058991, 41095619, 58117982, 82191238, 116235963, 164382475, 232471925, 
	328764949, 464943849, 657529897, 929887697, 1315059793, 1859775394, 
	2630119585u, 3719550787u
};

struct set_t {
	int size_idx;
	int n;
	
	set_entry_t *entry;
	set_entry_t *head, *tail;
};

/**** Allocation and deallocation ****/
set_t *new_set(int minimum){
	if (minimum < 8){
		minimum = 8;
	}
	set_t *set = malloc(sizeof(*set));
	if (!set){ return NULL; }
	
	int preferred_size = (int) ceil(minimum/SET_UTILIZATION_RATE);
	int i;
	for (i=0; i < num_primes-1; i++){
		if (set_primes[i] >=	preferred_size)
			break;
	}
	set->size_idx = i;
	int size = set_primes[i];
	set->entry = malloc(size * sizeof(*set->entry));
	if (!set->entry){
		free(set); return NULL;
	}
	
	set_clean(set);
	
	return set;
}

void delete_set(set_t *set){
	assert(set);
	free(set->entry);
	free(set);
}

error_t set_realloc(set_t *set){
	int size = set_primes[set->size_idx];
	if (set->n > size * SET_UTILIZATION_RATE){
		int new_size = set_primes[set->size_idx+1];
		set_t *other = new_set(new_size);
		if (!other){ return ERROR_NO_MEMORY; }
		
		set_entry_t *p;
		for (p = set->head; p != NULL; p = p->next){
			set_put(other, p->key);
		}
		
		free(set->entry);
		set->size_idx = other->size_idx;
		set->n        = other->n;
		set->entry    = other->entry;
		set->head     = other->head;
		set->tail     = other->tail;
		free(other);
	}
	return ERROR_SUCCESS;
}

/**** Insertion and retrieval ****/

int set_hash(int key, int size){
	return (31*key) % size;
}

int set_locate(const set_t *set, int key){
	int size = set_primes[set->size_idx];
	int pos = set_hash(key, size);
	while (set->entry[pos].key != key && 
	       set->entry[pos].key >= 0){
		pos = (pos + 1) % size;             // Linear probing collision resolution
	}
	return pos;
}
	
error_t set_put(set_t *set, int key){
	assert(set);
	assert(key >= 0);
	
	int pos = set_locate(set, key);
	set_entry_t *addr = &set->entry[pos];
	
	if (set->n == 0)
	{
		set->head = set->tail = addr;
		set->n++;
	}
	else if (addr->key < 0)
	{
		set->tail->next = addr;
		set->tail = addr;
		set->n++;
	}
	
	set->entry[pos].key = key;
	
	return set_realloc(set);
}

bool set_contains(const set_t *set, int key){
	assert(set);
	assert(key >= 0);
	int pos = set_locate(set, key);
	
	if (set->entry[pos].key < 0){ return false; }
	else                        { return true; }
}

bool set_remove(set_t *set, int key){
	assert(set);
	assert(key >= 0);
	
	// Constant time checking before walking the linked list
	if (!set_contains(set, key)){ return false; }
	
	if (set->head->key == key){
		if (set->tail->key == key){ set->tail = NULL; }
		set->head->key = -1;
		set->head = set->head->next;
	}
	
	set_entry_t *p;
	for (p = set->head; p != set->tail; p = p->next){
		if (p->next->key == key){
			if (set->tail->key == key){ set->tail = p; }
			p->next->key = -1;
			p->next = p->next->next;
			break;
		}
	}
	
	set->n--;
	return true;
}

void set_clean(set_t *set){
	assert(set);
	
	uint64_t i, size = set_primes[set->size_idx];
	for (i=0; i < size; i++){
		set->entry[i].key = -1;
		set->entry[i].next = NULL;
	}
	set->head = set->tail = NULL;
	
	set->n = 0;
}

/**** Set operations ****/
error_t set_union(set_t *dest, const set_t *other){
	assert(dest);
	assert(other);
	
	set_entry_t *p;
	for (p=other->head; p != NULL; p = p->next){
		error_t error = set_put(dest, p->key);
		if (error){ return error; }
	}
	return ERROR_SUCCESS;
}

void set_difference(set_t *dest, const set_t *other){
	assert(dest);
	assert(other);
	
	set_entry_t *p;
	for (p=other->head; p != NULL; p = p->next){
		set_remove(dest, p->key);
	}
}

void set_intersection(set_t *dest, const set_t *other){
	assert(dest);
	assert(other);
	
	// Iterates over all elements in dest, except the head, checking if they
	//are present in other
	set_entry_t *p;
	for (p=dest->head; p->next != NULL;){
		if (!set_contains(other, p->next->key))
		{
			set_entry_t *q = p->next;
			p->next = q->next;
			q->key = -1;
			q->next = NULL;
			dest->n--;
		}
		else
		{
			p = p->next;
		}
	}
	dest->tail = p;
	
	// Treat the head case
	if (!set_contains(other, dest->head->key)){
		set_entry_t *p = dest->head;
		dest->head = p->next;
		p->key = -1;
		p->next = NULL;
		dest->n--;
		if (dest->n == 0){
			dest->tail = NULL;
		}
	}
}

/**** Data structure querying ****/

int set_get(const set_t *set, int pos){
	assert(set);
	assert(pos >= 0 && pos < set->n);
	
	int i;
	set_entry_t *p = set->head;
	for (i=0; i < pos; i++){
		p = p->next;
	}
	
	return p->key;
}

int set_size(const set_t *set){
	assert(set);
	return set->n;
}

int set_table_size(const set_t *set){
	assert(set);
	return set_primes[set->size_idx];
}

int set_index(const set_t *set, int key){
	assert(set);
	assert(key >= 0);
	
	if (!set_contains(set, key)){ return -1; }
	
	int i;
	set_entry_t *p = set->head;
	for (i=0; i < set->n; i++){
		if (p->key == key) { break; }
		p = p->next;
	}
	
	return i;
}

set_entry_t *set_head(const set_t *set){
	return set->head;
}

// Let N be the number of elements in the table, and P the size of the table.
//The utilization rate r is given as r = N/P.
//
// The probability to choose a non-empty slot uniformly at the first try is r; 
//in the second try is r*(1-r); in the third try is r*(1-r)^2 and so on.
// The expected number of tries k is 1*r + 2*r*(1-r) + 3*r*(1-r)^2 + ...
// k = r * sum_{n=1}^\infty {n*(1-r)^(n-1)} = r * (1/r^2) = 1/r
// k = P/N
//
// We can also pick a random number by selecting a number between 0 and N-1 and
//walking the linked list. The expected number of steps s is N/2. 
// Let a be the time per random try, and b be the time of a step. We should
//use random picking if
// a * P/N < b * N/2 => (2a/b) * P < N^2 => N > sqrt(2a/b * P)
// For practical reasons we use a/b = 8, so we have N > 4*sqrt(P)
int set_get_random(const set_t *set){
	return set_get_random_r(set, NULL);
}

int set_get_random_r(const set_t *set, unsigned int *seedp){
	assert(set);
	if (set->n == 0){ return -1; }
	
	// Random picking
	if (set->n > 4*set_primes_sqrt[set->size_idx])
	{
		int size = set_primes[set->size_idx];
		int pos;
		do {
			pos = uniform(size, seedp);
		} while (set->entry[pos].key < 0);
		
		return set->entry[pos].key;
	}
	
	// List traversal
	return set_get(set, uniform(set->n, seedp));
}

/**** Optimize linked list ****/

// Organize links to point to next element in succession at the entry table,
//hopefully improving memory locality
void set_optimize(set_t *set){
	assert(set);
	if (set->n < 2){ return; }
	
	set_entry_t *curr = set->entry, *prev = NULL;
	while(curr->key < 0){ curr++; }
	set->head = prev = curr;
	curr++;
	
	int i;
	for (i=1; i < set->n; i++){
		while(curr->key < 0){ curr++; }
		prev->next = curr;
		prev = curr;
		curr++;
	}
	prev->next = NULL;
	set->tail = prev;
}

/**** Printing ****/
void set_print(const set_t *set){
	set_fprint(stdout, set);
}

void set_fprint(FILE *stream, const set_t *set){
	assert(stream);
	assert(set);
	
	int i, n = set->n;
	set_entry_t *p = set->head;
	fprintf(stream, "{");
	for (i=0; i < n; i++){
		fprintf(stream, "%d", p->key);
		p = p->next;
		if (i < n-1){ fprintf(stream, ", "); }
	}
	fprintf(stream, "}");
}

/**** Copying ****/
set_t *set_copy(const set_t *set){
	assert(set);
	
	set_t *copy = new_set(set->n);
	if (!copy){ return NULL; }
	
	set_entry_t *p;
	for (p=set->head; p != NULL; p = p->next){
		set_put(copy, p->key);
	}
	
	set_optimize(copy);
	return copy;
}

void set_to_array(const set_t *set, int *arr){
	assert(set);
	assert(arr);
	int i;
	set_entry_t *p = set->head;
	for (i=0; i < set->n; i++){
		arr[i] = p->key;
		p = p->next;
	}
}

int* set_to_dynamic_array(const set_t *set, int *n){
	assert(set);
	*n = set->n;
	int *arr = malloc(*n * sizeof(*arr));
	set_to_array(set, arr);
	return arr;
}
