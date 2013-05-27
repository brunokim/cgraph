#ifndef _STAT_H
#define _STAT_H

typedef struct {
	int key;
	int value;
} pair_t;

typedef struct {
	double min;
	double max;
	int value;
} interval_t;

int uniform(int n, unsigned int *seedp);

int comp_key_asc(const void*, const void*);
int comp_key_desc(const void*, const void*);

int comp_value_asc(const void*, const void*);
int comp_value_desc(const void*, const void*);

double stat_int_sum(const int *v, int n);
double stat_int_average(const int *v, int n);
double stat_int_variance(const int *v, int n);
double stat_int_entropy(const int *v, int n);

double stat_int_dist_average(const int *v, int n);
double stat_int_dist_sum(const int *v, int n);
double stat_int_dist_harmonic_sum(const int *v, int n);

double stat_double_sum(const double *v, int n);
double stat_double_average(const double *v, int n);

void stat_double_normalization(double *v, int n);

int *stat_sort_copy(const int *v, int n);
pair_t *stat_frequencies(const int *v, int n, int *num_keys);
interval_t *stat_histogram(const double *v, int n, int num_bins);

#endif
