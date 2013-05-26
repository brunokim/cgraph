#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "stat.h"

void test_frequencies(){
	int n = 20;
	int v[] = {3, 2, 3, 9, 6, 
	           1, 2, 3, 2, 1, 
	           9, 3, 6, 2, 1, 
	           2, 3, 9, 9, 1};
	 
	 int num_keys;
	 pair_t *freq = stat_frequencies(v, n, &num_keys);
	 
	 assert(num_keys == 5);
	 assert(freq[0].key   == 1);
	 assert(freq[0].value == 4);
	 
	 assert(freq[1].key   == 2);
	 assert(freq[1].value == 5);
	 
	 assert(freq[2].key   == 3);
	 assert(freq[2].value == 5);
	 
	 assert(freq[3].key   == 6);
	 assert(freq[3].value == 2);
	 
	 assert(freq[4].key   == 9);
	 assert(freq[4].value == 4);
	 
	 free(freq);
}

void test_histogram(){
	double v[] = {
		0.00, 0.02,
		0.10, 0.13, 0.19,
		0.21,
		
		0.41, 0.43, 0.45, 0.46, 0.47, 0.49,
		0.53, 0.58,
		0.61, 0.66, 0.68,
		
		0.80, 0.82, 0.85, 0.89,
		0.91, 0.93, 0.94, 1.00
	};
	int n = sizeof(v)/sizeof(v[0]);
	int num_bins = 10;
	
	interval_t *hist = stat_histogram(v, n, num_bins);
	assert(hist[0].value == 2);
	assert(hist[1].value == 3);
	assert(hist[2].value == 1);
	assert(hist[3].value == 0);
	assert(hist[4].value == 6);
	assert(hist[5].value == 2);
	assert(hist[6].value == 3);
	assert(hist[7].value == 0);
	assert(hist[8].value == 4);
	assert(hist[9].value == 4);
	free(hist);
}

int main(){
	test_frequencies();
	test_histogram();
	printf("success\n");
	return 0;
}
