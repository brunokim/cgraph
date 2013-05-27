#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "graph.h"
#include "graph_model.h"
#include "graph_propagation.h"



void test_animate_si(){
	int n = 64, k = 4;
	int width = 1, radius = 5;
	unsigned int seed = 42;
	
	graph_t *g = new_barabasi_albert_r(n, k, &seed);
	coord_t *p = malloc(n * sizeof(*p));
	srand(42);
	graph_layout_degree(g, radius+width, p);
	
	short *state = malloc(n * sizeof(*state));
	memset(state, GRAPH_SI_S, n * sizeof(*state));
	state[0] = GRAPH_SI_I;
	
	graph_si_params_t params;
	params.alpha = 0.5;
	
	int num_step;
	propagation_step_t *step = graph_propagation(g, state, &num_step,si,&params);
	
	graph_animate_propagation
		("test/animate_si", g, p, GRAPH_SI_NUM_STATE, step, num_step);
	free(p);
	free(state);
	delete_propagation_steps(step, num_step);
	delete_graph(g);
}

void test_animate_sis(){
	int n = 64, k = 4;
	int width = 1, radius = 5;
	unsigned int seed = 42;
	
	graph_t *g = new_barabasi_albert_r(n, k, &seed);
	coord_t *p = malloc(n * sizeof(*p));
	srand(42);
	graph_layout_degree(g, radius+width, p);
	
	short *state = malloc(n * sizeof(*state));
	memset(state, GRAPH_SIS_S, n * sizeof(*state));
	state[0] = GRAPH_SIS_I;
	
	graph_sis_params_t params;
	params.alpha = 1.0;
	params.beta = 0.5;
	params.num_iter = 20;
	
	int num_step;
	propagation_step_t *step = graph_propagation(g, state, &num_step,sis,&params);
	
	graph_animate_propagation
		("test/animate_sis", g, p, GRAPH_SIS_NUM_STATE, step, num_step);
	free(p);
	free(state);
	delete_propagation_steps(step, num_step);
	delete_graph(g);
}


int main(){
	test_animate_si();
	test_animate_sis();
	printf("success\n");
	return 0;
}
