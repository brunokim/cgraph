#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
#include "error.h"

void test_basic(){
	list_t *list = new_list(0);
	list_put(list, 0, 42);
	assert(list_get(list, 0) == 42);
	
	list_put(list, 0, 41);
	list_put(list, 0, 40);
	list_put(list, 0, 39);
	assert(list_get(list, 0) == 39);
	assert(list_get(list, 1) == 40);
	assert(list_get(list, 2) == 41);
	assert(list_get(list, 3) == 42);
	
	int i;
	for (i=0; i <= 8; i++){
		list_put(list, i, 30+i);
	}
	for (i=0; i <= 12; i++){
		assert(list_get(list, i) == 30+i);
	}	
	delete_list(list);
}

void test_random(){
	int n = 1024;
	list_t *unsorted = new_list(n);
	list_t *sorted = new_list(n);
	
	int i;
	for (i=0; i < n; i++){
		int r = rand() % n;
		list_push(unsorted, r);
		list_insert(sorted, r);
	}
	
	int size_sorted = list_size(sorted);
	int size_unsorted = list_size(unsorted);
	assert(size_sorted == n);
	assert(size_sorted == size_unsorted);
	
	list_sort(unsorted);
	for (i=0; i < n; i++){
		int p = list_get(sorted, i);
		int q = list_get(sorted, i);
		assert(p == q);
	}
	
	//list_print(sorted); printf("\n");
	delete_list(sorted);
	delete_list(unsorted);
}

void test_sorted(){
	list_t *list = new_list(2);
	list_insert(list, 9); // (                9)
	list_insert(list, 5); // (        5       9)
	list_insert(list, 2); // (  2     5       9)
	list_insert(list, 3); // (  2 3   5       9)
	list_insert(list, 7); // (  2 3   5   7   9)
	list_insert(list, 4); // (  2 3 4 5   7   9)
	list_insert(list, 8); // (  2 3 4 5   7 8 9)
	list_insert(list, 6); // (  2 3 4 5 6 7 8 9)
	list_insert(list, 1); // (1 2 3 4 5 6 7 8 9)
	
	int i;
	for (i=0; i < 9; i++){ 
		assert(i+1 == list_get(list, i));
	}
	delete_list(list);
}

void test_redimension(){
	list_t *list = new_list(10);
	
	int i, j;
	for (i=0; i < 10; i++){
		for (j=0; j < 100; j++){
			list_push(list, j);
		}
		for (j=0; j < 90; j++){
			list_pop(list);
		}
	}
	
	for (i=0; i < 10; i++){
		for (j=0; j < 100; j++){
			list_put(list, 100-j, j);
		}
		for (j=0; j < 90; j++){
			list_get(list, 100-j);
		}
	}
	delete_list(list);
}

int main(){
	srand(42);
	test_basic();
	test_random();
	test_sorted();
	test_redimension();
	printf("success\n");
	return 0;
}
