#ifndef _SORTING_H
#define _SORTING_H

typedef int (*comparator_fn_t)(const void*, const void*);

typedef void* (*search_f)
	(const void *key, const void *base, size_t n, size_t size, 
	comparator_fn_t compar);

void * linsearch
 (const void *key, const void *base, size_t n, size_t size, 
	comparator_fn_t compar);
	
void * search_max
	(const void *base, size_t n, size_t size, comparator_fn_t compar);	
void * search_min
	(const void *base, size_t n, size_t size, comparator_fn_t compar);	

// Comparison functions
int comp_int_asc(const void *p_i1, const void *p_i2);
int comp_int_desc(const void *p_i1, const void *p_i2);

int comp_double_asc(const void *p_d1, const void *p_d2);
int comp_double_desc(const void *p_d1, const void *p_d2);

#endif
