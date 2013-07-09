#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "set.h"
#include "graph.h"
#include "graph_community.h"

/*    1 --- 3 --- 5
 *   /|     |\  _/|
 *  / |     |_\/  |
 * 0  |   _/|  \  |  7
 *  \ | _/  |   \ | /
 *   \|/    |    \|/
 *    2 --- 4 --- 6
 */
graph_t * make_a_graph(bool is_directed){
	int is_weighted = false;
	int n = 8;
	graph_t *g = new_graph(n, is_weighted, is_directed);
	
	graph_add_edge(g, 0, 1);
	graph_add_edge(g, 0, 2);
	graph_add_edge(g, 1, 2);
	
	graph_add_edge(g, 1, 3);
	graph_add_edge(g, 2, 4);
	graph_add_edge(g, 2, 5);
	
	graph_add_edge(g, 3, 4);
	graph_add_edge(g, 3, 5);
	graph_add_edge(g, 3, 6);
	graph_add_edge(g, 4, 6);
	graph_add_edge(g, 5, 6);
	graph_add_edge(g, 6, 7);
	
	return g;
}

void test_matrix(){
	graph_t *g = make_a_graph(false);
	int num_comm = 2;
	int community[8] = {0, 0, 0, 1, 1, 1, 1, 1};
	
	set_t *mapping = new_set(0);
	int **e = graph_community_matrix(g, &community[0], mapping);
	assert(set_size(mapping) == num_comm);
	
	assert(e[0][0] == 3);
	assert(e[0][1] == 3);
	assert(e[1][0] == 3);
	assert(e[1][1] == 6);
	
	free(e[0]); free(e);
	delete_set(mapping);
	delete_graph(g);
}

void test_modularity(){
	graph_t *g = make_a_graph(false);
	int community[8] = {0, 0, 0, 1, 1, 1, 1, 1};
	
	double modularity = graph_modularity(g, community);
	assert(fabs(modularity + 1.0/16) < 1e-6);
	delete_graph(g);
}

int main(){
	test_matrix();
	test_modularity();
	
	printf("success\n");
	return 0;
}
