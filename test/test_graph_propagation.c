#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "graph.h"
#include "graph_model.h"
#include "graph_propagation.h"

void test_animate(int n, propagation_model_t model, void *params){
	int i, j, k = 4;
	int width = 1, radius = 5;
	unsigned int seed = 42;
	
	graph_t *g = new_barabasi_albert_r(n, k, &seed);
	coord_t *p = malloc(n * sizeof(*p));
	srand(42);
	graph_layout_degree(g, radius+width, p);
	
	short *state = malloc(n * sizeof(*state));
	memset(state, 0, n * sizeof(*state));
	state[0] = model.infectious_state;
	
	int num_step;
	propagation_step_t *step = 
		graph_propagation(g, state, &num_step, model, params);
	
	char str[256];
	sprintf(str, "test/animate_%s", model.name);
	graph_animate_propagation
		(str, g, p, model.num_state, step, num_step);
	
	int **freq = malloc (num_step * sizeof(*freq));
	freq[0] = malloc (num_step * model.num_state * sizeof(*freq[0]));
	for (i=1; i < num_step; i++)
		freq[i] = freq[0] + i * model.num_state;
	graph_propagation_freq(step, num_step, freq, model.num_state);
	
	sprintf(str, "test/animate_%s/freq.dat", model.name);
	FILE *fp = fopen(str, "wt");
	for (i=0; i < num_step; i++){
		fprintf(fp, "%d ", i);
		for (j=0; j < model.num_state; j++){
			fprintf(fp, "%d ", freq[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	
	free(freq[0]); free(freq);
	free(p);
	free(state);
	delete_propagation_steps(step, num_step);
	delete_graph(g);
}

void test_animate_si(){
	graph_si_params_t params;
	params.alpha = 1.0;
	test_animate(64, si, &params);
}

void test_animate_sis(){
	graph_sis_params_t params;
	params.alpha = 1.0;
	params.beta  = 0.5;
	test_animate(64, sis, &params);
}

void test_animate_sir(){
	graph_sir_params_t params;
	params.alpha = 1.0;
	params.beta  = 0.5;
	test_animate(64, sir, &params);
}

void test_animate_seir(){
	graph_seir_params_t params;
	params.alpha = 1.0;
	params.beta  = 0.5;
	params.gamma = 0.5;
	test_animate(64, seir, &params);
}

int main(){
	test_animate_si();
	test_animate_sis();
	test_animate_sir();
	test_animate_seir();
	printf("success\n");
	return 0;
}
