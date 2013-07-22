#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>

#include "sorting.h"
#include "stat.h"
#include "set.h"
#include "list.h"
#include "graph.h"
#include "graph_metric.h"

#ifndef CACHE_ALIGNMENT
	#define CACHE_ALIGNMENT 64
#endif

/************** Component identification and extraction ***********************/
int graph_undirected_components(const graph_t *g, int *label){
	assert(g);
	assert(label);
	
	int i, n = graph_num_vertices(g);
	for (i=0; i < n; i++){
		label[i] = -1;
	}
	
	int *queue = malloc(n * sizeof(*queue));
	int head = 0, tail = 0;
	if (!queue){ return -1; }
	
	bool *is_visited = malloc(n * sizeof(*is_visited));
	if (!is_visited){ free(queue);  return -1; }
	memset(is_visited, 0, n * sizeof(*is_visited));
	
	queue[tail++] = 0;
	
	int count, smallest=0;
	for (count=0; smallest < n;count++){
		while (tail > head){
			int u = queue[head++];
			is_visited[u] = true;
			label[u] = count;
			
			set_entry_t *adj = graph_adjacent_head(g, u);
			for (;adj != NULL; adj = adj->next){
				int v = adj->key;
				if (!is_visited[v]){
					queue[tail++] = v;
				}
			}
		}
		
		while(smallest < n && is_visited[smallest]){
			smallest++;
		}
		head = tail = 0;
		queue[tail++] = smallest;
	}
	
	free(is_visited);
	free(queue);
	return count;
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
	
	int max_comp = freq[0].key, max_size = freq[0].value;
	for (i=1; i < num_comp; i++){
		if (freq[i].value > max_size){
			max_comp = freq[i].key;
			max_size = freq[i].value;
		}
	}
	free(freq);
	
	list_t *vertices = new_list(max_size);
	if (!vertices){ free(label); return NULL; }
	for (i=0; i < n; i++){
		if (label[i] == max_comp){
			list_push(vertices, i);
		}
	}
	free(label);
	
	graph_t *giant = graph_subset(g, vertices);
	delete_list(vertices);
	
	return giant;
}

/***************************** Degree metrics *********************************/

void graph_degree(const graph_t *g, int *degree){
	assert(g);
	assert(degree);
	
	int i, n = graph_num_vertices(g);
	for (i=0; i < n; i++){
		degree[i] = graph_num_adjacents(g, i);
	}
	if (graph_is_directed(g)){
		for (i=0; i < n; i++){
			set_entry_t *adj = graph_adjacent_head(g, i);
			for (; adj != NULL; adj = adj->next){
				degree[ adj->key ]++;
			}
		}
	}
}

void graph_directed_degree(const graph_t *g, int *in_degree, int *out_degree){
	assert(g);
	assert(in_degree);
	assert(out_degree);
	assert(graph_is_directed(g));
	
	int i, n = graph_num_vertices(g);
	memset(in_degree, 0, n * sizeof(*in_degree));
	memset(out_degree, 0, n * sizeof(*out_degree));
	
	for (i=0; i < n; i++){
		out_degree[i] = graph_num_adjacents(g, i);
		set_entry_t *adj = graph_adjacent_head(g, i);
		for (; adj != NULL; adj = adj->next){
			in_degree[ adj->key ]++;
		}
	}
}

/***************************** Clustering metrics *****************************/

void graph_clustering(const graph_t *g, double *clustering){
	assert(g);
	assert(clustering);
	assert(!graph_is_directed(g));
	
	int i, n = graph_num_vertices(g);
	memset(clustering, 0, n * sizeof(*clustering));
	
	for (i=0; i < n; i++){
		int ki = graph_num_adjacents(g, i);
		if (ki <= 1)
		{ 
			clustering[i] = 0.0; 
		}
		else
		{
			set_entry_t *p = graph_adjacent_head(g, i), *q;
			for (; p != NULL; p = p->next){
				for (q=p->next; q != NULL; q = q->next){
					if (graph_is_adjacent(g, p->key, q->key)){
						clustering[i] += 2.0;
					}
				}
			}
			clustering[i] /= ki * (ki - 1);
		}
	}
}

void graph_num_triplets
		(const graph_t *g, int *_num_triplet, int *_num_triangle){
	assert(g);
	assert(!graph_is_directed(g));
	assert(_num_triplet || _num_triangle);
	
	int i, n = graph_num_vertices(g);
	
	int num_triplet = 0;
	int num_closed = 0;
	
	for (i=0; i < n; i++){
		set_entry_t *adj = graph_adjacent_head(g, i);
		for (; adj != NULL; adj = adj->next){
			int num_second = graph_num_adjacents(g, adj->key);
			num_triplet += num_second - 1;
			
			set_entry_t *sec = graph_adjacent_head(g, adj->key);
			for (; sec != NULL; sec = sec->next){
				if (sec->key != i && graph_is_adjacent(g, i, sec->key)){
					num_closed++;
				}
			}
		}
	}
	
	if(_num_triplet){ *_num_triplet = num_triplet; }
	if(_num_triangle){ *_num_triangle = num_closed/6; }
}

double graph_transitivy(const graph_t *g){
	int num_triplet, num_triangle;
	graph_num_triplets(g, &num_triplet, &num_triangle);
	
	return (3.0*num_triangle)/num_triplet;
}

/************************ Geodesic distance metrics ***************************/

void graph_geodesic_paths
		(const graph_t *g, int s, int *distance, int *sequence, int *path_count,
		 list_t **predecessor){
	assert(g);
	int i, n = graph_num_vertices(g);
	assert(s >= 0 && s < n);
	assert(distance);
	
	bool is_betweenness = false;
	if (sequence && path_count && predecessor){
		is_betweenness = true;
	}
	
	int seq_step = 0;
	if (is_betweenness){
		// sequence stores the vertices in the order they were visited
		memset(sequence, 0, n * sizeof(*sequence));
		
		// path_count counts the number of paths that cross each vertex
		memset(path_count, 0, n * sizeof(*path_count));
		path_count[s] = 1;
		
		for (i=0; i < n; i++){
			list_clean(predecessor[i]);
		}
	}
	
	// distance stores the distance from each vertex to s. -1 represents infinity
	for (i=0; i < n; i++){ distance[i] = -1; }
	distance[s] = 0;
		
	// Enqueue vertices for processing
	int *queue = malloc(n * sizeof(*queue));
	if (!queue){ return; }
	int head = 0, tail = 0;
	
	queue[tail++] = s; // Enqueue s
	
	// Compute distance, path_count and predecessors
	while (tail > head){
		int v = queue[head++]; // Dequeue v
		if (is_betweenness){ sequence[seq_step++] = v; }
		
		set_entry_t *adj = graph_adjacent_head(g, v);
		for (; adj != NULL; adj = adj->next){
			int w = adj->key;
			// w found for the first time?
			if (distance[w] < 0){
				queue[tail++] = w; // Enqueue w
				distance[w] = distance[v] + 1;
			}
			// shortest path to w via v?
			if (is_betweenness && distance[w] == distance[v]+1){
				path_count[w] += path_count[v];
				list_push(predecessor[w], v);
			}
		}
	}
	
	free(queue);
}

int graph_geodesic_distance(const graph_t *g, int origin, int dest){
	assert(g);
	int i, n = graph_num_vertices(g);
	assert(origin >= 0 && origin < n);
	assert(dest >= 0 && dest < n);
	
	if (origin == dest){ return 0; }
	
	// distance stores the distance from each vertex to s. -1 represents infinity
	int *distance = malloc(n * sizeof(*distance));
	if (!distance){ return INT_MIN; }
	for (i=0; i < n; i++){ distance[i] = -1; }
	distance[origin] = 0;
		
	// Enqueue vertices for processing
	int *queue = malloc(n * sizeof(*queue));
	if (!queue){ free(distance); return INT_MIN; }
	int head = 0, tail = 0;
	
	queue[tail++] = origin; // Enqueue origin
	
	// Compute distance
	while (tail > head){
		int v = queue[head++];
		
		set_entry_t *adj = graph_adjacent_head(g, v);
		for (; adj != NULL; adj = adj->next){
			int w = adj->key;
			// w found for the first time?
			if (distance[w] < 0){
				queue[tail++] = w;
				distance[w] = distance[v] + 1;
			}
			if (w == dest){
				break;
			}
		}
		
		if (distance[dest] >= 0)
			break;
	}
	
	int d = distance[dest];
	free(distance);
	free(queue);
	
	return d;
}

void graph_geodesic_vertex(const graph_t *g, int i, int *distance){
	assert(g);
	assert(distance);
	
	graph_geodesic_paths(g, i, distance, NULL, NULL, NULL);
}

void graph_geodesic_all(const graph_t *g, int **distance){
	assert(g);
	assert(distance);
	assert(distance[0]);
	
	int i, n = graph_num_vertices(g);
	for (i=0; i < n; i++){
		graph_geodesic_vertex(g, i, distance[i]);
	}
}

int *graph_geodesic_distribution(const graph_t *g, int *_diameter){
	assert(g);
	
	int i, j, n = graph_num_vertices(g);
	
	int *distribution = malloc(n * sizeof(*distribution));
	if (!distribution){ return NULL; }
	memset(distribution, 0, n * sizeof(*distribution));
	
	int *distance = malloc(n * sizeof(*distance));
	
	int diameter = 0;
	int unreachable_paths = 0;
	
	for (i=0; i < n; i++){
		graph_geodesic_vertex(g, i, distance);
		for (j=0; j < n; j++){
			if (distance[j] == -1){ unreachable_paths++; } 
			else                  { distribution[ distance[j] ]++; }
			
			if (distance[j] > diameter){ diameter = distance[j]; }
		}
	}
	free(distance);
	
	distribution = realloc(distribution, (diameter+1) * sizeof(*distribution));
	distribution[diameter] = unreachable_paths;
	
	if (_diameter){ *_diameter = diameter; }
	return distribution;
}

/************************ Centrality measures *********************************/

/** Betweenness 
 * Betweenness calculation is an expensive task. For this reason, this code
 * is organized to allow simple parallel execution.
 * 
 * graph_betweenness: 
 *   Calls graph_betweenness_step with a step=1, thus calculating
 * betweenness for all vertices.
 * 
 * graph_betweenness_step:
 *   Computes betweenness properly, but only of vertices initial, initial+step,
 * initial+2*step, ..., initial+k*step < n.
 * 
 * graph_parallel_betweenness:
 *   Creates num_processors threads, each computing a subset of betweenness with
 * graph_betweenness_step. As they complete their tasks, the value is reduced
 * in one single vector.
 *   This function replicates memory needs for all threads, thus its possible
 * to exhaust memory. If a failure is detected, the function launches a single
 * thread execution.
 */

// Increments betweenness given the result of a run starting from vertex s.
void graph_inc_betweenness
		(int s, const int *distance, const int *sequence, const int *path_count,
		 list_t **predecessor, double *betweenness, int n){
	
	double *dependency = malloc(n * sizeof(*dependency));
	if (!dependency){ return; }
	memset(dependency, 0, n * sizeof(*dependency));
	
	int i, j;
	for (i=n-1; i >= 0; i--){
		int w = sequence[i];
		int num_predecessor = list_size(predecessor[w]);
		for (j=0; j < num_predecessor; j++){
			int v = list_get(predecessor[w], j);
			dependency[v] += 
				(double)path_count[v]/path_count[w] + (1 + dependency[w]);
			if (w != s){
				betweenness[w] += dependency[w];
			}
		}
	}
	
	free(dependency);
}

void graph_betweenness_step
		(const graph_t *g, double *betweenness, int initial, int step){
	assert(g);
	assert(betweenness);
	assert(step > 0);
	assert(initial >= 0 && initial < step);
	
	int i, n = graph_num_vertices(g);
	memset(betweenness, 0, n * sizeof(*betweenness));
	
	int *distance = malloc(n * sizeof(*distance));
	int *sequence = malloc(n * sizeof(*sequence));
	int *path_count = malloc(n * sizeof(*path_count));
	
	bool lists_ok = true;
	list_t **predecessor = malloc(n * sizeof(*predecessor));
	if (!predecessor){ lists_ok = false; }
	if (lists_ok){
		for (i=0; i < n; i++){
			predecessor[i] = new_list(0); if (!predecessor[i]){ lists_ok = false; }
		}
	}
	
	if (distance && sequence && path_count && lists_ok) {
		int s;
		for (s=initial; s < n; s += step){
			graph_geodesic_paths(g, s, distance, sequence, path_count, predecessor);
			graph_inc_betweenness(s, distance, sequence, path_count, predecessor, 
														betweenness, n);
		}
	}
	
	if (distance){ free(distance); }
	if (sequence){ free(sequence); }
	if (path_count){ free(path_count); }
	if (predecessor){
		for (i=0; i < n; i++){ 
			if (predecessor[i]){ delete_list(predecessor[i]); }
		}
		free(predecessor);
	}
}

void graph_betweenness(const graph_t *g, double *betweenness){
	int initial=0, step=1;
	graph_betweenness_step(g, betweenness, initial, step);
}

typedef struct {
	const graph_t *graph;
	int index, num_processors;
	char padding[CACHE_ALIGNMENT - sizeof(const graph_t *) - 2*sizeof(int)];
} graph_betweenness_task_params_t;

void *graph_betweenness_task(void *args);

void graph_parallel_betweenness
		(const graph_t *g, double *betweenness, int num_processors){
	assert(g);
	assert(betweenness);
	assert(num_processors > 1);
	
	int i, j, n = graph_num_vertices(g);
	memset(betweenness, 0, n * sizeof(*betweenness));
	bool is_failure = false;
	
	pthread_t *thread = malloc(num_processors * sizeof(*thread));
	graph_betweenness_task_params_t *params;
	params = malloc(num_processors * sizeof(*params));
	
	if (!(thread && params)){ is_failure = true; num_processors = 0; }
	
	for (i=0; i < num_processors; i++){
		params[i].graph = g;
		params[i].index = i;
		params[i].num_processors = num_processors;
		int result 
			= pthread_create(&thread[i], NULL, graph_betweenness_task, &params[i]);
		if (result != 0)
		{
			is_failure = true;
			break;
		}
	}
	
	// If a failure happened launching threads, its necessary to wait for the
	//successful ones to complete. If no failure happened, than 
	//i == num_processors and we wait on all threads.
	num_processors = i;
	
	for (i=0; i < num_processors; i++){
		double *partial;
		int result = pthread_join(thread[i], (void **)&partial);
		if (result == 0 && partial != NULL)
		{
			for (j=0; j < n; j++){
				betweenness[j] += partial[j];
			}
		}
		else
		{
			is_failure = true;
		}
		
		if (partial){ free(partial); }
	}
	
	free(params);
	free(thread);
	
	if (is_failure){
		fprintf(stderr, "Failure executing parallel betweenness. "
		                "Launching single-threaded\n");
		graph_betweenness(g, betweenness);
	}
}

void *graph_betweenness_task(void *args){
	graph_betweenness_task_params_t params;
	params = *(graph_betweenness_task_params_t*) args;
	
	const graph_t *g = params.graph;
	int initial = params.index;
	int step = params.num_processors;
	
	int n = graph_num_vertices(g);	
	double *betweenness = malloc(n * sizeof(*betweenness));
	if (!betweenness){ return NULL; }
	
	graph_betweenness_step(g, betweenness, initial, step);
	return betweenness;
}

// Vector distance, defined by canonical inner product
double dist(double *u, double *v, int n){
	double d = 0.0;
	int i;
	for (i=0; i < n; i++){
		d += (u[i] - v[i])*(u[i] - v[i]);
	}
	return d;
}

// Vector norm, defined by canonical inner product
double norm(double *v, int n){
	double s = 0.0;
	int i;
	for (i=0; i < n; i++){
		s += v[i]*v[i];
	}
	return s;
}

void graph_eigenvector(const graph_t *g, double *eigen){
	assert(g);
	assert(eigen);
	
	int i, n = graph_num_vertices(g);
	
	double *temp = malloc(n * sizeof(*temp)); 
	if (!temp){ return; }
	
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
			set_entry_t *adj = graph_adjacent_head(g, i);
			for (; adj != NULL; adj = adj->next){
				int v = adj->key;
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
}

void graph_pagerank(const graph_t *g, double alpha, double *rank){
	assert(g);
	assert(rank);
	
	int i, n = graph_num_vertices(g);
	
	double *temp = malloc(n * sizeof(*temp)); 
	if (!temp){ return; }
	
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
			int ki = graph_num_adjacents(g, i);
			set_entry_t *adj = graph_adjacent_head(g, i);
			for (; adj != NULL; adj = adj->next){
				int v = adj->key;
				next[v] += (1-alpha)/n + alpha * curr[i]/ki;
			}
		}
		
		count++;
		double *aux = curr;
		curr = next;
		next = aux;
	}
	
	free(temp);
}

int graph_kcore(const graph_t *g, int *core){
	assert(g);
	assert(!graph_is_directed(g));
	assert(core);
	
	int i, n = graph_num_vertices(g);
	
	memset(core, 0, n * sizeof(*core));
	
	// Lists current degrees, and finds maximum degree kmax
	int *degree = malloc(n * sizeof(*degree));
	int kmax = 0;
	
	for (i=0; i < n; i++){
		int ki = graph_num_adjacents(g, i);
		degree[i] = ki;
		if (ki > kmax){ kmax = ki; }
	}
	
	// Map from degree to vertex
	list_t **map = malloc((kmax+1) * sizeof(*map));
	for (i=0; i < kmax+1; i++){
		map[i] = new_list(0);
	}
	
	for (i=0; i < n; i++){
		int ki = degree[i];
		list_insert(map[ki], i);
	}
	
	int k = 0; // core count
	for (i=0; i < n; i++){
		// Finds minimum degree with unprocessed vertices
		int d;
		for (d=0; d < kmax+1; d++){
			if (list_size(map[d]) > 0){
				break;
			}
		}
		assert(d < kmax+1);
		
		// "Remove" vertex from graph, storing its core number
		if (d > k){ k = d; }
		int u = list_pop(map[d]);
		degree[u] = 0;
		core[u] = k;
		
		// Update adjacents degrees (and map placement)
		set_entry_t *adj = graph_adjacent_head(g, u);
		for (; adj != NULL; adj = adj->next){
			int v = adj->key;
			if (degree[v] > 0){ // Unprocessed vertex
				degree[v]--;
				
				int prev = degree[v]+1;
				int curr = degree[v];
				
				list_remove_element(map[prev], v);
				list_insert(map[curr], v);
			}
		}
	}
	
	free(degree);
	for (i=0; i < kmax+1; i++){
		delete_list(map[i]);
	}
	free(map);
	
	return k;
}

/************************ Correlation measures ********************************/

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
	
	for (i=0; i < n; i++){
		int ki = graph_num_adjacents(g, i);
		set_entry_t *adj = graph_adjacent_head(g, i);
		for (j=0; j < ki; j++){
			int kj = graph_num_adjacents(g, adj->key);
			adj = adj->next;
			mat[ki][kj]++;
		}
	}
	
	if (_kmax){ *_kmax = kmax+1; }
	return mat;
}

double graph_neighbor_degree_vertex(const graph_t *g, int i){
	assert(g);
	int n = graph_num_vertices(g);
	assert(0 <= i && i < n);
	
	int ki = graph_num_adjacents(g, i);
	set_entry_t *adj = graph_adjacent_head(g, i);
	
	double knn = 0.0;
	for (; adj != NULL; adj = adj->next){
		int v = adj->key;
		knn += (double) graph_num_adjacents(g, v) / ki;
	}
	
	return knn;
}

int graph_neighbor_degree_all(const graph_t *g, double *avg_degree){
	assert(g);
	assert(avg_degree);
	
	int i, n = graph_num_vertices(g);
	int kmax = 0;
	
	for (i=0; i < n; i++){
		int ki = graph_num_adjacents(g, i);
		if (ki > kmax){ kmax = ki; }
		
		avg_degree[i] = 0.0;
		set_entry_t *adj = graph_adjacent_head(g, i);
		for (; adj != NULL; adj = adj->next){
			int v = adj->key;
			avg_degree[i] += (double)graph_num_adjacents(g, v) / ki;
		}
	}
	
	return kmax;
}

double *graph_knn(const graph_t *g, int *_kmax){
	assert(g);
	assert(_kmax);
	
	int i, n = graph_num_vertices(g);
	
	double *avg_degree = malloc(n * sizeof(*avg_degree));
	int kmax = graph_neighbor_degree_all(g, avg_degree);
	
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
	
	int i, n = graph_num_vertices(g);
	
	double *degree = malloc (n * sizeof(*degree));
	if (!degree){ return NAN; }
	
	double *neighbor_avg_deg = malloc (n * sizeof(*neighbor_avg_deg));
	if (!neighbor_avg_deg){ free(degree); return NAN; }
	
	int kmax = 0;
	for (i=0; i < n; i++){
		int ki = graph_num_adjacents(g, i);
		degree[i] = (double)ki;
		if (ki > kmax) { kmax = ki; }
	}
	
	for (i=0; i < n; i++){
		int ki = graph_num_adjacents(g, i);
		neighbor_avg_deg[i] = 0.0;
		set_entry_t *adj = graph_adjacent_head(g, i);
		for (; adj != NULL; adj = adj->next){
			int v = adj->key;
			neighbor_avg_deg[i] += degree[v] / ki;
		}
	}
	
	double r = stat_pearson(degree, neighbor_avg_deg, n);
	
	free(degree);
	free(neighbor_avg_deg);
	return r;
}

void graph_closeness(const graph_t *g, double *closenness){
	assert(g);
	assert(closenness);
	
	int i, n = graph_num_vertices(g);
	
	int *distance = malloc(n * sizeof(*distance));
	
	for (i=0; i < n; i++){
		graph_geodesic_vertex(g, i, distance);
		int farness = stat_int_sum(distance, n);
		closenness[i] = 1.0/farness;
	}
	
	free(distance);
}
