#ifndef _GRAPH_METRIC_H
#define _GRAPH_METRIC_H

#include "graph.h"

/* Return value: number of components */
int graph_undirected_components(const graph_t *g, int *label);
int graph_directed_components(const graph_t *g, int *label);

/* Gives number of components labels obtained with graph_*_components */
int graph_num_components(const int *label, int n);
void graph_components(const int *label, int n, set_t **comp, int num_comp);

graph_t * graph_giant_component(const graph_t *g);

void graph_degree(const graph_t *g, int *degree);
void graph_directed_degree(const graph_t *g, int *in_degree, int *out_degree);

void graph_clustering(const graph_t *g, double *clustering);

void graph_num_triplets
	(const graph_t *g, int *num_triplet, int *num_triangle);
double graph_transitivy(const graph_t *g);

int graph_geodesic_distance(const graph_t *g, int i, int j);
void graph_geodesic(const graph_t *g, int **distance);
// Returns distribution of geodesic path lengths. The number of unreachable 
//pairs is returned in distance[n-1].
void graph_geodesic_distance_dist(const graph_t *g, int *distance);
void graph_geodesic_weight(const graph_t *g, double **distance); 

void graph_betweenness(const graph_t *g, double *betweenness,
                       int *distance_dist);
                       
int **graph_degree_matrix(const graph_t *g, int *kmax);
int graph_neighbor_degree(const graph_t *g, double *avg_degree);
int *graph_knn(const graph_t *g, int *kmax);
double graph_assortativity(const graph_t *g);

#endif
