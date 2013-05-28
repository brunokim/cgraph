#include <assert.h>
#include <stdlib.h>
#include "graph.h"

/*
 * 0 -- 1 -- 2
 * |  /   \   
 * | /     \  
 * 3 ------- 4
 */
void test_basic(){
	const int n = 5;
	bool is_weighted = false;
	bool is_directed = false;
	graph_t *g = new_graph(n, is_weighted, is_directed);
	
	graph_add_edge(g, 0, 1);
	graph_add_edge(g, 0, 3);
	graph_add_edge(g, 1, 2);
	graph_add_edge(g, 1, 3);
	graph_add_edge(g, 1, 4);
	graph_add_edge(g, 3, 4);
	
	bool adj[5][5] = {
		{0, 1, 0, 1, 0},
		{1, 0, 1, 1, 1},
		{0, 1, 0, 0, 0},
		{1, 1, 0, 0, 1},
		{0, 1, 0, 1, 0}
	};
	
	int i, j;
	for (i=0; i < n; i++){
		for (j=0; j < n; j++){
			assert(graph_is_adjacent(g, i, j) == adj[i][j]);
		}
	}
	// graph_print(g);
	delete_graph(g);
}

void test_input(){
	char *filename = "datasets/powergrid/edges.txt";
	graph_t *g = load_graph(filename, true);
	
	int n = graph_num_vertices(g);
	int m = graph_num_edges(g);
	assert(n == 4941);
	assert(m == 6594);
	
	delete_graph(g);
}

void test_copy(){
	const int n = 10;
	bool is_weighted = false;
	bool is_directed = false;
	graph_t *g = new_graph(n, is_weighted, is_directed);
	
	int i, j;
	for (i=0; i < n; i++){
		for (j=0; j < 4; j++){
			graph_add_edge(g, i, rand() % n);
		}
	}
	
	graph_t *copy = graph_copy(g);
	//printf("<< g >>\n"); graph_print(g); printf("\n\n");
	//printf("<< copy >>\n"); graph_print(copy); printf("\n\n");
	
	for (i=0; i < n; i++){
		for (j=0; j < n; j++){
			assert(graph_is_adjacent(g, i, j) == graph_is_adjacent(copy, i, j));
		}
	}
	
	delete_graph(g);
	delete_graph(copy);
}

/* 0 -- 1 -- 2   3 -- 4
 * | \/        /   \    
 * | /\       /     \   
 * 5    6 -- 7   8 -- 9
 * 
 * vertices: {0, 2, 3, 6, 7, 8, 9}
 * 0         2   3     
 *   \         /   \    
 *    \       /     \   
 *      6 -- 7   8 -- 9
 * 
 * New indices:
 * 0         1   2     
 *   \         /   \    
 *    \       /     \   
 *      3 -- 4   5 -- 6
 * 
 */
void test_subset(){
	bool is_weighted = false;
	bool is_directed = false;
	graph_t *g = new_graph(10, is_weighted, is_directed);
	
	graph_add_edge(g, 0, 1);
	graph_add_edge(g, 0, 5);
	graph_add_edge(g, 0, 6);
	graph_add_edge(g, 1, 2);
	graph_add_edge(g, 1, 5);
	graph_add_edge(g, 3, 4);
	graph_add_edge(g, 3, 7);
	graph_add_edge(g, 3, 9);
	graph_add_edge(g, 6, 7);
	graph_add_edge(g, 8, 9);
	
	int n = 7;
	set_t *vertices = new_set(n);
	set_put(vertices, 0);
	set_put(vertices, 2);
	set_put(vertices, 3);
	set_put(vertices, 6);
	set_put(vertices, 7);
	set_put(vertices, 8);
	set_put(vertices, 9);
	
	graph_t *subgraph = graph_subset(g, vertices);
	bool adj[7][7] = {
		{0, 0, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 0, 1},
		{1, 0, 0, 0, 1, 0, 0},
		{0, 0, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 1},
		{0, 0, 1, 0, 0, 1, 0}
	};
	
	int i, j;
	for (i=0; i < n; i++){
		for (j=0; j < n; j++){
			assert(graph_is_adjacent(subgraph, i, j) == adj[i][j]);
		}
	}
	
	delete_set(vertices);
	delete_graph(g);
	delete_graph(subgraph);
}

int main(){
	srand(42);
	test_basic();
	test_input();
	test_copy();
	test_subset();
	printf("success\n");
	return 0;
}
