#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sorting.h"
#include "stat.h"

int my_rand_r(unsigned int *seedp){
	return seedp ? rand_r(seedp) : rand();
}

int uniform(int n, unsigned int *seedp){
	int r;
	int rem = RAND_MAX % n;
	do {
		r = my_rand_r(seedp);
	} while(r > RAND_MAX - rem);
	return r % n;
}

int comp_key_asc(const void *p_p1, const void *p_p2){
	pair_t p1 = *(pair_t *)p_p1;
	pair_t p2 = *(pair_t *)p_p2;
	
	if (p1.key < p2.key){ return -1; }
	if (p1.key > p2.key){ return +1; }
	return 0;
}

int comp_key_desc(const void *p_p1, const void *p_p2){
	pair_t p1 = *(pair_t *)p_p1;
	pair_t p2 = *(pair_t *)p_p2;
	
	if (p1.key > p2.key){ return -1; }
	if (p1.key < p2.key){ return +1; }
	return 0;
}

int comp_value_asc(const void *p_p1, const void *p_p2){
	pair_t p1 = *(pair_t *)p_p1;
	pair_t p2 = *(pair_t *)p_p2;
	
	if (p1.value < p2.value){ return -1; }
	if (p1.value > p2.value){ return +1; }
	return 0;
}

int comp_value_desc(const void *p_p1, const void *p_p2){
	pair_t p1 = *(pair_t *)p_p1;
	pair_t p2 = *(pair_t *)p_p2;
	
	if (p1.value > p2.value){ return -1; }
	if (p1.value < p2.value){ return +1; }
	return 0;
}

double stat_int_sum(const int *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = 0.0;
	int i;
	for (i=0; i < n; i++){
		s += (double)v[i];
	}
	return s;
}

double stat_int_dist_sum(const int *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = 0.0;
	int i;
	for (i=1; i < n; i++){
		s += (double)v[i]*i;
	}
	return s;
}

double stat_int_dist_harmonic_sum(const int *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = 0.0;
	int i;
	for (i=1; i < n; i++){
		s += v[i] * (1.0/i);
	}
	return s;
}

double stat_int_average(const int *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = stat_int_sum(v, n);
	return (s / n);
}

double stat_int_variance(const int *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = 0.0;
	double m = stat_int_average(v, n);
	int i;
	for (i=0; i < n; i++){
		s += (v[i] - m)*(v[i] - m)/(n - 1);
	}
	return s;
}

double stat_int_entropy(const int *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = stat_int_sum(v, n);
	double l = 0.0;
	int i;
	for (i=0; i < n; i++){
		if (v[i] > 0){
			double p = v[i]/s;
			l += -p * log(p)/log(2.0);
		}
	}
	
	return l;
}

double stat_int_dist_average(const int *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = stat_int_dist_sum(v, n);
	double total = stat_int_sum(v, n);
	
	return (s / total);
}

double stat_double_sum(const double *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = 0.0;
	int i;
	for (i=0; i < n; i++){
		s += v[i];
	}
	return s;
}

double stat_double_average(const double *v, int n){
	assert(v);
	assert(n > 0);
	
	double s = 0.0;
	int i;
	for (i=0; i < n; i++){
		s += v[i]/n;
	}
	return s;
}

void stat_double_normalization(double *v, int n){
	assert(v);
	assert(n > 0);
	double *p_max = search_max(v, n, sizeof(*v), comp_double_asc);
	double *p_min = search_min(v, n, sizeof(*v), comp_double_asc);
	
	double max = *p_max, min = *p_min;
	if (max - min > 1e-8){
		int i;
		for (i=0; i < n; i++){
			v[i] = (v[i] - min)/(max - min);
		}
	}
}

int *stat_sort_copy(const int *v, int n){
	assert(v);
	assert(n > 0);
	
	int *copy = malloc(n * sizeof(*copy)); 
	if (!copy){ return NULL; }
	
	memcpy(copy, v, n * sizeof(*copy));
	qsort(copy, n, sizeof(*copy), comp_int_asc);
	
	return copy;
}

pair_t *stat_frequencies(const int *v, int n, int *_num_keys){
	assert(v);
	assert(n > 0);
	
	int *copy = stat_sort_copy(v, n);
	
	int i, count = 1, num_keys = 1;
	pair_t *freq = malloc(num_keys * sizeof(*freq));
	
	for (i=1; i < n; i++){
		if (copy[i] == copy[i-1])
		{
			count++;
		}
		else
		{
			freq[num_keys-1].key   = copy[i-1];
			freq[num_keys-1].value = count;
			
			count = 1;
			num_keys++;
			
			pair_t *new_freq = realloc(freq, num_keys * sizeof(*freq));
			if (!new_freq){ free(freq); free(copy); return NULL; }
			freq = new_freq;
		}
	}
	freq[num_keys-1].key   = copy[n-1];
	freq[num_keys-1].value = count;
			
	free(copy);
	
	if (_num_keys){ *_num_keys = num_keys; }
	return freq;
}

interval_t *stat_histogram(const double *_v, int n, int num_bins){
	assert(_v);
	assert(n > 1);
	assert(num_bins > 0);
	
	double *v = malloc(n * sizeof(*v));
	memcpy(v, _v, n * sizeof(*v));
	qsort(v, n, sizeof(*v), comp_double_asc);
	
	double max = v[n-1], min = v[0];
	double range = max - min;
	double delta = range/num_bins;
	
	interval_t *interval = malloc (num_bins * sizeof(*interval));
	
	int i, pos = 0;
	for (i=0; i < num_bins; i++){
		interval[i].min = min + i*delta;
		interval[i].max = min + (i+1)*delta;
		interval[i].value = 0;
		
		while (v[pos] < interval[i].max)
		{
			interval[i].value++;
			pos++;
		}
	}
	interval[num_bins-1].value++;
	
	free(v);
	return interval;
}
