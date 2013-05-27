#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "graph.h"
#include "graph_model.h"

// Obtained at http://stackoverflow.com/a/4003287/946814
int mod(int a, int b){
	if (b < 0){ return mod(-a, -b); }
	int ret = a % b;
	if (ret < 0){ ret += b; }
	return ret;
}

int uniform(int n, unsigned int *seedp){
	int r;
	int rem = RAND_MAX % n;
	do {
		r = seedp ? rand_r(seedp) : rand();
	} while(r > RAND_MAX - rem);
	return r % RAND_MAX;
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
			if (i != j){
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
	for (i=0; i < k; i++){
		for (j=i+1; j < k; j++){
			graph_add_edge(g, i, j);
		}
	}
	
	int *degree = malloc(n * sizeof(*degree));
	int *d = malloc(n * sizeof(*d));
	memset(degree, 0, n * sizeof(*degree));
	for (i=0; i < k; i++){
		degree[i] = k;
	}
	
	int total_edges = k * (k-1);
	
	for (i=k; i < n; i++){
		memcpy(d, degree, (i-1) * sizeof(*d));
		int m = total_edges;
		
		for (j=0; j < k; j++){
			int s = uniform(m, seedp);
			int v;
			for (v=0; v < k; v++){ 
				s -= d[v]; 
				if (s < 0) break;
			}
			
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
