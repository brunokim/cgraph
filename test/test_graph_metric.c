
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "graph.h"
#include "graph_metric.h"

/* 0 -- 1 -- 2    3
 * |              |
 * 4    5    6 -- 7
 */
void test_components(){
	int n = 8;
	bool is_directed = false;
	bool is_weighted = false;
	graph_t *graph = new_graph(n, is_directed, is_weighted);
	
	graph_add_edge(graph, 0, 1);
	graph_add_edge(graph, 0, 4);
	graph_add_edge(graph, 1, 2);
	
	graph_add_edge(graph, 3, 7);
	graph_add_edge(graph, 6, 7);
	
	int *label = malloc (n * sizeof(*label));
	int num_comp = graph_undirected_components(graph, label);
	
	assert(num_comp == 3);
	
	assert(label[0] == label[1]);
	assert(label[0] == label[2]);
	assert(label[0] == label[4]);
	
	assert(label[3] == label[6]);
	assert(label[3] == label[7]);
	
	assert(label[0] != label[5]);
	assert(label[0] != label[3]);
	assert(label[3] != label[5]);
	
	num_comp = graph_num_components(label, n);
	assert(num_comp == 3);
	
	set_t **comp = malloc(num_comp * sizeof(*comp));
	int i;
	for (i=0; i < num_comp; i++){
		comp[i] = new_set(0);
	}
	graph_components(label, n, comp, num_comp);
	
	int c0 = label[0], c1 = label[3], c2 = label[5];
	assert( set_size    (comp[c0]) == 4);
	assert( set_contains(comp[c0], 0));
	assert( set_contains(comp[c0], 1));
	assert( set_contains(comp[c0], 2));
	assert( set_contains(comp[c0], 4));
	
	assert( set_size    (comp[c1]) == 3);
	assert( set_contains(comp[c1], 3));
	assert( set_contains(comp[c1], 6));
	assert( set_contains(comp[c1], 7));
	
	assert( set_size    (comp[c2]) == 1);
	assert( set_contains(comp[c2], 5));
	
	for (i=0; i < num_comp; i++){
		delete_set(comp[i]);
	}
	free(comp);
	free(label);
	delete_graph(graph);
}

void test_giant(){
	int k[] = {5, 10, 3}, n = 18;
	bool is_directed = false;
	bool is_weighted = false;
	graph_t *g = new_graph(n, is_weighted, is_directed);
	
	int i;
	for (i=1; i < k[0]; i++){
		graph_add_edge(g, 0, i);
	}
	
	for (i=k[0]+1; i < k[0]+k[1]; i++){
		graph_add_edge(g, k[0], i);
	}
	
	for (i=k[0]+k[1]+1; i < k[0]+k[1]+k[2]; i++){
		graph_add_edge(g, k[0]+k[1], i);
	}
	//graph_print(g);
	
	graph_t *giant = graph_giant_component(g);
	
	assert(graph_num_vertices(giant) == 10);
	assert(graph_num_edges(giant) == 9);
	
	delete_graph(giant);
	delete_graph(g);
}

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

void test_degree(){
	graph_t *directed = make_a_graph(true);
	graph_t *undirected = make_a_graph(false);
	
	int n = 8;
	int in_degree[]  = {0, 1, 2, 1, 2, 2, 3, 1};
	int out_degree[] = {2, 2, 2, 3, 1, 1, 1, 0};
	int degree[]     = {2, 3, 4, 4, 3, 3, 4, 1};
	
	int *degree1 = malloc(n * sizeof(*degree));
	int *degree2 = malloc(n * sizeof(*degree));
	
	graph_degree(directed, degree1);
	graph_degree(undirected, degree2);
	
	int i;
	for (i=0; i < n; i++){
		assert(degree[i] == degree1[i]);
		assert(degree[i] == degree2[i]);
	}
	
	graph_directed_degree(directed, degree1, degree2);
	for (i=0; i < n; i++){
		assert(in_degree[i]  == degree1[i]);
		assert(out_degree[i] == degree2[i]);
	}
	
	free(degree1); free(degree2);
	delete_graph(directed);
	delete_graph(undirected);
}

void test_clustering(){
	graph_t *g = make_a_graph(false);
	
	int n = 8;
	double c[] = {1.0, 1.0/3, 1.0/6, 1.0/3, 1.0/3, 1.0/3, 1.0/3, 0.0};
	double *clustering = malloc(n * sizeof(*clustering));
	
	graph_clustering(g, clustering);
	
	int i;
	for (i=0; i < n; i++){
		assert(fabs(clustering[i] - c[i]) < 1e-6);
	}
	free(clustering);
	delete_graph(g);
}

void test_transitivity(){
	graph_t *g = make_a_graph(false);
	
	int num_triplet, num_triangle;
	graph_num_triplets(g, &num_triplet, &num_triangle);
	
	assert(num_triplet == 56);
	assert(num_triangle == 3);
	
	assert(fabs(graph_transitivy(g) - 9.0/56) < 1e-6);
	
	delete_graph(g);
}

void test_distance(){
	graph_t *g = make_a_graph(false);
	
	int expected[8][8] = {
		{0, 1, 1, 2, 2, 2, 3, 4},
		{1, 0, 1, 1, 2, 2, 2, 3},
		{1, 1, 0, 2, 1, 1, 2, 3},
		{2, 1, 2, 0, 1, 1, 1, 2},
		{2, 2, 1, 1, 0, 2, 1, 2},
		{2, 2, 1, 1, 2, 0, 1, 2},
		{3, 2, 2, 1, 1, 1, 0, 1},
		{4, 3, 3, 2, 2, 2, 1, 0}
	};
	
	int i, j, n = 8;
	for (i=0; i < n; i++){
		for (j=0; j < n; j++){
			assert(graph_geodesic_distance(g, i, j) == expected[i][j]);
		}
	}
	
	int **d = malloc(n * sizeof(*d));
	d[0] = malloc(n * n * sizeof(*d[0]));
	for (i=1; i < n; i++){
		d[i] = d[0] + i*n;
	}
	graph_geodesic_all(g, d);
	for (i=0; i < n; i++){
		for (j=0; j < n; j++){
			assert(d[i][j] == expected[i][j]);
		}
	}
	
	int expected_dist[] =  {8, 24, 24, 6, 2};
	
	int diameter;
	int *dist = graph_geodesic_distribution(g, &diameter);
	for (i=0; i < diameter; i++){
		assert(dist[i] == expected_dist[i]);
	}
	assert(dist[diameter] == 0);
	
	double *betweenness = malloc (n*sizeof(*betweenness));
	graph_betweenness(g, betweenness);
	
	free(betweenness);
	free(dist);
	free(d[0]); free(d);
	delete_graph(g);
}

graph_t *make_a_cycle(int n){
	graph_t *g = new_graph(n, false, false);
	
	int i;
	for (i=0; i < n-1; i++){
		graph_add_edge(g, i, i+1);
	}
	graph_add_edge(g, n-1, 0);
	
	return g;
}

graph_t *make_a_line(int n){
	graph_t *g = new_graph(n, false, false);
	
	int i;
	for (i=0; i < n-1; i++){
		graph_add_edge(g, i, i+1);
	}
	
	return g;
}

graph_t *make_a_star(int n){
	graph_t *g = new_graph(n, false, false);
	
	int i;
	for (i=1; i < n; i++){
		graph_add_edge(g, 0, i);
	}
	
	return g;
}

graph_t *make_a_clique(int n){
	graph_t *g = new_graph(n, false, false);
	
	int i, j;
	for (i=0; i < n; i++){
		for (j=i+1; j < n; j++){
			graph_add_edge(g, i, j);
		}
	}
	
	return g;
}

void test_betweenness(){
	int i, n, max = 100;
	double *betweenness = malloc(max * sizeof(*betweenness));
	
	/* Star: all other vertices connected to v0 
	 * B_center = (n-1)(n-2)
	 */
	for (n=3; n < max; n++){
		graph_t *star = make_a_star(n);
		int expected = 2*(n-1)*(n-2);
		graph_betweenness(star, betweenness);
		assert(fabs(betweenness[0] - expected) < 1e-6);
		for (i=1; i < n; i++){
			assert(fabs(betweenness[i]) < 1e-6);
		}
		delete_graph(star);
	}
	
	/* Clique: all vertices are connected to all others
	 * All vertices should have betweenness 0
	 */
	for (n=2; n < max; n++){
		graph_t *clique = make_a_clique(n);
		graph_betweenness(clique, betweenness);
		for (i=0; i < n; i++){
			assert(fabs(betweenness[i]) < 1e-6);
		}
		delete_graph(clique);
	}
	
	/* Line: vertices are connected in a line
	 * Middle betweenness should be (n-1)^2/2
	 */
	for (n=3; n < max; n+=2){
		graph_t *line = make_a_line(n);
		int expected = (n-1)*(n-1);
		graph_betweenness(line, betweenness);
		assert(fabs(betweenness[n/2] - expected) < 1e-6);
		delete_graph(line);
	}
	
	/* Cycle: vertices are connected in a cycle
	 * All betweenness are equal, and should be equal to the same as in a line
	 * with half its size: (n/2-1)*(n/2-1)
	 */
	for (n=3; n < max; n++){
		graph_t *cycle = make_a_cycle(n);
		//int expected = (n/2-1)*(n/2-1);
		graph_betweenness(cycle, betweenness);
		for (i=1; i < n; i++){
			//assert(fabs(betweenness[i] - expected) < 1e-6);
		}
		delete_graph(cycle);
	}
	
	/* Custom graph: do not know how betweenness should be previously */
	graph_t *g = make_a_graph(false);
	graph_betweenness(g, betweenness);
	n = 8;
	for (i=0; i < n; i++){
		printf("%lf ", betweenness[i]);
	}
	printf("\n");
	
	free(betweenness);
	delete_graph(g);
}

void test_parallel_betweenness(){
	int i, j, n = 997;
	int num_processors = 8;
	double *single = malloc(n * sizeof(*single));
	double *multi = malloc(n * sizeof(*multi));
	
	graph_t *star = make_a_star(n);
	graph_t *line = make_a_line(n);
	graph_t *cycle = make_a_cycle(n);
	graph_t *erdos = new_graph(n, false, false);
	
	unsigned int seed = 991784611L;
	for (i=0; i < n; i++){
		for (j=i+1; j < n; j++){
			double p = 10.0/n;
			if (rand_r(&seed) < p*RAND_MAX){
				graph_add_edge(erdos, i, j);
			}
		}
	}
	
	graph_t *graphs[] = {star, line, cycle, erdos};
	int num_graphs = sizeof(graphs)/sizeof(graphs[0]);
	
	for (i=0; i < num_graphs; i++){
		graph_betweenness(graphs[i], single);
		graph_parallel_betweenness(graphs[i], multi, num_processors);
		for (j=0; j < n; j++){
			assert(fabs(single[j] - multi[j]) < 1e-6);
		}
		free(graphs[i]);
	}
	
	free(single); free(multi);
}

typedef struct {
	graph_t *g;
	int *core;
	int k;
} kcore_test_t;

kcore_test_t small_core(){
	int n = 5;
	graph_t *g = new_graph(n, false, false);
	graph_add_edge(g, 1, 2);
	graph_add_edge(g, 2, 3);
	graph_add_edge(g, 2, 4);
	graph_add_edge(g, 3, 4);
	
	int *core = malloc(n * sizeof(*core));
	core[0] = 0;
	core[1] = 1;
	core[2] = core[3] = core[4] = 2;
	
	kcore_test_t test = {g, core, 2};
	return test;
}

kcore_test_t medium_core(){
	int n = 15;
	graph_t *g = new_graph(n, false, false);
	
	int d[15] = {7, 6, 6, 7, 5, 3, 3, 3, 2, 3, 3, 2, 2, 2, 2};
	int adj[15][7] = {
		// 3-Core
		/*0:*/ {1, 2, 3, 4, 8, 9, 11},
		/*1:*/ {0, 2, 3, 4, 5, 9},
		/*2:*/ {0, 1, 3, 4, 6, 14},
		/*3:*/ {0, 1, 2, 5, 6, 7, 10},
		/*4:*/ {0, 1, 2, 7, 8},
		
		// 2-Core
		/*5:*/ {1, 3, 12},
		/*6:*/ {2, 3, 12},
		/*7:*/ {3, 4, 10},
		/*8:*/ {0, 4},
		/*9:*/ {0, 1, 11},
		/*10:*/ {3, 7, 13},
		/*11:*/ {0, 9},
		/*12:*/ {5, 6},
		/*13:*/ {6, 10},
		/*14:*/ {2, 12}
	};
	
	int i, j;
	for (i=0; i < n; i++){
		for (j=0; j < d[i]; j++){
			graph_add_edge(g, i, adj[i][j]);
		}
	}
	
	int *core = malloc(n * sizeof(*core));
	for (i=0; i < 5; i++){ core[i] = 3; }
	for (i=5; i < n; i++){ core[i] = 2; }
	
	kcore_test_t test = {g, core, 3};
	return test;
}

kcore_test_t bigger_core(){
	int n = 20;
	graph_t *g = new_graph(n, false, false);
	
	int *core = malloc(n * sizeof(*core));
	
	int i, j;
	// 4-core: (0, 1, 2, 3, 4)
	for (i=0; i < 5; i++){
		for (j=i+1; j < 5; j++){
			graph_add_edge(g, i, j);
		}
		core[i] = 4;
	}
	
	// 3-core: (5, 6, 7, 8, 9)
	for (i=5; i < 10; i++){
		int initial = i-5;
		for (j=initial; j < initial+3; j++){
			graph_add_edge(g, i, j);
		}
		core[i] = 3;
	}
	
	// 2-core: (10, 11, 12, 13, 14)
	for (i=10; i < 15; i++){
		int initial = i-5;
		for (j=initial; j < initial+2; j++){
			graph_add_edge(g, i, j);
		}
		core[i] = 2;
	}
	
	// 1-core: (15, 16, 17, 18, 19)
	for (i=15; i < 20; i++){
		graph_add_edge(g, i, i-5);
		core[i] = 1;
	}
	
	kcore_test_t test = {g, core, 4};
	return test;
}

kcore_test_t tree(){
	int n = 100;
	graph_t *g = new_graph(n, false, false);
	int *core = malloc (n * sizeof(*core));
	int i;
	core[0] = 1;
	for (i=1; i < n; i++){
		graph_add_edge(g, i, rand() % i);
		core[i] = 1;
	}
	kcore_test_t test = {g, core, 1};
	return test;
}

void test_kcore(){
	
	kcore_test_t test[] = {small_core(), medium_core(), bigger_core(), tree()};
	int num_tests = sizeof(test)/sizeof(test[0]);
	
	int t;
	for (t=0; t < num_tests; t++){
		graph_t *g = test[t].g;
		int i, n = graph_num_vertices(g);
		
		int *expected_core = test[t].core;
		int expected_k = test[t].k;
		
		int *core = malloc(n * sizeof(*core));
		
		int k = graph_kcore(g, core);
		assert(k == expected_k);
		
		for (i=0; i < n; i++){
			assert(core[i] == expected_core[i]);
		}
		
		free(core);
		delete_graph(g);
		free(expected_core);
	}
}

int main(){
	test_components();
	test_giant();
	test_degree();
	test_clustering();
	test_transitivity();
	test_distance();
	test_betweenness();
	test_kcore();
	test_parallel_betweenness();
	printf("success\n");
	return 0;
}
