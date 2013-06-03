#ifndef _GRAPH_METRIC_H
#define _GRAPH_METRIC_H

#include "graph.h"

/* Error tolerance for numeric methods. */
#ifndef GRAPH_METRIC_TOLERANCE
 #define GRAPH_METRIC_TOLERANCE 1e-6
#endif
/* Maximum number of iterations for numeric methods */
#ifndef GRAPH_METRIC_MAX_ITERATIONS
 #define GRAPH_METRIC_MAX_ITERATIONS 100
#endif

/************** Component identification and extraction ***********************/
// Label vertices' components treating edges as undirected.
int graph_undirected_components(const graph_t *g, int *label);
// Label vertices' components treating edges as directed.
int graph_directed_components(const graph_t *g, int *label);
// Extract number of components from label vector.
int graph_num_components(const int *label, int n);
// Map componenents to vertices from a label vector.
void graph_components(const int *label, int n, set_t **comp, int num_comp);
// Creates a new graph from g's largest component.
graph_t * graph_giant_component(const graph_t *g);

/***************************** Degree metrics *********************************/
// List all vertices' degrees.
void graph_degree(const graph_t *g, int *degree);
// List all vertices' incoming and outgoing degrees.
void graph_directed_degree(const graph_t *g, int *in_degree, int *out_degree);

/***************************** Clustering metrics *****************************/
// List all vertices' local clustering.
void graph_clustering(const graph_t *g, double *clustering);
/* Counts number of triplets and triangles (6 * number of closed triplets).
 * This measures are only defined for undirected graphs.
 * 
 * Seeing g as a binary matrix, a triplet (i, j, k) is defined if 
 *     g_ij * g_jk == 1.
 * A closed triplet (i, j, k) is defined if 
 *     g_ij * g_jk * g_ki = 1.
 * 
 * The number of triangles is 6 times the number of closed triplets, because 
 * the closed triplet is counted in all combinations of the triangle's vertices:
 * (i, j, k), (i, k, j), (j, i, k), (j, k, i), (k, i, j), (k, j, i).
 * 
 * Pre:
 *   g is undirected.
 * Post:
 *   If num_triplet != NULL, *num_triplet is the number of triplets.
 *   If num_triangle != NULL, *num_triangle has the number of triangles.
 * */
void graph_num_triplets
	(const graph_t *g, int *num_triplet, int *num_triangle);
/* Compute the ratio between number of triangles and number of triplets.
 * This measure is only defined for undirected graphs.
 * 
 * Considering the definitions on graph_num_triplets, the transitivity is
 * simply defined as
 *     C = number of closed triplets / number of triplets
 * 
 * Pre:
 *   g is undirected.
 * Return value:
 *   Transitivity measure.
 * */
double graph_transitivy(const graph_t *g);

/************************ Geodesic distance metrics ***************************/
/****************************** DEFINITIONS ***********************************
 * 
 * Seeing g as a binary matrix, we define a path P_ij = {i, k1, k2, ..., kp, j} 
 * as a sequence of vertices such that 
 *     g_{i,k1} * g_{k1,k2} * ... g_{kp,j} = 1
 * Alternatively, we can see a path as a sequence of edges 
 *     P' = {g_{i,k1}, ..., g_{kp,j}}
 * 
 * Vertices i and j are reachable if there exists a path between them.
 * 
 * The length of a path is the number of edges traversed, or (number of
 * vertices in the path) - 1). A path with only one vertex has length 0, and
 * a path between two adjacent vertices has length 1.
 * 
 * A geodesic path is a path that has the smallest length possible. There can
 * be many geodesic paths.
 * 
 * Geodesic distance is the length of a geodesic path.
 */
/* Returns the geodesic distance between vertices i and j. 
 * The algorithm used is a simple breadth-first traversal.
 * 
 * Pre:
 *   i and j are valid indices (0 <= i < n, 0 <= j < n)
 * Return value:
 *   The geodesic distance between i and j, or a negative number if they are not
 *  reachable.
 * */
int graph_geodesic_distance(const graph_t *g, int i, int j);
/* Calculates geodesic distance between a vertex and all other vertices.
 * 
 * Pre:
 *   i is a valid vertex index (0 <= i < n).
 *   distance is an array with dimension n.
 * Post:
 *   distance[j] is the geodesic distance between i and j, or a negative number
 *  if they are not reachable.
 * */
void graph_geodesic_vertex(const graph_t *g, int i, int *distance);
/* Calculates all geodesic distances between all pair of vertices.
 * If g is undirected, the resulting matrix is symmetric.
 * 
 * Pre:
 *   distance is a matrix with dimensions n*n
 * Post:
 *   distance[i][j] is the geodesic distance between vertices i and j, or 
 *  a negative number if they are not reachable.
 * */
void graph_geodesic_all(const graph_t *g, int **distance);
/* Calculates distribution of distances between all vertices.
 * 
 * Post:
 *   if diameter != NULL, *diameter is the largest geodesic distance among
 *  all pairs of vertices.
 * 
 *   distribution[0] == n, ie, all geodesic paths with length zero are from a 
 *  vertex to itself. Therefore, there are as many geodesic paths with length 0
 *  as vertices (n).
 * 
 *   distribution[1] == m, ie, all geodesic paths with length one are from a 
 *  vertex to an adjacent, having only one edge. Therefore, there are as many
 *  geodesic paths with length 1 as edges (m).
 * 
 *   distribution[*diameter] is the number of unreachable pairs of vertices. 
 * 
 * Return value:
 *   Array distribution with dimension diameter+1.
 * Memory deallocation:
 *   free(distribution);
 * */
int *graph_geodesic_distribution(const graph_t *g, int *diameter);

/************************ Centrality measures *********************************/
/* List all vertices' betweenness centrality.
 * 
 * Betweenness is defined as the number of geodesic paths that pass through a
 * vertex among all other pairs of vertices, weighted by the number of geodesic
 * paths that exist between each pair of vertices.
 * Formally,
 *     B_v = \sum_{s,t != v} |P_st(v)|/|P_st|
 * Where
 *   P_st is the set of geodesic paths between s and t
 *   P_st(v) is the set of geodesic paths between s and t that include v
 *   |.| is the cardinality of a set.
 * 
 * Pre:
 *   betweenness is an array with dimension n
 * Post:
 *   betweenness[v] = B_v
 * */
void graph_betweenness(const graph_t *g, double *betweenness);

/* List all vertices' eigenvector centrality.
 * 
 * The eigenvector centrality of a vertex is the sum of its incident's
 * centrality.
 * Formally,
 *     E_v = 1/L \sum{u} g_uv E_u
 * 
 * where L is a constant. This has a valid solution only if 0 <= E_i <= 1 and 
 * L is the largest eigenvalue of the adjacency matrix.
 * 
 * Pre:
 *   eigen is an array with dimension n
 * Post:
 *   eigen[v] = E_v
 * */
void graph_eigenvector(const graph_t *g, double *eigen);
/* List all vertices' PageRank centrality with damping factor alpha.
 * The usual value for alpha is 0.85.
 * 
 * The rank of a vertex is defined by its incident vertices rank, and also by
 * a random probability to walk to it.
 * Formally,
 *     R_v = (1-alpha)/n + alpha * \sum_{u} R_u g_uv / k^{out}_u
 * where
 *   alpha is the damping factor,
 *   (1 - alpha)/n is the probability to walk to any other vertex,
 *   k^{out}_u is the out-degree of vertex u
 * 
 * Pre:
 *   alpha is a valid probability (0.0 <= alpha <= 1.0)
 *   rank is an array with dimension n.
 * Post:
 *   rank[v] = R_v
 * */
void graph_pagerank(const graph_t *g, double alpha, double *rank);
/* Label vertices' k-core.
 * 
 * The k-core is the largest connected subgraph where all vertices have
 * degree at least k. The core of a vertex is the maximum k of all k-cores 
 * where it belongs to.
 * 
 * Pre:
 *   core is an array with dimension n.
 * Post:
 *   core[i] = k if it belongs to a k-core but not to a (k+1)-core.
 * Return:
 *   maximum k-core, or graph degeneracy
 * */
int graph_kcore(const graph_t *g, int *core);

/* List all vertices' closenness. */
void graph_closeness(const graph_t *g, double *closenness);

/************************ Correlation measures ********************************/
/* Calculates the distribution matrix of degrees (ki,kj).
 * 
 * Pre:
 *   g is undirected.
 * Post:
 *   if kmax != NULL, *kmax is the maximum degree.
 * Return value:
 *   Matrix A with dimensions kmax * kmax, where A[ki][kj] is the number of
 *  edges that connect a vertex with degree ki to a vertex with degree kj.
 * Memory deallocation
 *   free(A[0]); free(A);
 * */
int **graph_degree_matrix(const graph_t *g, int *kmax);
/* Returns the average degree of vertex i's neighbors
 * 
 * The neighbor's average degree <k_nn(i)> is calculated as follow:
 *   <k_nn(i)> = 1/k_i \sum_{j} k_j g_ij
 * where 
 *   k_v is the degree of vertex v.
 * 
 * Pre:
 *   i is a valid index (0 <= i < n).
 * Return value:
 *   Average degree of vertex i's neighbors.
 * */
double graph_neighbor_degree_vertex(const graph_t *g, int i);
/* Calculates neighbors' average degree for all vertices.
 * 
 * Pre:
 *   avg_degree is an array with dimension n
 * Post:
 *   avg_degree[i] = <k_nn(i)>
 * Return value:
 *   Maximum degree among all vertices.
 * */
int graph_neighbor_degree_all(const graph_t *g, double *avg_degree);
/* Returns the average neighbor's degree grouped by degree.
 * 
 * Post:
 *   if kmax != NULL, kmax is the maximum degree.
 *   knn[d] = average of <k_nn(i)>, for all vertex i such that k_i = d
 * Return value
 *   Array knn with dimension kmax.
 * Memory deallocation:
 *   free(knn);
 * */
double *graph_knn(const graph_t *g, int *kmax);
/* Returns the degree assortativity.
 * 
 * Assortativity is defined as the Pearson linear correlation coefficient of 
 * the distribution matrix of degrees (ki, kj), lying between -1 and 1. 
 * 
 * A positive assortativity indicates that vertices with high degree usually 
 * connect with vertices with high degree, and conversely that vertices with
 * lower degree usually connect to vertices with low degree.
 * 
 * A negative assortativity indicates the contrary: vertices with high degree 
 * usually connect with vertices with low degree, and vice versa.
 * 
 * Return value:
 *   Degree assortativity.
 * */
double graph_assortativity(const graph_t *g);

#endif
