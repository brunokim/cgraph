#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "sorting.h"
#include "list.h"

struct list_t {
	bool is_sorted;
	
	int n;
	int log_minimum;
	int log_size;
	int *arr;
};

int log_2(int i){
	int l;
	for (l=-1; i > 0; l++){
		i = i >> 1;
	}
	return l;
}

/************** Allocation and deallocation ***************************/

list_t *new_list(int minimum){
	if (minimum < 8){ minimum = 8; }
	
	list_t *list = malloc (sizeof(*list));
	if (!list) return NULL;
	list->is_sorted = true;
	
	list->n = 0;
	
	list->log_minimum = log_2(minimum);
	list->log_size    = 1 + list->log_minimum;
	int size = 1 << list->log_size;
	
	list->arr = malloc(size * sizeof(*list->arr));
	if (!list->arr){ free(list); return NULL;}
	
	return list;
}

void delete_list(list_t * list){
	assert(list);
	
	free(list->arr);
	free(list);
}

error_t list_realloc(list_t *list){
	int minimum = 1 << list->log_minimum;
	if (list->n > minimum){
		int log_n = log_2(list->n);
		
		bool is_grow   = log_n  == list->log_size;
		bool is_shrink = log_n+2 < list->log_size;
		if(is_grow || is_shrink){
			if (is_grow){ list->log_size++; }
			else        { list->log_size--; }
			int size = 1 << list->log_size;
			
			int *new_arr = realloc(list->arr, size * sizeof(*list->arr));
			if (!new_arr){ return ERROR_NO_MEMORY; }
			list->arr = new_arr;
		}
	}
	return ERROR_SUCCESS;
}

/************************ Stack operations  ***************************/
error_t list_push(list_t *list, int v){
	assert(list);
	
	list->is_sorted = false;
	
	list->arr[list->n++] = v;
	if (!list_realloc(list)){ return ERROR_NO_MEMORY; }
	return ERROR_SUCCESS;
}

int list_peek(const list_t *list){
	assert(list);
	
	return list->arr[list->n-1];
}

int list_pop(list_t *list){
	assert(list);
	
	int v = list->arr[list->n-1];
	list->n--;
	list_realloc(list);
	return v;
}

/************************ Array operations  ***************************/
error_t list_put(list_t *list, int pos, int v){
	assert(list);
	assert(pos >= 0 && pos <= list->n);
	
	list->is_sorted = false;
	
	if (list->n > 0 && pos != list->n){
		memmove(list->arr + pos+1, list->arr + pos, 
	          (list->n - pos) * sizeof(*list->arr));
	}
	list->arr[pos] = v;
	list->n++;
	
	if (!list_realloc(list)){ return ERROR_NO_MEMORY; }
	return ERROR_SUCCESS;
}

int list_get(const list_t *list, int pos){
	assert(list);
	assert(pos >= 0 && pos < list->n);
	
	return list->arr[pos];
}

int list_remove(list_t *list, int pos){
	assert(list);
	assert(pos >= 0 && pos < list->n);
	
	int v = list->arr[pos];
	memmove(list->arr + pos, list->arr + pos+1, 
	        (list->n - pos) * sizeof(*list->arr));
	list->n--;
	
	list_realloc(list);
	return v;
}

void list_clean(list_t *list){
	assert(list);
	while (list->n > 0){
		list_pop(list);
	}	
}

/********************* Sorting and searching **************************/
void list_sort(list_t *list){
	list_sorting(list, comp_int_asc);
}

void list_sorting(list_t *list, comparator_fn_t compar){
	assert(list);
	
	qsort(list->arr, list->n, sizeof(*list->arr), compar);
	list->is_sorted = true;
}

int list_find(const list_t *list, int v){
	assert(list);
	
	int *ptr;
	search_f search = list->is_sorted ? bsearch : linsearch;
	ptr = search((void *)&v, list->arr, list->n, sizeof(*list->arr), 
	             comp_int_asc);
	
	if (ptr){ return ptr - list->arr; }
	else    { return -1;              }
}

int list_remove_element(list_t *list, int v){
	assert(list);
	
	int pos = list_find(list, v);
	if (pos >= 0){
		list_remove(list, pos);
	}
	return pos;
}

/********************* Sorted array operations ************************/
error_t list_insert(list_t *list, int v){
	assert(list);
	assert(list->is_sorted);
	
	int pos;
	
	if (list->n == 0 || v <= list->arr[0])
	{ 
		pos = 0;
	} 
	else if (list->arr[list->n - 1] <= v)
	{
		pos = list->n;
	}
	else
	{	
		//Binary search, until list->arr[a] < v < list->arr[b]
		int a = 0, b = list->n;
		while (b - a > 1){
			int mid = a + (b - a)/2;
			if (list->arr[mid] < v){
				a = mid;
			} else {
				b = mid;
			}
		}
		pos = b;
	}
	
	if (!list_put(list, pos, v)){ return ERROR_NO_MEMORY; }
	list->is_sorted = true;
	return ERROR_SUCCESS;
}

/********************* Data structure queries *************************/
bool list_is_sorted(const list_t *list){
	assert(list);
	return list->is_sorted;
}

int list_size(const list_t *list){
	assert(list);
	
	return list->n;
}

/**************************** Printing ********************************/
void list_print(const list_t *list){
	list_fprint(stdout, list);
}

void list_fprint(FILE *stream, const list_t *list){
	assert(list);
	
	int i;
	
	fprintf(stream, "(");
	for (i=0; i < list->n; i++){
		fprintf(stream, "%d", list_get(list, i));
		if (i < list->n-1){ fprintf(stream, ", "); }
	}	
	fprintf(stream, ")");
}

/***************************** Copying ********************************/
void list_to_array(const list_t *list, int *arr){
	assert(list);
	assert(arr);
	
	memcpy(arr, list->arr, list->n * sizeof(*list->arr));
}

int* list_to_dynamic_array(const list_t *list, int *n){
	assert(list);
	
	int *arr = malloc(list->n * sizeof(*arr));
	list_to_array(list, arr);
	if (n){ *n = list->n; }
	return arr;
}
