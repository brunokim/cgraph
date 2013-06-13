#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "error.h"
#include "stat.h"
#include "graph.h"
#include "graph_model.h"

// Obtained at http://stackoverflow.com/a/4003287/946814
int mod(int a, int b){
	if (b < 0){ return mod(-a, -b); }
	int ret = a % b;
	if (ret < 0){ ret += b; }
	return ret;
}

graph_t *new_clique(int n){
	assert(n > 0);
	
	graph_t *g = new_graph(n, false, false);
	if (!g){ return NULL; }
	
	int i, j;
	for (i=0; i < n; i++){
		for (j=i+1; j < n; j++){
			error_t err = graph_add_edge(g, i, j);
			if (err){ delete_graph(g); return NULL; }
		}
	}
	
	return g;
}

graph_t *new_erdos_renyi(int n, double k){
	return new_erdos_renyi_r(n, k, NULL);
}

graph_t *new_erdos_renyi_r(int n, double k, unsigned int *seedp){
	assert(n > 0);
	assert(k > 0.0 && k < n);
	
	graph_t *g = new_graph(n, false, false);
	if (!g){ return NULL; }
	
	// r < target => rand()/RAND_MAX < p = k/n
	double target = RAND_MAX * k/n;
	
	int i, j;
	for (i=0; i < n; i++){
		for (j=i+1; j < n; j++){
			int r;
			if (seedp){ r = rand_r(seedp); }
			else      { r = rand(); }
			if (r < target){
				graph_add_edge(g, i, j);
			}
		}
	}
	
	return g;
}

graph_t *new_watts_strogatz(int n, int k, double beta){
	return new_watts_strogatz_r(n, k, beta, NULL);
}

graph_t *new_watts_strogatz_r(int n, int k, double beta, unsigned int *seedp){
	assert(n > 0);
	assert(k % 2 == 0 && k < n);
	assert(beta >= 0.0 && beta <= 1.0);
	
	graph_t *g = new_graph(n, false, false);
	if (!g){ return NULL; }
	
	double target = beta*RAND_MAX;
	
	int i, j;
	for (i=0; i < n; i++){
		for (j=0; j < k/2; j++){
			int r;
			if (seedp) { r = rand_r(seedp); }
			else       { r = rand(); }
			
			int next;
			if (r < target) // Wire randomly to a vertex not yet adjacent to i
			{
				do 
				{ 
					next = uniform(n, seedp); 
				}
				while(i == next || graph_is_adjacent(g, i, next));
			}
			else // Wire to neighbor
			{
				next = i+j+1;
			}
			
			graph_add_edge(g, i, mod(next, n));
		}
	}
	
  return g;
}

graph_t *new_barabasi_albert(int n, int k){
	return new_barabasi_albert_r(n, k, NULL);
}

graph_t *new_barabasi_albert_r(int n, int k, unsigned int *seedp){
	assert(n > 0);
	assert(k > 0 && k < n);
	
	graph_t *g = new_graph(n, false, false);
	if (!g){ return NULL; }
	
	// Creates initial clique
	int i, j;
	for (i=0; i < k+1; i++){
		for (j=i+1; j < k+1; j++){
			graph_add_edge(g, i, j);
		}
	}
	
	// degree keeps track of the actual degree of each vertex.
	int *degree = malloc(n * sizeof(*degree));
	int *d = malloc(n * sizeof(*d));
	memset(degree, 0, n * sizeof(*degree));
	for (i=0; i < k+1; i++){
		degree[i] = k;
	}
	
	int total_edges = k * (k+1);
	
	for (i=k+1; i < n; i++){
		// For each new vertex, do k rounds of a raffle algorithm to select
		// its target vertices. m is the number of tickets, and each vertex 
		// has as many tickets as its degree
		memcpy(d, degree, i * sizeof(*d));
		int m = total_edges;
		
		for (j=0; j < k; j++){
			// Selects a ticket at random and searches for the winner vertex
			int s = uniform(m, seedp);
			int v;
			for (v=0; v < i; v++){ 
				s -= d[v]; 
				if (s < 0) break;
			}
			
			// Once a vertex is selected, its tickets are taken off the bowl
			// so it wont be chosen in the next selection round 
			m -= d[v];
			d[v] = 0;
			
			graph_add_edge(g, i, v);
			degree[v]++;
		}
		
		total_edges += 2*k;
		degree[i] = k;
	}
	
	free(d);
	free(degree);
	return g;
}

typedef enum {
	GRAPH_RAVASZ_CENTRAL, GRAPH_RAVASZ_INTERMEDIATE, GRAPH_RAVASZ_PERIPHERICAL
} graph_ravasz_state_t;

void graph_ravasz_fill
		(graph_ravasz_state_t **state, int level, int initial, int k){
	int s, i;
	if (level == 0)
	{
		state[initial+0][level] = GRAPH_RAVASZ_CENTRAL;
		for (s=1; s < k; s++){
			state[initial+s][level] = GRAPH_RAVASZ_PERIPHERICAL;
		}
	}
	else
	{
		int step = (int) pow(k, level);
		for (s=0; s < k; s++){
			// Compute state in inner level
			graph_ravasz_fill(state, level-1, initial + s*step, k);
			
			// Defines current state based on previous state
			for (i=0; i < step; i++){
				int pos = initial + s*step + i;
				
				switch(state[pos][level-1]){
					case GRAPH_RAVASZ_CENTRAL: 
						state[pos][level] = 
							s != 0 ? GRAPH_RAVASZ_INTERMEDIATE : 
						           GRAPH_RAVASZ_CENTRAL;
						break;
					case GRAPH_RAVASZ_INTERMEDIATE:
						state[pos][level] = GRAPH_RAVASZ_INTERMEDIATE; 
						break;
					case GRAPH_RAVASZ_PERIPHERICAL:
						state[pos][level] = 
							s != 0 ? GRAPH_RAVASZ_PERIPHERICAL : 
							         GRAPH_RAVASZ_INTERMEDIATE;
							break;
				}
			}
		}
	}
}

graph_t *new_ravasz_barabasi(int l, int k){
	assert(l > 0);
	assert(k > 2);
	
	int i, j, n = (int) pow(k, l);
	graph_t *g = new_graph(n, false, false);
	if (!g){ return NULL; }
	
	graph_ravasz_state_t **state = malloc(n * sizeof(*state));
	state[0] = malloc(n * l * sizeof(*state[0]));
	for (i=1; i < n; i++){
		state[i] = state[0] + i*l;
	}
	
	graph_ravasz_fill(state, l-1, 0, k);
	
	// Connect centers to peripherical nodes in each level
	int level;
	for (level=1; level < l; level++){
		int center = state[0][level];
		for (i=0; i < n; i++){
			switch(state[i][level]){
				case GRAPH_RAVASZ_CENTRAL:
					center = i;
					break;
				case GRAPH_RAVASZ_PERIPHERICAL:
					graph_add_edge(g, center, i);
					break;
				case GRAPH_RAVASZ_INTERMEDIATE:
				default:
					/* Do nothing */
					break;
			}
		}
	}
	
	// Forms level 0 cliques
	int num_cliques = n/k;
	int c;
	for (c=0; c < num_cliques; c++){
		for (i=0; i < k; i++){
			for (j=i+1; j < k; j++){
				graph_add_edge(g, c*k+i, c*k+j);
			}
		}
	}
	
	free(state[0]); free(state);
	return g;
}
