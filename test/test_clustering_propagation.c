#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "stat.h"
#include "graph.h"
#include "graph_model.h"
#include "graph_metric.h"
#include "graph_propagation.h"

int main(){
	int n = 10000, k = 8;
	
	double *clustering = malloc(n * sizeof(*clustering));
	
	short *init_state = malloc(n * sizeof(*init_state));
	memset(init_state, 0, n * sizeof(*init_state));
	init_state[0] = 1;
	
	int repetition = 50;
	int *time_prop = malloc(repetition * sizeof(*time_prop));
	int *num_s = malloc(repetition * sizeof(*num_s));
	
	graph_sir_params_t params;
	params.alpha = 1.0;
	params.beta = 0.5;
	
	unsigned int seed = 42;
	
	int t;
	for (t=0; t <= 100; t++){
		double beta = t/100.0;
		unsigned int seed_ws = 42;
		graph_t *ws = new_watts_strogatz_r(n, k, beta, &seed_ws);
		
		double transitivity = graph_transitivy(ws);
		
		graph_clustering(ws, clustering);
		double avg_clustering = stat_double_average(clustering, n);
		double sd_clustering = sqrt(stat_double_variance(clustering, n));
		
		int r;
		for (r=0; r < repetition; r++){
			propagation_step_t *step =
				graph_propagation_r(ws, init_state, &time_prop[r], sir, &params, &seed);
			num_s[r] = 
				graph_count_state(GRAPH_SIR_S, step[ time_prop[r]-1 ].state, n);
			delete_propagation_steps(step, time_prop[r]);
		}
		double avg_time = stat_int_average(time_prop, repetition);
		double sd_time = stat_int_average(time_prop, repetition);
		double avg_s = stat_int_average(num_s, repetition)/n;
		double sd_s = sqrt(stat_int_variance(num_s, repetition))/n;
		
		delete_graph(ws);
		
		printf("%lf %lf %lf %lf %lf %lf %lf %lf\n", 
		  beta, transitivity, 
		  avg_clustering, sd_clustering, 
		  avg_s, sd_s,
		  avg_time, sd_time);
	}
	return 0;
}
