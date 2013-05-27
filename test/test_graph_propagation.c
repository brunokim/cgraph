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
	
	graph_t *g = new_barabasi_albert(n, k);
	coord_t *p = malloc(n * sizeof(*p));
	graph_layout_degree(g, radius+width, p);
	
	short *state = malloc(n * sizeof(*state));
	memset(state, GRAPH_SI_S, n * sizeof(*state));
	state[0] = GRAPH_SI_I;
	int num_step;
	propagation_step_t *step = graph_propagation_si(g, state, &num_step);
	
	graph_animate_propagation
		("test/animate_si", g, p, GRAPH_SI_NUM_STATE, step, num_step);
	free(p);
	free(state);
	delete_propagation_steps(step, num_step);
	delete_graph(g);
}

int main(){
	test_animate_si();
	printf("success\n");
	return 0;
}
