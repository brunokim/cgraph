#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>	

#include "set.h"
#include "error.h"
#include "sorting.h"
#include "list.h"
#include "graph.h"

typedef struct {
	int from;
	int to;
	double weight;
} edge_t;

int comp_edge_asc(const void *p_e1, const void *p_e2){
	edge_t e1 = *(edge_t *)p_e1;
	edge_t e2 = *(edge_t *)p_e2;
	
	if (e1.from < e2.from){ return -1; }
	if (e1.from > e2.from){ return +1; }
	if (e1.to   < e2.to)  { return -1; }
	if (e1.to   > e2.to)  { return +1; }
	return 0;
}

struct graph_t {
	bool is_weighted;
	bool is_directed;
	
	int n, m;
	
	set_t **adjacencies;
	
	int size_edge;
	bool is_edges_sorted;
	edge_t *edge;
};

/****************** Allocation and deallocation ***********************/

graph_t * new_graph(int n, bool is_weighted, bool is_directed){
	if (n < 0) { return NULL; }
	graph_t *graph = malloc (sizeof(*graph));
	graph->n = n;
	
	graph->m = 0;
	graph->is_edges_sorted = false;
	
	graph->is_weighted = is_weighted;
	if (is_weighted)
	{
		graph->size_edge = 4*n;
		graph->edge = malloc(graph->size_edge * sizeof(*graph->edge));
	}
	else 
	{
		graph->size_edge = 0;
		graph->edge = NULL;
	}
	
	graph->is_directed = is_directed;
	
	graph->adjacencies = malloc(n * sizeof(*graph->adjacencies));
	int i;
	for (i=0; i < n; i++){
		graph->adjacencies[i] = new_set(0);
	}
	
	return graph;
}

void delete_graph(graph_t *graph){
	assert(graph);
	if (graph->is_weighted){
		free(graph->edge);
	}
	
	int i;
	for (i=0; i < graph->n; i++){
		delete_set(graph->adjacencies[i]);
	}
	free(graph->adjacencies);
	free(graph);
}

void graph_check(const graph_t *g, int i, int j){
	assert(g);
	assert(i >= 0 && i < g->n);
	assert(j >= 0 && j < g->n);
}

/**************************  Insertion ********************************/
error_t graph_add_edge(graph_t *g, int i, int j){
	graph_check(g, i, j);
	
	if (!set_contains(g->adjacencies[i], j)){
		error_t error_ij = 0, error_ji = 0;
		
		if (g->is_directed)
		{
			error_ij = set_put(g->adjacencies[i], j);
		}
		else
		{
			error_ij = set_put(g->adjacencies[i], j);
			error_ji = set_put(g->adjacencies[j], i);
		}
		g->m++;
		
		if (error_ij || error_ji){
			if (error_ij == ERROR_NO_MEMORY || error_ji == ERROR_NO_MEMORY)
				return ERROR_NO_MEMORY;
			return ERROR_UNDEFINED;
		}
	}
	
	return ERROR_SUCCESS;
}

error_t graph_edge_realloc(graph_t *graph){
	if (graph->m == graph->size_edge){
		int s = graph->size_edge;
		edge_t *new_arr = realloc(graph->edge, 2*s*sizeof(*graph->edge));
		if (!new_arr){ return ERROR_NO_MEMORY; }
		
		graph->edge      = new_arr;
		graph->size_edge = 2*s;
	}
	return ERROR_SUCCESS;
}

error_t graph_add_weighted_edge (graph_t *g, int i, int j, double w){
	graph_check(g, i, j);
	assert(g->is_weighted);
	
	if (!graph_is_adjacent(g, i, j)){
		error_t error = graph_add_edge(g, i, j);
		if (error) return error;
		
		g->is_edges_sorted = false;
		
		//The key must have i < j
		int min = i < j ? i : j;
		int max = i < j ? j : i;
		edge_t edge = {min, max, w};
		
		g->edge[g->m-1] = edge;
		
		error = graph_edge_realloc(g);
		if (error) return error;
	}
	return ERROR_SUCCESS;
}

void graph_sort_edges(graph_t *g){
	assert(g);
	assert(g->is_weighted);
	
	if (!g->is_edges_sorted){
		g->is_edges_sorted = true;
		qsort(g->edge, g->m, sizeof(*g->edge), comp_edge_asc);
	}
}

graph_t * load_graph(char *file_name, bool is_directed){
	FILE *fp = fopen(file_name, "rt");
	if (!fp){ 
		fprintf(stderr, "Can't open file %s\n", file_name); 
		return NULL;
	}
	
	int version;
	fscanf(fp, "G%d", &version);
	if (version != 1){ 
		fprintf(stderr, "Bad version in file %s\n", file_name); 
		return NULL;
	}
	
	bool is_weighted;
	char is_weighted_str[16];
	fscanf(fp, "%16s", is_weighted_str);
	if (!strncmp(is_weighted_str, "weighted", 16))       { is_weighted = true; }
	else if (!strncmp(is_weighted_str, "unweighted", 16)){ is_weighted = false; }
	else {
		fprintf(stderr, "Bad weighted string in file %s: %*s\n", file_name, 
		                16, is_weighted_str); 
		return NULL;
	}
	
	int size = 4;
	int *data = malloc(2 * size * sizeof(*data));
	double *w = is_weighted ? malloc(size * sizeof(*w)) : NULL;
	
	// Count number of lines
	int n = -1, m;
	for (m=0; !feof(fp); m++){
		if (m == size){
			data = realloc(data, 2 * (2*size) * sizeof(*data));
			if (is_weighted){ w = realloc(w, (2*size) * sizeof(*w)); }
			size *= 2;
		}
		
		int from, to;
		double weight = 1.0;
		
		int count_items = fscanf(fp, "%d %d", &from, &to);
		if (is_weighted){
			count_items += fscanf(fp, "%lf", &w[m]);
		}
		
		if (count_items != (is_weighted ? 3 : 2)){
			fprintf(stderr, "Bad file %s, stopping in edge %d\n", file_name, m); 
			break;
		}
		
		data[2*m + 0] = from-1;
		data[2*m + 1] = to-1;
		if (is_weighted){ w[m] = weight; }
		
		if (n < from){ n = from; }
		if (n < to)  { n = to; }
	}
	fclose(fp);
	
	graph_t *graph = new_graph(n, is_weighted, is_directed);
	if (!graph){ 
		fprintf(stderr, "No memory for %s\n", file_name); free(data);
		return NULL;
	}
	
	int i;
	for (i=0; i < m; i++){
		int error;
		
		if (is_weighted)
		{
			error = graph_add_weighted_edge(graph, data[2*i+0], data[2*i+1], w[i]);
		}
		else
		{
			error = graph_add_edge(graph, data[2*i+0], data[2*i+1]);
		}
		if (error){
			fprintf(stderr, "No memory for %s\n", file_name);
			delete_graph(graph); free(data); return NULL;
		}
	}
	free(data);
	if (is_weighted){
		graph_sort_edges(graph);
		free(w); 
	}
	
	return graph;
}

int graph_num_vertices(const graph_t *g){
	assert(g);
	return g->n;
}

int graph_num_edges(const graph_t *g){
	assert(g);
	return g->m;
}

bool graph_is_directed(const graph_t *g){
	assert(g);
	return g->is_directed;
}

bool graph_is_weighted(const graph_t *g){
	assert(g);
	return g->is_weighted;
}

int graph_num_adjacents(const graph_t *g, int i){
	assert(g);
	assert(i >= 0 && i < g->n);
	
	return set_size(g->adjacencies[i]);
}

int graph_adjacents(const graph_t *g, int i, int *adj){
	assert(g);
	assert(i >= 0 && i < g->n);
	assert(adj);

	set_to_array(g->adjacencies[i], adj);
	return set_size(g->adjacencies[i]);
}

error_t graph_adjacent_set(const graph_t *g, int i, set_t *adj){
	assert(g);
	assert(i >= 0 && i < g->n);
	assert(adj);
	
	return set_union(adj, g->adjacencies[i]);
}

bool graph_is_adjacent(const graph_t *g, int i, int j){
	graph_check(g, i, j);
	
	return set_contains(g->adjacencies[i], j);
}

double graph_get(const graph_t *g, int i, int j){
	graph_check(g, i, j);
	
	bool is_adjacent = graph_is_adjacent(g, i, j);
	
	if (!is_adjacent)   { return +1.0/0.0; }
	if (!g->is_weighted){ return 1.0; }
	
	//The key must have i < j
	int min = i < j ? i : j;
	int max = i < j ? j : i;
	edge_t key = {min, max, 0.0};
	
	search_f search = g->is_edges_sorted ? bsearch : linsearch;
	edge_t *result = search((void *)&key, g->edge, g->m, 
													 sizeof(*g->edge), comp_edge_asc);
	assert(result);
	
	return result->weight;
}

void graph_print(const graph_t *graph){
	graph_fprint(stdout, graph);
}

int log_10(int i){
	int l;
	for (l=-1; i > 0; l++){
		i /= 10;
	}
	return l;
}

void graph_fprint(FILE *stream, const graph_t *graph){
	assert(graph);
	
	int i, n = graph_num_vertices(graph);
	
	int log_n = log_10(n);
	
	for (i=0; i < n; i++){
		fprintf(stream, "%*d: ", log_n, i);
		set_fprint(stream, graph->adjacencies[i]);
		fprintf(stream, "\n");
	}
}

graph_t *graph_copy(const graph_t *graph){
	assert(graph);
	
	int n = graph_num_vertices(graph);
	bool is_directed = graph_is_directed(graph);
	bool is_weighted = graph_is_weighted(graph);
	graph_t *copy = new_graph(n, is_weighted, is_directed);
	if (!copy){ return NULL; }
	
	int *adj = malloc (n * sizeof(*adj)); 
	if (!adj){ delete_graph(copy); return NULL; }
	
	int i, j;
	for (i=0; i < n; i++){
		int num_adj = graph_adjacents(graph, i, adj);
		for (j=0; j < num_adj; j++){
			if (is_weighted)
			{
				double w = graph_get(graph, i, adj[j]);
				error_t error = graph_add_weighted_edge(copy, i, adj[j], w);
				if (error){ free(adj); delete_graph(copy); return NULL; }
			} 
			else 
			{
				error_t error = graph_add_edge(copy, i, adj[j]);
				if (error){ free(adj); delete_graph(copy); return NULL; }
			}
		}
	}
	free(adj);
	
	return copy;
}

graph_t *graph_subset(const graph_t *graph, const set_t *vertices){
	assert(graph);
	assert(vertices);
	
	int n = graph_num_vertices(graph);
	int n_sub = set_size(vertices);
	bool is_directed = graph_is_directed(graph);
	bool is_weighted = graph_is_weighted(graph);
	
	graph_t *sub = new_graph(n_sub, is_weighted, is_directed);
	if (!sub){ return NULL; }
	
	int *adj = malloc (n * sizeof(*adj));
	if (!adj){ delete_graph(sub); return NULL; }
	
	int i, v;
	for (i=0; i < n_sub; i++){
		int origin = set_get(vertices, i);
		int num_adj = graph_adjacents(graph, origin, adj);
		for (v=0; v < num_adj; v++){
			int dest = adj[v];
			if (set_contains(vertices, dest)){
				int j = set_index(vertices, dest);
				if (is_weighted)
				{
					double w = graph_get(graph, origin, dest);
					error_t error = graph_add_weighted_edge(sub, i, j, w);
					if (error){ free(adj); delete_graph(sub); return NULL; }
				}
				else
				{
					error_t error = graph_add_edge(sub, i, j);
					if (error){ free(adj); delete_graph(sub); return NULL; }
				}
			}
		}
	}
	
	free(adj);
	return sub;
}
