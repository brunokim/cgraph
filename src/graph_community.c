#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "stat.h"
#include "set.h"
#include "graph.h"
#include "graph_community.h"

int **graph_community_matrix(graph_t *g, int *community, set_t *mapping){
	assert(g);
	assert(community);
	assert(mapping);
	
	int i, j, n = graph_num_vertices(g);
	
	set_clean(mapping);
	for (i=0; i < n; i++){
		set_put(mapping, community[i]);
	}
	int num_comm = set_size(mapping);
	
	// e[i][j] = number of edges between communities i and j
	int **e = malloc(num_comm * sizeof(*e));
	e[0] = malloc(num_comm * num_comm * sizeof(*e[0]));
	for (i=1; i < num_comm; i++){
		e[i] = e[0] + i*num_comm;
	}
	memset(e[0], 0, num_comm * num_comm * sizeof(*e[0]));
	
	// Iterates edges counting numbers of edges inside and outside 
	int *adj = malloc(n * sizeof(*adj));
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			
			int ci = set_index(mapping, community[i]);
			int cv = set_index(mapping, community[v]);
			
			e[ci][cv]++;
		}
	}
	
	for (i=0; i < num_comm; i++){
		e[i][i] /= 2;
	}
	
	return e;
}

double graph_modularity(graph_t *g, int *community){
	assert(g);
	assert(community);
	
	int i, j, m = graph_num_edges(g);
	
	set_t *mapping = new_set(0);
	int **e = graph_community_matrix(g, community, mapping);
	int num_comm = set_size(mapping);
	
	// a[i] = number of edges in community i
	int *a = malloc(num_comm * sizeof(*a));
	
	for (i=0; i < num_comm; i++){
		a[i] = 0;
		for (j=0; j < num_comm; j++){
			a[i] += e[i][j];
		}
	}
	
	double modularity = 0.0;
	for (i=0; i < num_comm; i++){
		double inside = (double)e[i][i]/m;
		double total = (double)a[i]/m;
		modularity += inside - total * total;
	}
	
	free(a);
	free(e[0]); free(e);
	delete_set(mapping);
	
	return modularity;
}

typedef struct {
	graph_t *community;
	int *inner;
	set_t **vertices;
} graph_newman_step_t;

graph_newman_step_t new_graph_newman_step(int n){
	graph_newman_step_t step;
	
	bool is_weighted = true;
	bool is_directed = false;
	step.community = new_graph(n, is_weighted, is_directed);
	
	step.inner = malloc(n * sizeof(*step.inner));
	memset(step.inner, 0, n * sizeof(*step.inner));
	
	step.vertices = malloc(n * sizeof(*step.vertices));
	int i;
	for (i=0; i < n; i++){
		step.vertices[i] = new_set(1);
	}
	
	return step;
}

void graph_fast_newman(graph_t *g, int *community){
	assert(g);
	assert(community);
	
	int i, j, n = graph_num_vertices(g), m = graph_num_edges(g);
	int *adj = malloc(n * sizeof(*adj));
	
	int t = 0;
	graph_newman_step_t *step = malloc((n-1) * sizeof(*step));
	step[t] = new_graph_newman_step(n);
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			graph_add_weighted_edge(step[t].community[i], i, v, 1.0);
		}
		
		step[t].inner[i] = 0;
		set_put(step[t].vertices[i], i);
	}
	
	for (t=1; t < n; t++){
		int num_comm = graph_num_vertices(step[t-1].community);
		double max_increment = -1.0/0.0;
		int merged[2];
		for (i=0; i < num_comm; i++){
			int ki = graph_adjacents(step[t-1].community, i, adj);
			for (j=0; j < ki; j++){
				int v = adj[j];
				int kv = graph_num_adjacents(step[t-1].community, v);
				
				double M = (double) m;
				double between = graph_get(step[t-1].community, i, v)/M;
				double increment = between - (ki/M)*(kv/M);
				
				if (increment > max_increment){
					max_increment = increment;
					merged[0] = i; merged[1] = v;
				}
			}
			
		}
		
		
		step[t] = new_graph_newman_step(num_comm-1);
	}
}
