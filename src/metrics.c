#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include "sorting.h"
#include "stat.h"
#include "graph.h"
#include "graph_metric.h"

#ifndef NUM_PROCESSORS
	#define NUM_PROCESSORS 8
#endif

enum {
	DEGREE, CLUSTERING, AVG_DEGREE, BETWEENNESS, 
	EIGENVECTOR, PAGERANK, CLOSENNESS, K_CORE,
	NUM_METRIC
};
bool is_int[NUM_METRIC] = {
	true, false, false, false, 
	false, false, false, true
};

#define MAX_NAME_SIZE 19

const char *metrics_name[] = {
	"degree", "clustering", "avg-neighbor-degree", "betweenness", 
	"eigenvector", "pagerank", "closenness", "k-core"
}; 

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

void general_info(FILE *summary, graph_t *g){
	fprintf(summary, "number of vertices = %d\n", graph_num_vertices(g));
	fprintf(summary, "number of edges = %d\n", graph_num_edges(g));
}

void component_info(FILE *summary, graph_t *g){
	int n = graph_num_vertices(g);
	
	int *label = malloc(n * sizeof(*label));
	int num_comp = graph_undirected_components(g, label);
	fprintf(summary, "number of components = %d\n", num_comp);
	
	fprintf(summary, "component sizes = (");
	pair_t *component_size = stat_frequencies(label, n, &num_comp);
	int i;
	for (i=0; i < num_comp; i++){
		const char *interposed = (i == num_comp-1) ? ")\n" : " ";
		fprintf(summary, "%d%s", component_size[i].value, interposed);
	}
	
	pair_t *max = 
		search_max(component_size, num_comp, sizeof(*max), comp_value_asc);
	fprintf(summary, "giant component size = %d\n", max->value);
	fprintf(summary, "percent of vertices in giant component = %.2lf\n", 
	                   (100.0*max->value)/n);
	
	free(component_size);
	free(label);
}

void degree_info(FILE *summary, graph_t *g, double **metrics){
	int n = graph_num_vertices(g);
	
	int *degree = malloc(n * sizeof(*degree));
	graph_degree(g, degree);
	
	int max = *(int *)search_max(degree, n, sizeof(*degree), comp_int_asc);
	double avg = stat_int_average(degree, n);
	double var = stat_int_variance(degree, n);
	double entropy = stat_int_entropy(degree, n);
	
	fprintf(summary, "maximum degree = %d\n", max);
	fprintf(summary, "degree average = %.3lf\n", avg);
	fprintf(summary, "degree variance = %.3lf\n", var);
	fprintf(summary, "degree stddev = %.3lf\n", sqrt(var));
	fprintf(summary, "degree entropy = %.3lf\n", entropy);
	
	int i;
	for (i=0; i < n; i++){
		metrics[DEGREE][i] = (double)degree[i];
	}
	free(degree);
}

void clustering_info
		(FILE *summary, graph_t *g, const char *folder, double **metrics){
	int n = graph_num_vertices(g);
	
	double *clustering = metrics[CLUSTERING];
	graph_clustering(g, clustering);
	
	double avg = stat_double_average(clustering, n);
	double transitivity = graph_transitivy(g);
	
	fprintf(summary, "clustering average = %.3lf\n", avg);
	fprintf(summary, "transitivy = %.3lf\n", transitivity);
}

void correlation_info
		(FILE *summary, graph_t *g, const char *folder, double **metrics){
	int i, j;;
	
	double assortativity = graph_assortativity(g);
	fprintf(summary, "assortativity = %+.3lf\n", assortativity);
	
	char str[256]; 
	FILE *fp;
	
	int kmax;
	int **mat = graph_degree_matrix(g, &kmax);
	if (mat){
		int max_dist = 0;
		for (i=0; i < kmax*kmax; i++){
			if (mat[0][i] > max_dist){ max_dist = mat[0][i]; }
		}
		
		// Print cross-degree distribution in PBM format
		snprintf(str, 256, "%s/deg_matrix.pbm", folder); 
		fp = fopen(str, "wt");
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
	
	double *avg_degree = metrics[AVG_DEGREE];
	graph_neighbor_degree_all(g, avg_degree);
	
	double *knn = graph_knn(g, &kmax);
	snprintf(str, 256, "%s/knn.dat", folder); 
	fp = fopen(str, "wt");
	for (i=0; i < kmax; i++){
		if (knn[i] > 0.0){
			fprintf(fp, "%d %.3lf\n", i, knn[i]);
		}
	}
	fclose(fp);
	free(knn);
}

void betweenness_info(FILE *summary, graph_t *g, double **metrics){
	int n = graph_num_vertices(g);
	double *betweenness = metrics[BETWEENNESS];
	
	if (n < 4000)
	{
		graph_betweenness(g, betweenness);
	} 
	else
	{
		graph_parallel_betweenness(g, betweenness, NUM_PROCESSORS);
	}
	
	//stat_double_normalization(betweenness, n);
	double max = 
		*(double *)search_max(betweenness, n, sizeof(max), comp_double_asc);
	double avg = stat_double_average(betweenness, n);
	fprintf(summary, "maximum betweenness = %.3lf\n", max);
	fprintf(summary, "betweenness average = %.3lf\n", avg);
	
	double cpd = 0.0;
	int i;
	for (i=0; i < n; i++){
		cpd += (max - betweenness[i])/(n-1);
	}
	fprintf(summary, "central point dominance = %.3lf\n", cpd);
}

void distance_info(FILE *summary, graph_t *g, const char *folder){
	int n = graph_num_vertices(g);
	int diameter;
	int *distance = graph_geodesic_distribution(g, &diameter);
	
	double avg = stat_int_dist_average(distance, diameter);
	double eff = stat_int_dist_harmonic_sum(distance, diameter)/(n*(n-1));
	fprintf(summary, "distance average = %.3lf\n", avg);
	fprintf(summary, "efficiency = %.3lf\n", eff);
	fprintf(summary, "diameter = %d\n", diameter);
	
	char str[256]; snprintf(str, 256, "%s/distance.dat", folder); 
	FILE *fp = fopen(str, "wt");
	int i;
	for (i=0; i < diameter; i++){
		fprintf(fp, "%d %d\n", i, distance[i]);
	}
	fclose(fp);
	free(distance);
}

void centrality_info
		(FILE *summary, graph_t *g, const char *folder, double **metrics){
	int i, j, n = graph_num_vertices(g);
	
	double *eigenvector = metrics[EIGENVECTOR];
	double *pagerank = metrics[PAGERANK];
	double *closenness = metrics[CLOSENNESS];
	int *k = malloc(n * sizeof(*k));
	double *core = metrics[K_CORE];
	
	graph_eigenvector(g, eigenvector);
	graph_pagerank(g, 0.15, pagerank);
	graph_closeness(g, closenness);
	int degeneracy = graph_kcore(g, k);
	
	fprintf(summary, "degeneracy = %d\n", degeneracy);
	
	for (i=0; i < n; i++){
		core[i] = (double)k[i];
	}
	free(k);
	
	// Metrics correlation
	fprintf(summary, "\nMetrics correlation\n");
	fprintf(summary, "%*s ", MAX_NAME_SIZE, " ");
	for (i=0; i < NUM_METRIC; i++){
		fprintf(summary, "%*s ", MAX_NAME_SIZE, metrics_name[i]);
	}
	fprintf(summary, "\n");
	
	for (i=0; i < NUM_METRIC; i++){
		fprintf(summary, "%*s ", MAX_NAME_SIZE, metrics_name[i]);
		for (j=0; j < NUM_METRIC; j++){
			double r = stat_pearson(metrics[i], metrics[j], n);
			fprintf(summary, "%+*.6lf ", MAX_NAME_SIZE, r);
		}
		fprintf(summary, "\n");
	}
}

void print_metrics(const char *folder, double **metrics, int n){
	int i, j;
	char str[256]; snprintf(str, 256, "%s/metrics.dat", folder);
	FILE *fp = fopen(str, "wt");
	
	fprintf(fp, "%*s ", MAX_NAME_SIZE, "#vertex");
	for (j=0; j < NUM_METRIC; j++){
		fprintf(fp, "%*s ", MAX_NAME_SIZE, metrics_name[j]);
	}
	fprintf(fp, "\n");
	
	
	for (i=0; i < n; i++){
		fprintf(fp, "%*d ", MAX_NAME_SIZE, i);
		for (j=0; j < NUM_METRIC; j++){
			double val = metrics[j][i];
			if (is_int[j]){ fprintf(fp, "%*.0lf ", MAX_NAME_SIZE, val); }
			else          { fprintf(fp, "%*.6le ", MAX_NAME_SIZE, val); }
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void print_histograms(const char *folder, double **metrics, int n){
	int i, j;
	
	for (i=0; i < NUM_METRIC; i++){
		char str[256]; snprintf(str, 256, "%s/%s.dat", folder, metrics_name[i]);
		FILE *fp = fopen(str, "wt");
		
		int num_bins = 20;
		if (is_int[i]){
			double min = metrics[i][0], max = metrics[i][0];
			for (j=1; j < n; j++){
				if (metrics[i][j] < min){ min = metrics[i][j]; }
				if (metrics[i][j] > max){ max = metrics[i][j]; }
			}
			num_bins = (int)(max - min);
		}
		interval_t *hist = stat_histogram(metrics[i], n, num_bins);
		
		for (j=0; j < num_bins-2; j++){
			double x = is_int[i] ? hist[j].min : (hist[j].min + hist[j].max)/2;
			fprintf(fp, "%le %d\n", x, hist[j].value);
		}
		double x = is_int[i] ? hist[j].min : (hist[j].min + hist[j].max)/2;
		fprintf(fp, "%le %d", x, hist[j].value + hist[j+1].value);
		
		free(hist);
		fclose(fp);
	}
}

void *experiment(void *args){
	const char *folder = (char *)args;
	char str[256];
	
	snprintf(str, 256, "%s/summary.txt", folder);
	FILE *f_summary = fopen(str, "wt");
	
	bool is_directed = false;
	snprintf(str, 256, "%s/edges.txt", folder);
	graph_t *complete = load_graph(str, is_directed);
	
	general_info(f_summary, complete);
	component_info(f_summary, complete);
	
	fprintf(stderr, "Extracting giant component in %s...\n", folder);
	graph_t *g = graph_giant_component(complete);
	int n = graph_num_vertices(g);
	delete_graph(complete);
	
	// Allocate metrics matrix
	double **metrics = malloc(NUM_METRIC * sizeof(*metrics));
	metrics[0] = malloc(NUM_METRIC * n * sizeof(*metrics[0]));
	int i;
	for (i=1; i < NUM_METRIC; i++){
		metrics[i] = metrics[0] + i*n;
	}
	
	fprintf(stderr, "Calculating degree in %s...\n", folder);
	degree_info(f_summary, g, metrics);
	
	fprintf(stderr, "Calculating clustering in %s...\n", folder);
	clustering_info(f_summary, g, folder, metrics);
	
	fprintf(stderr, "Calculating degree correlation in %s...\n", folder);
	correlation_info(f_summary, g, folder, metrics);
	
	fprintf(stderr, "Calculating betweenness in %s...\n", folder);
	betweenness_info(f_summary, g, metrics);
	
	fprintf(stderr, "Calculating distance in %s...\n", folder);
	distance_info(f_summary, g, folder);
	
	fprintf(stderr, "Calculating centralities in %s\n", folder);
	centrality_info(f_summary, g, folder, metrics);
	
	print_metrics(folder, metrics, n);
	print_histograms(folder, metrics, n);
	
	free(metrics);
	free(metrics[0]);
	delete_graph(g);
	fclose(f_summary);
	fprintf(stderr, "Processing in %s completed\n", folder);
	return NULL;
}
