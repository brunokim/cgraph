
#include <stdlib.h>
#include "sorting.h"

void * linsearch(
		const void *key, const void *base, size_t num, size_t size, 
		comparator_fn_t compar){

	int i;
	for (i=0; i < num; i++){
		if (compar(key, (char*)base+i*size) == 0){
			return (char*)base+i*size;
		}
	}
	return NULL;
}

void * search_max
	(const void *base, size_t n, size_t size, comparator_fn_t compar)
{
	int max = 0;
	int i;
	for (i=1; i < n; i++){
		if (compar((char*)base+max*size, (char*)base+i*size) < 0){
			max = i;
		} 
	}
	return (char*)base+max*size;
}

void * search_min
	(const void *base, size_t n, size_t size, comparator_fn_t compar)	
{
	int min = 0;
	int i;
	for (i=1; i < n; i++){
		if (compar((char*)base+min*size, (char*)base+i*size) > 0){
			min = i;
		} 
	}
	return (char*)base+min*size;
}

int comp_int_asc(const void *p_i1, const void *p_i2){
	int i1 = *(int *)p_i1;
	int i2 = *(int *)p_i2;
	
	if (i1 < i2){ return -1; }
	if (i1 > i2){ return +1; }
	return 0;
}

int comp_int_desc(const void *p_i1, const void *p_i2){
	int i1 = *(int *)p_i1;
	int i2 = *(int *)p_i2;
	
	if (i1 > i2){ return -1; }
	if (i1 < i2){ return +1; }
	return 0;
}

int comp_double_asc(const void *p_d1, const void *p_d2){
	double d1 = *(double *)p_d1;
	double d2 = *(double *)p_d2;
	
	if (d1 < d2){ return -1; }
	if (d1 > d2){ return +1; }
	return 0;
}

int comp_double_desc(const void *p_d1, const void *p_d2){
	double d1 = *(double *)p_d1;
	double d2 = *(double *)p_d2;
	
	if (d1 > d2){ return -1; }
	if (d1 < d2){ return +1; }
	return 0;
}
