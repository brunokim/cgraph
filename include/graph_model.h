#ifndef _GRAPH_MODEL_H
#define _GRAPH_MODEL_H

#include "graph.h"

/***************************** Model creation *********************************/
/** The reentrant version accepts a pointer to a state variable to be passed
 * to rand_r.
 */

// Creates a complete network with n vertices
graph_t *new_clique(int n);
// Creates a random network with n vertices and average degree k.
graph_t *new_erdos_renyi(int n, double k);
graph_t *new_erdos_renyi_r(int n, double k, unsigned int *seedp);
// Creates a small-world network with n vertices and average degree k, with
//rewiring probability beta.
graph_t *new_watts_strogatz(int n, int k, double beta);
graph_t *new_watts_strogatz_r(int n, int k, double beta, unsigned int *seedp);
// Creates a scale-free network with n vertices and average degree k.
graph_t *new_barabasi_albert(int n, int k);
graph_t *new_barabasi_albert_r(int n, int k, unsigned int *seedp);
// Creates a scale-free network with modular structure and k^l vertices.
graph_t *new_ravasz_barabasi(int l, int k);

#endif
