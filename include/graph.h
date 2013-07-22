#ifndef _GRAPH_H
#define _GRAPH_H

#include "error.h"
#include "set.h"
#include "list.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct graph_t graph_t;

// Allocation and deallocation
graph_t * new_graph(int n, bool is_weighted, bool is_directed);
void delete_graph(graph_t *graph);

// Data input
graph_t * load_graph(char *file_name, bool is_directed);

// Insertion
error_t graph_add_edge(graph_t *g, int i, int j);
error_t graph_add_weighted_edge(graph_t *g, int i, int j, double w);

// Sorting
void graph_sort_edges(graph_t *g);

// Retrieval
bool graph_is_adjacent(const graph_t *g, int i, int j);
double graph_get(const graph_t *g, int i, int j);

// Query
int graph_num_vertices(const graph_t *g);
int graph_num_edges(const graph_t *g);
bool graph_is_directed(const graph_t *g);
bool graph_is_weighted(const graph_t *g);

// Adjacencies
int graph_num_adjacents(const graph_t *g, int i);
int graph_adjacents(const graph_t *g, int i, int *adj);
set_entry_t *graph_adjacent_head(const graph_t *g, int i);
error_t graph_adjacent_set(const graph_t *g, int i, set_t *adj);

// Printing
void graph_print(const graph_t *graph);
void graph_fprint(FILE *stream, const graph_t *graph);

// Copying
graph_t *graph_copy(const graph_t *graph);
graph_t *graph_subset(const graph_t *graph, const list_t *vertices);

#endif
