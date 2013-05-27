#include <assert.h>
#include <stdio.h>
#include "set.h"

void test_basic(){
	set_t *set = new_set(10);
	
	int i, j;
	for (i=0; i < 10; i++){
		for (j=0; j < 10; j++){
			set_put(set, i);
		}
	}
	assert(set_size(set) == 10);
	delete_set(set);
}

void test_set_operations(){
	set_t *even1 = new_set(10);
	set_t *even2 = new_set(10);
	set_t *odd  = new_set(10);
	
	int i;
	for (i=0; i < 10; i++){
		set_put(even1, 2*i);
		set_put(even2, 2*i);
		set_put(odd, 2*i+1);
	}
	
	set_union(even1, odd);
	assert(set_size(even1) == 20);
	
	set_difference(even2, odd);
	assert(set_size(even2) == 10);
	
	set_intersection(even2, odd);
	assert(set_size(even2) == 0);
	
	// set_print(even1); printf("\n");
	// set_print(even2); printf("\n");
	// set_print(odd); printf("\n");
	
	delete_set(even1);
	delete_set(even2);
	delete_set(odd);
}

int main(){
	test_basic();
	test_set_operations();
	printf("success\n");
	return 0;
}
