#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include "sorting.h"
#include "stat.h"
#include "graph.h"
#include "graph_metric.h"

void *experiment(void *args);

int main(int argc, char *argv[]){
	if (argc == 1){
		printf("Usage: experiment <folders>\n"
		       "       Each folder should have a file called edges.txt\n");
		exit(EXIT_SUCCESS);
	}
	
	int i, n = argc-1;
	pthread_t *thread = malloc(n * sizeof(*thread));
	
	for (i=0; i < n; i++){
		pthread_create(&thread[i], NULL, experiment, argv[i+1]);
	}
	fprintf(stderr, "%d experiments launched\n", n);
	
	for (i=0; i < n; i++){
		void *dummy;
		pthread_join(thread[i], &dummy);
	}
	
	free(thread);
	printf("success\n");
	return 0;
}

void *experiment(void *args){
	const char *folder = (char *)args;
	char str[256];
	
	int i, j;
	snprintf(str, 256, "%s/summary.txt", folder);
	FILE *f_summary = fopen(str, "wt");
	
	// General information
	bool is_directed = false;
	snprintf(str, 256, "%s/edges.txt", folder);
	graph_t *complete = load_graph(str, is_directed);
	int n = graph_num_vertices(complete);
	int m = graph_num_edges(complete);
	fprintf(f_summary, "number of vertices = %d\n", n);
	fprintf(f_summary, "number of edges = %d\n", m);
	
	// Component information
	int *label = malloc(n * sizeof(*label));
	int num_comp = graph_undirected_components(complete, label);
	fprintf(f_summary, "number of components = %d\n", num_comp);
	fprintf(f_summary, "component sizes = (");
	pair_t *component_size = stat_frequencies(label, n, &num_comp);
	for (i=0; i < num_comp; i++){
		const char *interposed = (i == num_comp-1) ? ")\n" : " ";
		fprintf(f_summary, "%d%s", component_size[i].value, interposed);
	}
	pair_t *max = search_max(component_size, num_comp, sizeof(*max), comp_value_asc);
	fprintf(f_summary, "giant component size = %d\n", max->value);
	fprintf(f_summary, "percent of vertices in giant component = %.2lf\n", 
	                   (100.0*max->value)/n);
	free(component_size);
	free(label);
	
	// Giant component extraction
	graph_t *g = graph_giant_component(complete);
	delete_graph(complete);
	n = graph_num_vertices(g);
	m = graph_num_edges(g);
	
	FILE *fp;
	
	// Degree distribution
	int *degree = malloc(n * sizeof(*degree));
	fprintf(stderr, "Calculating degree in %s...\n", folder);
	graph_degree(g, degree);
	
	fprintf(f_summary, "degree average = %.3lf\n", stat_int_average(degree, n));
	fprintf(f_summary, "degree variance = %.3lf\n", stat_int_variance(degree, n));
	fprintf(f_summary, "degree stddev = %.3lf\n", sqrt(stat_int_variance(degree, n)));
	fprintf(f_summary, "degree entropy = %.3lf\n", stat_int_entropy(degree, n));
	
	int num_deg; pair_t *dist_degree = stat_frequencies(degree, n, &num_deg);
	snprintf(str, 256, "%s/degree.dat", folder); fp = fopen(str, "wt");
	for (i=0; i < num_deg; i++){
		fprintf(fp, "%d %d\n", dist_degree[i].key, dist_degree[i].value);
	}
	fclose(fp);
	/*free(degree); */ // Still needed for centralities
	free(dist_degree);
	
	// Clustering coefficient distribution
	double *clustering = malloc(n * sizeof(*clustering));
	fprintf(stderr, "Calculating clustering in %s...\n", folder);
	graph_clustering(g, clustering);
	fprintf(f_summary, "clustering average = %.3lf\n", stat_double_average(clustering, n));
	fprintf(f_summary, "transitivy = %.3lf\n", graph_transitivy(g));
	
	int num_bins = 100; 
	interval_t *dist_cluster = stat_histogram(clustering, n, num_bins);
	
	snprintf(str, 256, "%s/clustering.dat", folder); fp = fopen(str, "wt");
	for (i=0; i < num_bins; i++){
		if (dist_cluster[i].value > 0){
			fprintf(fp, "%lf %lf %d\n", dist_cluster[i].min, dist_cluster[i].max, 
																	dist_cluster[i].value);
		}
	}
	fclose(fp);
	free(clustering); free(dist_cluster);
	
	// Degree correlations
	int kmax;
	fprintf(stderr, "Calculating degree distribution in %s...\n", folder);
	
	fprintf(f_summary, "assortativity = %+.3lf\n", graph_assortativity(g));
	
	int **mat = graph_degree_matrix(g, &kmax);
	if (mat){
		int max_dist = 0;
		for (i=0; i < kmax*kmax; i++){
			if (mat[0][i] > max_dist){ max_dist = mat[0][i]; }
		}
		
		snprintf(str, 256, "%s/deg_matrix.pbm", folder); fp = fopen(str, "wt");
		fprintf(fp, "P2\n%d %d\n%d\n", kmax, kmax, max_dist);
		for (i=0; i < kmax; i++){
			for (j=0; j < kmax; j++){
				fprintf(fp, "%4d ", max_dist-mat[i][j]);
			}
			fprintf(fp, "\n");
		}
		fclose(fp); 
		free(mat[0]); free(mat);
	}
	
	double *avg_degree = malloc(n * sizeof(*avg_degree));
	graph_neighbor_degree_all(g, avg_degree);
	snprintf(str, 256, "%s/correlation.dat", folder); fp = fopen(str, "wt");
	for (i=0; i < n; i++){
		fprintf(fp, "%d %.3lf\n", graph_num_adjacents(g, i), avg_degree[i]);
	}
	fclose(fp);
	
	double *knn = graph_knn(g, &kmax);
	snprintf(str, 256, "%s/knn.dat", folder); fp = fopen(str, "wt");
	for (i=0; i < kmax; i++){
		if (knn[i] > 0.0){
			fprintf(fp, "%d %.3lf\n", i, knn[i]);
		}
	}
	fclose(fp);
	free(avg_degree);
	free(knn);
	
	// Betweenness distribution
	double *betweenness = malloc(n * sizeof(*betweenness));
	fprintf(stderr, "Calculating betweenness in %s...\n", folder);
	
	graph_betweenness(g, betweenness);
	stat_double_normalization(betweenness, n);
	fprintf(f_summary, "betweenness average = %+.3lg\n", stat_double_average(betweenness, n));
	
	double cpd = (n - stat_double_sum(betweenness, n))/(n-1);
	fprintf(f_summary, "central point dominance = %+.3lg\n", cpd);
	
	num_bins = 100; 
	interval_t *dist_between = stat_histogram(betweenness, n, num_bins);
	snprintf(str, 256, "%s/betweenness.dat", folder); fp = fopen(str, "wt");
	for (i=0; i < num_bins; i++){
		if (dist_between[i].value > 0){
			fprintf(fp, "%lf %lf %d\n", dist_between[i].min, dist_between[i].max, 
																	dist_between[i].value);
		}
	}
	fclose(fp);
	/*free(betweenness);*/ // Still needed for centralities
	free(dist_between);
	
	// Distance 
	int diameter;
	int *distance = graph_geodesic_distribution(g, &diameter);
	fprintf(f_summary, "distance average = %+.3lf\n", stat_int_dist_average(distance, diameter));
	fprintf(f_summary, "efficiency = %.3lf\n", stat_int_dist_harmonic_sum(distance, diameter)/(n*(n-1)));
	fprintf(f_summary, "diameter = %d\n", diameter);
	
	snprintf(str, 256, "%s/distance.dat", folder); fp = fopen(str, "wt");
	for (i=0; i < diameter; i++){
		fprintf(fp, "%d %d\n", i, distance[i]);
	}
	fclose(fp);
	free(distance);
	
	// Centralities
	fprintf(stderr, "Computing centralities in %s\n", folder);
	double *eigenvector = malloc(n * sizeof(*eigenvector));
	double *pagerank = malloc(n * sizeof(*pagerank));
	double *closenness = malloc(n * sizeof(*closenness));
	int *core = malloc(n * sizeof(*core));
	
	graph_eigenvector(g, eigenvector);
	graph_pagerank(g, 0.15, pagerank);
	graph_closeness(g, closenness);
	int degeneracy = graph_kcore(g, core);
	
	snprintf(str, 256, "%s/centrality.dat", folder); fp = fopen(str, "wt");
	fprintf(fp, "#vertex degree betwenness eigenvalue pagerank closenness kcore\n");
	for (i=0; i < n; i++){
		fprintf(fp, "%d ", i);
		fprintf(fp, "%lf ", (double)degree[i]/kmax);
		fprintf(fp, "%lf ", betweenness[i]);
		fprintf(fp, "%lf ", eigenvector[i]);
		fprintf(fp, "%lf ", pagerank[i]);
		fprintf(fp, "%lf ", closenness[i]);
		fprintf(fp, "%d ", core[i]);
		fprintf(fp, "\n");
	}
	
	fprintf(f_summary, "degeneracy = %d\n", degeneracy);
	
	// Centrality correlation
	fprintf(stderr, "Computing centralities correlation in %s\n", folder);
	double *d = malloc(n * sizeof(*d));
	double *k = malloc(n * sizeof(*k));
	
	for (i=0; i < n; i++){
		d[i] = (double)degree[i]/kmax;
		k[i] = (double)core[i]/degeneracy;
	}
	
	double *centrality[] = {d, betweenness, eigenvector, pagerank, closenness, k};
	int num_centrality = sizeof(centrality)/sizeof(centrality[0]);
	
	fprintf(f_summary, "\nCentralities correlation\n");
	for (i=0; i < num_centrality; i++){
		for (j=0; j < num_centrality; j++){
			if (j > i){
				double r = stat_pearson(centrality[i], centrality[j], n);
				fprintf(f_summary, "%+6.3lf ", r);
			} else {
				fprintf(f_summary, "       ");
			}
		}
		fprintf(f_summary, "\n");
	}
	
	free(d);
	free(k);
	free(degree);
	free(betweenness);
	free(eigenvector);
	free(closenness);
	free(pagerank);
	free(core);
	
	delete_graph(g);
	fclose(f_summary);
	fprintf(stderr, "Processing in %s completed\n", folder);
	return NULL;
}
