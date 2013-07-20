#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdbool.h>
#include <stdio.h>
#include "error.h"

typedef enum {
	GRAPH_WEIGHT    = 1 << 1, // Store weight in edges
	GRAPH_DIRECTION = 1 << 2, // Edges are directed
	GRAPH_LOOP      = 1 << 3, // Allows loops with length 1 (self-loops)
	GRAPH_PSEUDO    = 1 << 4, // Allows multiple edges between same vertices
} graph_flag_t;

typedef enum {
	GRAPH_MATRIX_TEXTUAL,
	GRAPH_MATRIX_BINARY,
	GRAPH_EDGELIST_TEXTUAL,
	GRAPH_EDGELIST_BINARY,
	GRAPH_PAJEK
} graph_format_t;

typedef struct {
	int from, to;
} edge_t;

typedef struct {
	int from, to; 
	float weight; 
} weighted_edge_t;

typedef struct graph_t graph_t;

// Allocation and deallocation
graph_t *new_simple_graph();
graph_t *new_graph(unsigned int flags);
void delete_graph(graph_t *graph);

// Graph conversion
graph_t *graph_threshold(const graph_t *original, double threshold, bool keep_weights);
graph_t *graph_dual(const graph_t *original);
graph_t *graph_simmetry(const graph_t *original, bool keep_directed);
graph_t *graph_direct(graph_t *original, bool split_weights);
graph_t *graph_remove_self_loops(const graph_t *original);
graph_t *graph_coalesce(const graph_t *original);

// Input/Output
error_t store_graph(FILE *fp, graph_format_t format);
graph_t *load_graph(FILE *fp);

// Insertion
void graph_add_vertex(graph_t *g, int v);
int  graph_add_edge(graph_t *g, int i, int j);
int  graph_add_weighted_edge(graph_t *g, int from, int to, double w);
int  graph_add_multi_edge(graph_t *g, const int *vertices);
int  graph_add_multi_weighted_edge(graph_t *g, const int *vertices, double w);

// Removal
void graph_remove_vertex(graph_t *g, int v);
void graph_remove_edge(graph_t *g, int from, int to);
void graph_remove_multi_edge(graph_t *g, int *vertices);
void graph_remove_edge_id(graph_t *g, int edge_id);

// Retrieval
bool graph_is_adjacent(const graph_t *g, int i, int j);
int     graph_get_id(const graph_t *g, int i, int j);
edge_t graph_get_edge(const graph_t *g, int edge_id);
double graph_get_weight(const graph_t *g, int i, int j);
double graph_get_weight_id(const graph_t *g, int edge_id);

// Query
int graph_num_vertices(const graph_t *g);
int graph_num_edges(const graph_t *g);
bool graph_is_weighted(const graph_t *g);
bool graph_is_directed(const graph_t *g);
bool graph_is_looped(const graph_t *g);
bool graph_is_pseudo(const graph_t *g);
bool graph_is_multi(const graph_t *g);

// Adjacencies
int graph_num_adjacents(const graph_t *g, int i);
int graph_adjacents(const graph_t *g, int i, int *adj);

// Copying
graph_t *graph_copy(const graph_t *graph);
graph_t *graph_subset(const graph_t *graph, const set_t *vertices);

#endif
