#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "graph_model.h"

void degree_dist(const graph_t *g, const char *filename){
	int i, n = graph_num_vertices(g);
	
	int *d = malloc(n * sizeof(*d));
	memset(d, 0, n * sizeof(*d));
	for (i=0; i < n; i++){
		d[graph_num_adjacents(g, i)]++;
	}
	
	FILE *fp = fopen(filename, "wt");
	for (i=0; i < n; i++){
		if (d[i] > 0){
			fprintf(fp, "%d %d\n", i, d[i]);
		}
	}
	fclose(fp);
	free(d);
}

void test_erdos_renyi(){
	int n = 1000; 
	double k = 4.0;
	graph_t *g = new_erdos_renyi(n, k);
	degree_dist(g, "test/test_erdos_renyi.dat");
	delete_graph(g);
}

void test_watts_strogatz(){
	int n = 1000, k = 4;
	double beta = 0.25;
	graph_t *g;
	g = new_watts_strogatz(n, k, beta);
	degree_dist(g, "test/test_watts_strogatz.dat");
	delete_graph(g);
	
	g = new_watts_strogatz(n, k, 0.0);
	degree_dist(g, "test/test_watts_strogatz_structured.dat");
	delete_graph(g);
	
	g = new_watts_strogatz(n, k, 1.0);
	degree_dist(g, "test/test_watts_strogatz_random.dat");
	delete_graph(g);
}

void test_barabasi_albert(){
	int n = 1000, k = 4;
	graph_t *g = new_barabasi_albert(n, k);
	degree_dist(g, "test/test_barabasi_albert.dat");
	delete_graph(g);
}

void test_ravasz_barabasi(){
	int k = 4, l = 5;
	graph_t *g = new_ravasz_barabasi(l, k);
	degree_dist(g, "test/test_ravasz_barabasi.dat");
	delete_graph(g);
}

int main(){
	test_erdos_renyi();
	test_barabasi_albert();
	test_watts_strogatz();
	test_ravasz_barabasi();
	
	printf("success\n");
	return 0;
}
