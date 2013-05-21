#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "sorting.h"
#include "stat.h"
#include "set.h"
#include "list.h"
#include "graph.h"
#include "graph_metric.h"

int graph_undirected_components(const graph_t *g, int *label){
	assert(g);
	assert(label);
	
	int i, n = graph_num_vertices(g);
	for (i=0; i < n; i++){
		label[i] = -1;
	}
	
	set_t *unvisited = new_set(0);
	set_t *visited   = new_set(0);
	set_t *seen      = new_set(0);
	for (i=0; i < n; i++){
		set_put(unvisited, i);
	}
	
	int curr_label = 0;
	while (set_size(unvisited) > 0){
		int v = set_get(unvisited, 0);
		set_remove(unvisited, v);
		
		set_put(visited, v);
		label[v] = curr_label;
		
		graph_adjacent_set(g, v, seen);
			
		while (set_size(seen) > 0)
		{
			int v = set_get(seen, 0);
			set_remove(unvisited, v);
			
			set_put(visited, v);
			label[v] = curr_label;
			
			graph_adjacent_set(g, v, seen);
			set_difference(seen, visited);
		}
		curr_label++;
	}
	
	delete_set(unvisited);
	delete_set(visited);
	delete_set(seen);
	
	return curr_label;
}

int graph_directed_components(const graph_t *g, int *label){
	assert(g);
	assert(label);
	
	bool is_implemented = false;
	assert(is_implemented);
	return 0;
}

int graph_num_components(const int *label, int n){
	assert(label);
	assert(n >= 0);
	
	set_t *set = new_set(0);
	int i;
	for (i=0; i < n; i++){
		set_put(set, label[i]);
	}
	int num = set_size(set);
	delete_set(set);
	return num;
}

void graph_components(const int *label, int n, set_t **comp, int num_comp){
	assert(label);
	assert(n >= 0);
	assert(comp);
	assert(num_comp == graph_num_components(label, n));
	int i;
	#ifndef NDEBUG
		for (i=0; i < num_comp; i++){
			assert(comp[i]);
		}
	#endif
	
	for (i=0; i < n; i++){
		set_put(comp[ label[i] ], i);
	}
}

graph_t * graph_giant_component(const graph_t *g){
	int i, n = graph_num_vertices(g);
	
	int *label = malloc(n * sizeof(*label));
	if (!label){ return NULL; }
	int num_comp = graph_undirected_components(g, label);
	
	// If the graph is connected, return a copy
	if (num_comp == 1){ 
		free(label); 
		return graph_copy(g);
	}
	
	pair_t *freq = stat_frequencies(label, n, NULL);
	if (!freq){ free(label); return NULL; }
	
	pair_t *max = search_max(freq, num_comp, sizeof(*freq), comp_value_asc);
	int max_comp = max->key;
	int max_size = max->value;
	free(freq);
	
	set_t *vertices = new_set(max_size);
	if (!vertices){ free(label); return NULL; }
	for (i=0; i < n; i++){
		if (label[i] == max_comp){
			set_put(vertices, i);
		}
	}
	free(label);
	
	graph_t *giant = graph_subset(g, vertices);
	delete_set(vertices);
	
	return giant;
}

void graph_degree(const graph_t *g, int *degree){
	assert(g);
	assert(degree);
	
	int i, j, n = graph_num_vertices(g);
	for (i=0; i < n; i++){
		degree[i] = graph_num_adjacents(g, i);
	}
	if (graph_is_directed(g)){
		int *adj = malloc(n * sizeof(*adj));
		for (i=0; i < n; i++){
			int num_adj = graph_adjacents(g, i, adj);
			for (j=0; j < num_adj; j++){
				degree[ adj[j] ]++;
			}
		}
		free(adj);
	}
}

void graph_directed_degree(const graph_t *g, int *in_degree, int *out_degree){
	assert(g);
	assert(in_degree);
	assert(out_degree);
	assert(graph_is_directed(g));
	
	int i, j, n = graph_num_vertices(g);
	memset(in_degree, 0, n * sizeof(*in_degree));
	memset(out_degree, 0, n * sizeof(*out_degree));
	
	int *adj = malloc(n * sizeof(*adj));
	for (i=0; i < n; i++){
		int num_adj = graph_adjacents(g, i, adj);
		out_degree[i] = num_adj;
		for (j=0; j < num_adj; j++){
			in_degree[ adj[j] ]++;
		}
	}
	free(adj);
}

void graph_clustering(const graph_t *g, double *clustering){
	assert(g);
	assert(clustering);
	assert(!graph_is_directed(g));
	
	int i, j, k, n = graph_num_vertices(g);
	memset(clustering, 0, n * sizeof(*clustering));
	
	int *adj = malloc(n * sizeof(*adj));
	
	for (i=0; i < n; i++){
		int num_adj = graph_adjacents(g, i, adj);
		if (num_adj <= 1)
		{ 
			clustering[i] = 0.0; 
		}
		else
		{
			for (j=0; j < num_adj; j++){
				for (k=j+1; k < num_adj; k++){
					if (graph_is_adjacent(g, adj[j], adj[k])){
						clustering[i] += 2.0;
					}
				}
			}
			clustering[i] /= num_adj * (num_adj - 1);
		}
	}
	
	free(adj);
}

void graph_num_triplets
		(const graph_t *g, int *_num_triplet, int *_num_triangle){
	assert(g);
	assert(!graph_is_directed(g));
	assert(_num_triplet || _num_triangle);
	
	int i, j, k, n = graph_num_vertices(g);
	
	int *adj = malloc(n * sizeof(*adj));
	int *second = malloc(n * sizeof(*second));
	
	if (!(adj && second)){
		if (adj){ free(adj); }
		if (second){ free(second); }
		return;
	}
	
	int num_triplet = 0;
	int num_closed = 0;
	
	for (i=0; i < n; i++){
		int num_adj = graph_adjacents(g, i, adj);
		for (j=0; j < num_adj; j++){
			int num_second = graph_adjacents(g, adj[j], second);
			num_triplet += num_second - 1;
			for (k=0; k < num_second; k++){
				if (second[k] != i && graph_is_adjacent(g, i, second[k])){
					num_closed++;
				}
			}
		}
	}
	
	free(adj);
	free(second);
	if(_num_triplet){ *_num_triplet = num_triplet; }
	if(_num_triangle){ *_num_triangle = num_closed/6; }
}

double graph_transitivy(const graph_t *g){
	int num_triplet, num_triangle;
	graph_num_triplets(g, &num_triplet, &num_triangle);
	
	return (3.0*num_triangle)/num_triplet;
}

int graph_geodesic_distance(const graph_t *g, int origin, int dest){
	assert(g);
	int n = graph_num_vertices(g);
	assert(origin >= 0 && origin < n);
	assert(dest >= 0 && dest < n);
	
	if (origin == dest){ return 0; }
	
	set_t *visited  = new_set(0);
	set_t *adjacent = new_set(0);
	set_t *seen     = new_set(0);
	if (!(visited && adjacent && seen)){
		if(visited) { delete_set(visited); }
		if(seen)    { delete_set(seen); }
		if(adjacent){ delete_set(adjacent); }
		return INT_MIN; 
	}
	
	set_put(seen, origin);
	int distance = 0;
	bool is_found = false;
	while (!is_found && set_size(seen) > 0){
		set_union(adjacent, seen);
		set_clean(seen);
		
		int i;
		for (i=0; i < set_size(adjacent); i++){
			int adj = set_get(adjacent, i);
			graph_adjacent_set(g, adj, seen);
			set_put(visited, adj);
			if (adj == dest){
				is_found = true;
				break;
			}
		}
		
		if (!is_found){
			set_difference(seen, visited);
			set_clean(adjacent);
			distance++;
		}
	}
	
	delete_set(visited);
	delete_set(adjacent);
	delete_set(seen);
	
	if (is_found){ return distance; }
	else         { return -1; }
}

void graph_geodesic(const graph_t *g, int **distance){
	assert(g);
	assert(distance);
	assert(distance[0]);
	
	int i, j, n = graph_num_vertices(g);
	for (i=0; i < n; i++){
		for (j=0; j < n; j++){
			distance[i][j] = -1;
		}
		distance[i][i] = 0;
	}
	
	set_t *visited  = new_set(0);
	set_t *adjacent = new_set(0);
	set_t *seen     = new_set(0);
	if (!(visited && adjacent && seen)){
		if(visited) { delete_set(visited); }
		if(seen)    { delete_set(seen); }
		if(adjacent){ delete_set(adjacent); }
	}
	
	for (i=0; i < n; i++){
		set_clean(visited);
		set_put(seen, i);
		int d = 0;
		while (set_size(seen) > 0){
			set_union(adjacent, seen);
			set_clean(seen);
		
			for (j=0; j < set_size(adjacent); j++){
				int adj = set_get(adjacent, j);
				graph_adjacent_set(g, adj, seen);
				set_put(visited, adj);
				distance[i][adj] = d;
			}
			
			set_difference(seen, visited);
			set_clean(adjacent);
			d++;
		}
	}
	
	delete_set(visited);
	delete_set(adjacent);
	delete_set(seen);
}

void graph_geodesic_distance_dist(const graph_t *g, int *distance){
	assert(g);
	assert(distance);
	
	int i, j, n = graph_num_vertices(g);
	memset(distance, 0, n*sizeof(*distance));
	
	set_t *visited  = new_set(0);
	set_t *adjacent = new_set(0);
	set_t *seen     = new_set(0);
	if (!(visited && adjacent && seen)){
		if(visited) { delete_set(visited); }
		if(seen)    { delete_set(seen); }
		if(adjacent){ delete_set(adjacent); }
	}
	
	int reachable_paths = 0;
	
	for (i=0; i < n; i++){
		set_clean(visited);
		set_put(seen, i);
		int d = 0;
		while (set_size(seen) > 0){
			distance[d] += set_size(seen);
			reachable_paths += set_size(seen);
			
			set_union(adjacent, seen);
			set_clean(seen);
		
			for (j=0; j < set_size(adjacent); j++){
				int adj = set_get(adjacent, j);
				graph_adjacent_set(g, adj, seen);
				set_put(visited, adj);
			}
			
			set_difference(seen, visited);
			set_clean(adjacent);
			d++;
		}
	}
	
	distance[n-1] = n*n - reachable_paths;
	
	delete_set(visited);
	delete_set(adjacent);
	delete_set(seen);
}

void graph_geodesic_weight(const graph_t *g, double **distance){
	assert(g);
	assert(distance);
	assert(distance[0]);
	
	bool is_implemented = false;
	assert(is_implemented);
}

/* Extracted from 
 * "A faster algorithm for betweenness centrality", Ulrik Brandes */
void graph_betweenness(const graph_t *g, double *betweenness, 
                       int *distance_dist){
	assert(g);
	assert(betweenness);
	
	int i, s, n = graph_num_vertices(g);
	memset(betweenness, 0, n * sizeof(*betweenness));
	if (distance_dist){
		memset(distance_dist, 0, n * sizeof(*distance_dist));
	}
	
	list_t *stack = new_list(0);
	list_t *queue = new_list(0);
	list_t **predecessor = malloc(n * sizeof(*predecessor));
	if (predecessor){ memset(predecessor, 0, n * sizeof(*predecessor)); }
	int     *path_count  = malloc(n * sizeof(*path_count));
	int     *distance    = malloc(n * sizeof(*distance));
	int     *adj         = malloc(n * sizeof(*adj));
	double  *dependency  = malloc(n * sizeof(*dependency));
	
	if (!(stack && queue && predecessor && path_count 
	            && distance && adj && dependency)){
		goto clean;
	}
	
	for (i=0; i < n; i++){
		predecessor[i] = new_list(0);
		if (!predecessor[i]){ goto clean; }
	}
	
	for (s=0; s < n; s++){
		for (i=0; i < n; i++){ list_clean(predecessor[i]); }
		
		memset(path_count, 0, n * sizeof(*path_count)); 
		path_count[s] = 1;
		
		for (i=0; i < n; i++){ distance[i] = -1; }
		distance[s] = 0;
		if (distance_dist){ distance_dist[0]++; }
		
		list_push(queue, s);
		
		while (list_size(queue) > 0){
			int v = list_remove(queue, 0);
			list_push(stack, v);
			
			int num_adj = graph_adjacents(g, v, adj);
			for (i=0; i < num_adj; i++){
				int w = adj[i];
				// w found for the first time?
				if (distance[w] < 0){
					list_push(queue, w);
					distance[w] = distance[v] + 1;
					if (distance_dist){ distance_dist[ distance[w] ]++; }
				}
				// shortest path to w via v?
				if (distance[w] == distance[v]+1){
					path_count[w] += path_count[v];
					list_push(predecessor[w], v);
				}
			}
		}
		
		memset(dependency, 0, n * sizeof(*dependency));
		while (list_size(stack) > 0){
			int w = list_pop(stack);
			for (i=0; i < list_size(predecessor[w]); i++){
				int v = list_get(predecessor[w], i);
				dependency[v] += (double)path_count[v]/path_count[w] 
				                 + (1 + dependency[w]);
				if (w != s){
					betweenness[w] += dependency[w];
				}
			}
		}
	}
	
clean:
	if (stack){ delete_list(stack); }
	if (queue){ delete_list(queue); }
	if (predecessor){ 
		for (i=0; i < n; i++){
			if (predecessor[i]){ delete_list(predecessor[i]); }
		}
		free(predecessor);
	}
	if (path_count) { free(path_count); }
	if (distance)   { free(distance); }
	if (adj)        { free(adj); }
	if (dependency) { free(dependency); }
}

int **graph_degree_matrix(const graph_t *g, int *_kmax){
	assert(g);
	
	int i, j, n = graph_num_vertices(g);
	
	// Discover highest degree
	int kmax = 0;
	for (i=0; i < n; i++){
		int ki = graph_num_adjacents(g, i);
		if (ki > kmax){ kmax = ki; }
	}
	
	int **mat = malloc((kmax+1) * sizeof(*mat));
	if (!mat){ return NULL; }
	
	mat[0] = malloc((kmax+1) * (kmax+1) * sizeof(*mat[0]));
	if (!mat[0]){ free(mat); return NULL; }
	for (i=1; i < kmax+1; i++){
		mat[i] = mat[0] + i*(kmax+1);
	}
	
	memset(mat[0], 0, (kmax+1) * (kmax+1) * sizeof(*mat[0]));
	
	int *adj = malloc(kmax * sizeof(*adj));
	if (!adj){ free(mat[0]); free(mat); }
	
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int kj = graph_num_adjacents(g, adj[j]);
			mat[ki][kj]++;
		}
	}
	free(adj);
	
	if (_kmax){ *_kmax = kmax+1; }
	return mat;
}

int graph_neighbor_degree(const graph_t *g, double *avg_degree){
	assert(g);
	assert(avg_degree);
	
	int i, j, n = graph_num_vertices(g);
	int kmax = 0;
	
	int *adj = malloc (n * sizeof(*adj));
	for (i=0; i < n; i++){
		int num_adj = graph_adjacents(g, i, adj);
		if (num_adj > kmax){ kmax = num_adj; }
		
		avg_degree[i] = 0.0;
		for (j=0; j < num_adj; j++){
			avg_degree[i] += (double)graph_num_adjacents(g, adj[j])/num_adj;
		}
	}
	free(adj);
	return kmax;
}

double *graph_knn(const graph_t *g, int *_kmax){
	assert(g);
	assert(_kmax);
	
	int i, n = graph_num_vertices(g);
	
	double *avg_degree = malloc(n * sizeof(*avg_degree));
	int kmax = graph_neighbor_degree(g, avg_degree);
	
	double *knn = malloc((kmax+1) * sizeof(*knn));
	int *num_degree = malloc((kmax+1) * sizeof(*num_degree));
	
	memset(num_degree, 0, (kmax+1) * sizeof(*num_degree));
	memset(knn, 0, (kmax+1) * sizeof(*knn));
	
	for (i=0; i < n; i++){
		int ki = graph_num_adjacents(g, i);
		knn[ki] += avg_degree[i];
		num_degree[ki]++;
	}
	
	for (i=0; i < kmax+1; i++){
		knn[i] /= num_degree[i];
	}
	
	free(avg_degree);
	free(num_degree);
	
	*_kmax = kmax+1;
	return knn;
}

double graph_assortativity(const graph_t *g){
	assert(g);
	
	int i, j, n = graph_num_vertices(g), m = graph_num_edges(g);
	int *adj = malloc(n * sizeof(*adj));
	if (!adj){ return 0.0/0.0; }
	
	double prod = 0.0;
	double sum = 0.0;
	double sq = 0.0;
	
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			int kj = graph_num_adjacents(g, v);
			if (v > i){
				prod += (1.0/m) * (ki * kj);
				sum += (0.5/m) * (ki + kj);
				sq += (0.5/m) * (ki*ki + kj*kj);
			}
		}
	}
	
	double r = (prod - sum*sum)/(sq - sum*sum);
	
	free(adj);
	return r;
}

double dist(double *u, double *v, int n){
	double d = 0.0;
	int i;
	for (i=0; i < n; i++){
		d += (u[i] - v[i])*(u[i] - v[i]);
	}
	return d;
}

double norm(double *v, int n){
	double s = 0.0;
	int i;
	for (i=0; i < n; i++){
		s += v[i]*v[i];
	}
	return s;
}

void graph_eigenvalue(const graph_t *g, double *eigen){
	assert(g);
	assert(eigen);
	
	int i, j, n = graph_num_vertices(g);
	
	double *temp = malloc(n * sizeof(*temp)); 
	if (!temp){ return; }
	
	int *adj = malloc(n * sizeof(*adj));	
	if (!adj){ free(temp); return; }
	
	double *curr = eigen, *next = temp;
	for (i=0; i < n; i++){
		next[i] = 0.0;
		curr[i] = 1.0;
	}
	
	double tol = GRAPH_METRIC_TOLERANCE;
	int max_iter = GRAPH_METRIC_MAX_ITERATIONS;
	
	int count = 0;
	while(dist(curr, next, n) > tol && count < max_iter){
		memset(next, 0, n * sizeof(*next));
		
		for (i=0; i < n; i++){
			int ki = graph_adjacents(g, i, adj);
			for (j=0; j < ki; j++){
				int v = adj[j];
				next[v] += curr[i];
			}
		}
		
		double s = sqrt(norm(next, n));
		for (i=0; i < n; i++){
			next[i] /= s;
		}
		
		count++;
		double *aux = curr;
		curr = next;
		next = aux;
	}
	
	free(temp);
	free(adj);
}

void graph_pagerank(const graph_t *g, double alpha, double *rank){
	assert(g);
	assert(rank);
	
	int i, j, n = graph_num_vertices(g);
	
	double *temp = malloc(n * sizeof(*temp)); 
	if (!temp){ return; }
	
	int *adj = malloc(n * sizeof(*adj));	
	if (!adj){ free(temp); return; }
	
	double *curr = rank, *next = temp;
	for (i=0; i < n; i++){
		next[i] = 0.0;
		curr[i] = 1.0/n;
	}
	
	double tol = GRAPH_METRIC_TOLERANCE;
	int max_iter = GRAPH_METRIC_MAX_ITERATIONS;
	
	int count = 0;
	while(n*dist(curr, next, n) > tol || count < max_iter){
		memset(next, 0, n * sizeof(*next));
		
		for (i=0; i < n; i++){
			int ki = graph_adjacents(g, i, adj);
			for (j=0; j < ki; j++){
				int v = adj[j];
				next[v] += (1-alpha)/n + alpha * curr[i]/ki;
			}
		}
		
		count++;
		double *aux = curr;
		curr = next;
		next = aux;
	}
	
	free(temp);
	free(adj);
}

void graph_kcore(const graph_t *g, int *core){
	assert(g);
	assert(core);
	
	int i, n = graph_num_vertices(g);
	
	memset(core, 0, n * sizeof(*core));
	
	
}
