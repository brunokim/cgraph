#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "graph.h"
#include "graph_model.h"
#include "graph_propagation.h"

#define NAN 0.0/0.0

/******************************** Streaming ***********************************/

typedef struct {
	const char **arg;
	int pos, size;
} str_stream_t;

str_stream_t *new_string_stream(const char *argv[], int argc){
	str_stream_t *stream = malloc (sizeof(*stream));
	stream->pos = 0;
	stream->size = argc;
	stream->arg = argv;
	
	return stream;
}

const char *stream_next(str_stream_t *stream){
	if (stream->pos == stream->size){
		fprintf(stderr, "Error: stream ended abruptly\n");
		exit(1);
	}
	return stream->arg[stream->pos++];
}

bool stream_is_end(str_stream_t *stream){
	return stream->pos == stream->size;
}

/************************* Parsing primitives *********************************/

int parse_int(const char arg[], const char name[]){
	int i;
	int success = sscanf(arg, "%d", &i);
	if (!success){
		fprintf(stderr, "Undefined parameter %s: %s\n", name, arg);
		exit(1);
	}
	return i;
}

double parse_double(const char arg[], const char name[]){
	double d;
	int success = sscanf(arg, "%lf", &d);
	if (!success){
		fprintf(stderr, "Undefined parameter %s: %s\n", name, arg);
		exit(1);
	}
	return d;
}

unsigned int parse_uint(const char arg[], const char name[]){
	unsigned int i;
	int success = sscanf(arg, "%ud", &i);
	if (!success){
		fprintf(stderr, "Undefined parameter %s: %s\n", name, arg);
		exit(1);
	}
	return i;
}

/******************************* Network Model ********************************/

enum {K, ER, BA, WS, RB, NUM_NETWORK_MODEL};
char network_model_code[NUM_NETWORK_MODEL][3] = {"K", "ER", "BA", "WS", "RB"};
char network_model_name[NUM_NETWORK_MODEL][16] = {
	"Clique", "Erdos-Renyi", "Barabasi-Albert", "Watts-Strogatz", "Ravasz-Barabasi"
};

char network_model_param[NUM_NETWORK_MODEL][40] = {
	"n: int",
	"n: int, k: double",
	"n: int, k: int",
	"n: int, k: int, beta: double",
	"l: int, k: int"
};

double network_params[3] = {NAN, NAN, NAN};

void parse_network_params(str_stream_t *stream, short network_model){
	
	
	if (network_model == ER)
	{
		network_params[0] = parse_uint(stream_next(stream), "n");
		network_params[1] = parse_double(stream_next(stream), "k");
	}
	else if (network_model == BA)
	{
		network_params[0] = parse_uint(stream_next(stream), "n");
		network_params[1] = parse_uint(stream_next(stream), "k");
	}
	else if (network_model == WS)
	{
		network_params[0] = parse_uint(stream_next(stream), "n");
		network_params[1] = parse_uint(stream_next(stream), "k");
		network_params[2] = parse_double(stream_next(stream), "beta");
	}
	else if (network_model == RB)
	{
		network_params[0] = parse_uint(stream_next(stream), "l");
		network_params[1] = parse_uint(stream_next(stream), "k");
	}
}

void check_network_params(short network_model){
	bool is_failure = false;
	double n = network_params[0];
	double k = network_params[1];
	double beta = network_params[2];
	
	if (n <= 0){
		fprintf(stderr, "Network model: %s is not positive\n", network_model != RB ? "n" : "l");
		is_failure = true;
	}
		
	if (network_model == ER || network_model == BA || network_model == WS || network_model == RB){
		if (k < 0){
			fprintf(stderr, "Network model: k is not positive\n");
			is_failure = true;
		}
	}
	
	if (network_model == ER || network_model == BA || network_model == WS){
		if (k >= n){
			fprintf(stderr, "Network model: k is equal or bigger than n\n");
			is_failure = true;
		}
	}
	
	if (network_model == WS){
		if (beta < 0.0 || beta > 1.0){
			fprintf(stderr, "Network model: beta is not a probability\n");
			is_failure = true;
		}
		if (((int)k) % 2 != 0){
			fprintf(stderr, "Network model: k is not even\n");
			is_failure = true;
		}
	}
	
	if (is_failure){
		exit(1);
	}
}

/***************************** Dissemination Model ****************************/

enum {SI, SIS, SIR, SEIR, DK, SIZR, NUM_PROPAGATION_MODEL};
char propagation_code[NUM_PROPAGATION_MODEL][5] = {
	"SI", "SIS", "SIR", "SEIR", "DK", "SIZR"
};

char propagation_model_param[NUM_PROPAGATION_MODEL][80] = {
	"alpha",
	"alpha, beta",
	"alpha, beta",
	"alpha, beta, gamma",
	"alpha, beta",
	"alpha, beta, delta, rho, csi, c"
};

double propagation_params[6] = {NAN, NAN, NAN, NAN, NAN, NAN};

void parse_propagation_params(str_stream_t *stream, short propagation_model){
	propagation_params[0] = parse_double(stream_next(stream), "alpha");
	
	if (propagation_model == SIS)
	{
		propagation_params[1] = parse_double(stream_next(stream), "beta");
	}
	else if (propagation_model == SIR)
	{
		propagation_params[1] = parse_double(stream_next(stream), "beta");
	}
	else if (propagation_model == SEIR)
	{
		propagation_params[1] = parse_double(stream_next(stream), "beta");
		propagation_params[2] = parse_double(stream_next(stream), "gamma");
	}
	else if (propagation_model == DK)
	{
		propagation_params[1] = parse_double(stream_next(stream), "beta");
	}
	else if (propagation_model == SIZR)
	{
		propagation_params[1] = parse_double(stream_next(stream), "beta");
		propagation_params[2] = parse_double(stream_next(stream), "delta");
		propagation_params[3] = parse_double(stream_next(stream), "rho");
		propagation_params[4] = parse_double(stream_next(stream), "csi");
		propagation_params[5] = parse_double(stream_next(stream), "c");
	}
}

void check_propagation_params(short propagation_model){
	bool is_failure = false;
	double alpha = propagation_params[0];
	double beta = propagation_params[1];
	
	if (alpha < 0.0 || alpha > 1.0){
		fprintf(stderr, "Propagation model: alpha is not a probability\n");
		is_failure = true;
	}
	
	if (propagation_model == SIS || propagation_model == SIR ||
	    propagation_model == SEIR || propagation_model == DK){
		if (beta < 0.0 || beta > 1.0){
			fprintf(stderr, "Propagation model: beta is not a probability\n");
			is_failure = true;
		}
	}
	
	if (propagation_model == SEIR){
		double gamma = propagation_params[2];
		if (gamma < 0.0 || gamma > 1.0){
			fprintf(stderr, "Propagation model: beta is not a probability\n");
			is_failure = true;
		}
	}
	
	if (is_failure){
		exit(1);
	}
}

/************************** General utilities *********************************/

void print_usage(){
		printf(
			"Usage: propagation\n"
		  "       ((-d|--dataset) {folder}|\n"
		  "        (-n|--network) <network-model> <params> [(-N|--network-seed) <value>])\n"
		  "       (-p|--propagation) <propagation-model> [(-P|--propagation-seed) <value>]\n"
		  "       [(-o|--outfile) <file>]\n"
		  "       [(-r|--repetition) <r>]\n"
		  "\n");
		
		printf("<network-model> = (");
		int i;
		for (i=0; i < NUM_NETWORK_MODEL; i++){
		  printf("%s", network_model_code[i]);
		  if (i != NUM_NETWORK_MODEL-1) printf("|");
		}
		printf(")\n");
		
		for (i=0; i < NUM_NETWORK_MODEL; i++){
		  printf("    %s: %s (params: %s)\n", 
				network_model_code[i], network_model_name[i], network_model_param[i]);
		}
		printf("\n");
		
		printf("<propagation-model> = (");
		for (i=0; i < NUM_PROPAGATION_MODEL; i++){
		  printf("%s", propagation_code[i]);
		  if (i != NUM_PROPAGATION_MODEL-1) printf("|");
		}
		printf(")\n");
		
		for (i=0; i < NUM_PROPAGATION_MODEL; i++){
		  printf("    %s parameters: %s\n", 
				propagation_code[i], propagation_model_param[i]);
		}
		printf("\n");
		
		//printf("\n");	
}

bool is_arg(const char *arg, char short_code, const char *long_code){
	if (arg[0] != '-'){
		fprintf(stderr, "Not an argument: %s\n", arg);
		exit(1);
	}
	
	if (arg[1] != '-' && arg[1] == short_code)
	{
		return true;
	}
	else if (!strcmp(arg+2, long_code))
	{
		return true;
	}
	return false;
}

const char *folder = NULL;
short network_model = -1;
unsigned int network_seed = 1;
short propagation_model = -1;
unsigned int propagation_seed = 1;
const char *filename = NULL;
int r = 1;

void parse_args(str_stream_t *stream){
	stream->pos = 1;
	while (!stream_is_end(stream)){
		const char *arg = stream_next(stream);
		
		if (is_arg(arg, 'd', "dataset"))
		{
			folder = stream_next(stream);
		} 
		else if (is_arg(arg, 'n', "network"))
		{
			const char *model_code = stream_next(stream);
			int j;
			for (j=0; j < NUM_NETWORK_MODEL; j++){
				if (!strcmp(network_model_code[j], model_code)) break;
			}
			if (j == NUM_NETWORK_MODEL){ 
				fprintf(stderr, "Invalid network model: %s\n", model_code);
				exit(1);
			}
			network_model = j;
			
			parse_network_params(stream, network_model);
		}
		else if (is_arg(arg, 'N', "network-seed"))
		{
			network_seed = parse_uint(stream_next(stream), "network-seed");
		}
		else if (is_arg(arg, 'p', "propagation"))
		{
			const char *prop_code = stream_next(stream);
			int j;
			for (j=0; j < NUM_PROPAGATION_MODEL; j++){
				if (!strcmp(propagation_code[j], prop_code)) break;
			}
			if (j == NUM_PROPAGATION_MODEL){ 
				fprintf(stderr, "Invalid propagation model: %s\n", prop_code);
				exit(1);
			}
			propagation_model = j;
			
			parse_propagation_params(stream, propagation_model);
		}
		else if (is_arg(arg, 'P', "propagation-seed"))
		{
			propagation_seed = parse_uint(stream_next(stream), "propagation-seed");
		}
		else if (is_arg(arg, 'o', "outfile"))
		{
			filename = stream_next(stream);
		}
		else if (is_arg(arg, 'r', "repetition"))
		{
			r = parse_uint(stream_next(stream), "repetition");
		}
	}
}

void print_args(){
	printf("dataset: %s\n", folder ? folder : "");
	printf("network-model : %s\n", 
	       network_model >= 0 ? network_model_name[network_model] : "");
	printf("network-seed : %d\n", network_seed);
	printf("propagation-model : %s\n", 
	       propagation_model >= 0 ? propagation_code[propagation_model] : "");
	printf("propagation-seed : %d\n", propagation_seed);
	printf("outfile: %s\n", filename ? filename : "");
	printf("repetition : %d\n", r);
}

void check(){
	bool is_failure = false;
	if (!folder && network_model < 0){
		fprintf(stderr, "Dataset or network model must be specified\n");
		is_failure = true;
	}
	
	if (folder && network_model >= 0){
		fprintf(stderr, "Both dataset and network model specified\n");
		is_failure = true;
	}
	if (network_model >= 0){ 
		check_network_params(network_model); 
	}
	
	if (propagation_model < 0){
		fprintf(stderr, "Propagation model not specified\n");
		is_failure = true;
	}
	
	check_propagation_params(propagation_model);
	
	if (r <= 0){
		fprintf(stderr, "Number of repetitions not positive\n");
		is_failure = true;
	}
	
	if (is_failure){
		exit(1);
	}
}

int main(int argc, const char *argv[]){
	if (argc == 1){
		print_usage();
		return 0;
	}
	
	str_stream_t *stream = new_string_stream(argv, argc);
	parse_args(stream);
	free(stream);
	
	//print_args();
	check();
	
	graph_t *g = NULL;
	if (network_model < 0) // Load dataset
	{
		char str[256];
		sprintf(str, "%s/edges.txt", folder);
		g = load_graph(str, false);
	}
	else
	{
		int    n    = (int) network_params[0];
		int    l    = (int) network_params[0];
		double k    = network_params[1];
		double beta = network_params[2];

		//print_args();
		
		switch(network_model){
			case K:
				g = new_clique(n);
				break;
			case ER:
				g = new_erdos_renyi_r(n, k, &network_seed);
				break;
			case BA:
				g = new_barabasi_albert_r(n, (int)k, &network_seed);
				break;
			case WS:
				g = new_watts_strogatz_r(n, (int)k, beta, &network_seed);
				break;
			case RB:
				g = new_ravasz_barabasi(l, (int)k);
				break;
		}
	}
	
	propagation_model_t model;
	void *params;
	
	if (propagation_model == SI)
	{
		model = si;
		params = malloc(sizeof(graph_si_params_t));
		((graph_si_params_t *)params)->alpha = propagation_params[0];
	}
	else if (propagation_model == SIS)
	{
		model = sis;
		params = malloc(sizeof(graph_sis_params_t));
		((graph_sis_params_t *)params)->alpha = propagation_params[0];
		((graph_sis_params_t *)params)->beta = propagation_params[1];
	}
	else if (propagation_model == SIR)
	{
		model = sir;
		params = malloc(sizeof(graph_sir_params_t));
		((graph_sir_params_t *)params)->alpha = propagation_params[0];
		((graph_sir_params_t *)params)->beta = propagation_params[1];
	}
	else if (propagation_model == SEIR)
	{
		model = seir;
		params = malloc(sizeof(graph_seir_params_t));
		((graph_seir_params_t *)params)->alpha = propagation_params[0];
		((graph_seir_params_t *)params)->beta = propagation_params[1];
		((graph_seir_params_t *)params)->gamma = propagation_params[2];
	}
	else if (propagation_model == DK)
	{
		model = dk;
		params = malloc(sizeof(graph_dk_params_t));
		((graph_dk_params_t *)params)->alpha = propagation_params[0];
		((graph_dk_params_t *)params)->beta = propagation_params[1];
	}
	else if (propagation_model == SIZR)
	{
		model = sizr;
		params = malloc(sizeof(graph_sizr_params_t));
		((graph_sizr_params_t *)params)->alpha = propagation_params[0];
		((graph_sizr_params_t *)params)->beta = propagation_params[1];
		((graph_sizr_params_t *)params)->delta = propagation_params[2];
		((graph_sizr_params_t *)params)->rho = propagation_params[3];
		((graph_sizr_params_t *)params)->csi = propagation_params[4];
		((graph_sizr_params_t *)params)->c = propagation_params[5];
	}
	
	FILE *outfile = filename ? fopen(filename, "wt") : stdout;
	fprintf(outfile, "#num_step num_message num_state...\n");
	
	int n = graph_num_vertices(g);
	short *state = malloc(n * sizeof(*state));
	
	int i, j, s;
	clock_t tstart, tstop;
	tstart = clock();
	for (i=0; i < r; i++){
		memset(state, 0, n * sizeof(*state));
		state[0] = model.infectious_state;
		//state[1] = model.infectious_state;
		//state[2] = model.infectious_state;
		//state[3] = model.infectious_state;
		//state[4] = model.infectious_state;
		//state[5] = model.infectious_state;
		//state[6] = model.infectious_state;
		//state[7] = model.infectious_state;
		//state[8] = model.infectious_state;
		//state[9] = model.infectious_state;
		
		int num_step;
		propagation_step_t *step = 
			graph_propagation_r(g, state, &num_step, model,
		                      params, &propagation_seed);
		
		int num_message = 0;
		for (s=0; s < num_step; s++){
			num_message += step[s].num_message;
		}
		
		fprintf(outfile, "%d %d ", num_step, num_message);
		for (j=0; j < model.num_state; j++){
			short *final_state = step[num_step-1].state;
			int final_n = step[num_step-1].n;
			fprintf(outfile, "%d ", graph_count_state(j, final_state, final_n));
		}
		fprintf(outfile, "\n");
		
		if (r == 1){
			int **freq = malloc(num_step * sizeof(*freq));
			freq[0] = malloc(num_step * model.num_state * sizeof(*freq[0]));
			for (j=1; j < num_step; j++){
				freq[j] = freq[0] + j*model.num_state;
			}
			
			graph_propagation_freq(step, num_step, freq, model.num_state);
			for (j=0; j < num_step; j++){
				int k;
				for (k=0; k < model.num_state; k++){
					printf("%d ", freq[j][k]);
				}
				printf("\n");
			}
			
			free(freq[0]); free(freq);
		}
		
		delete_propagation_steps(step, num_step);
	}
	tstop = clock();
	//printf("\nTempo de execucao: %ld\n\n", (tstop-tstart)/(CLOCKS_PER_SEC/1000));
 
	free(params);
	if (outfile != stdout){
		fclose(outfile);
	}
	
	return 0;
}
