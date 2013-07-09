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

void graph_fast_newman(graph_t *g, int *community){
	assert(g);
	assert(community);
	
	int i, n = graph_num_vertices(g);
	int t = 0;
	
	
}
