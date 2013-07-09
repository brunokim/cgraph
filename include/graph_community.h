#ifndef _GRAPH_COMMUNITY_H
#define _GRAPH_COMMUNITY_H

#include "graph.h"

int **graph_community_matrix(graph_t *g, int *community, set_t *mapping);
double graph_modularity(graph_t *g, int *community);

void graph_fast_newman(graph_t *g, int *community);

#endif
