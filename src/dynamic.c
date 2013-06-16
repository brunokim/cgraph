/* dynamic.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

// Preguica de fazer alocacao dinamica...
#define MAX_STATE 10

typedef enum {N, ALFA, BETA, GAMMA} param_order_t;

typedef void (*deriv)(double *y, double *x, double params[]);
typedef bool (*stop_pred)(double t, double *x, double params[]);

typedef struct model_t model_t;
struct model_t {
	int n_state;    // Numero de compartimentos
	int infectious_state; // Estado infeccioso
	deriv f;        // Equacao diferencial
	stop_pred stop; // Predicado de parada
};

/******************** SI Model ********************/

void si_deriv(double *y, double *x, double params[]){
    double alfa = params[ALFA];
    double s = x[0], i = x[1];
    y[0] = - alfa * s * i;
    y[1] =   alfa * s * i;
}

bool si_stop(double t, double *x, double params[]){
	return x[0] < 1.0/params[N];
}

model_t si = {2, 1, si_deriv, si_stop};

/******************** SIS Model ********************/

void sis_deriv(double *y, double *x, double params[]){
    double alfa = params[ALFA];
    double beta = params[BETA];
    double s = x[0], i = x[1];
    y[0] = - alfa * s * i + beta * i;
    y[1] =   alfa * s * i - beta * i;
}

bool sis_stop(double t, double *x, double params[]){
	double alfa = params[ALFA];
  double beta = params[BETA];  
	return fabs(x[0] - beta/alfa) < 1.0/params[N];
}

model_t sis = {2, 1, sis_deriv, sis_stop};

/******************** SIR Model ********************/

void sir_deriv(double *y, double *x, double params[]){
    double alfa = params[ALFA];
    double beta = params[BETA];
    double s = x[0], i = x[1];
    y[0] = - alfa * s * i;
    y[1] =   alfa * s * i - beta * i;
    y[2] =                  beta * i;
}

bool sir_stop(double t, double *x, double params[]){
	return x[1] < 0.5/params[N];
}

model_t sir = {3, 1, sir_deriv, sir_stop};

/******************** SEIR Model ********************/

void seir_deriv(double *y, double *x, double params[]){
		double alfa = params[ALFA];
    double beta = params[BETA];
    double gamma = params[GAMMA];
    double s = x[0], e = x[1], i = x[2];
    y[0] = - alfa * s * i;
    y[1] =   alfa * s * i - gamma * e;
    y[2] =                  gamma * e - beta * i;
    y[3] =                              beta * i;
}

bool seir_stop(double t, double *x, double params[]){
	return x[1]+x[2] < 1.0/params[N];
}

model_t seir = {4, 2, seir_deriv, seir_stop};

/**************** Daley-Kendall Model **************/

void dk_deriv(double *y, double *x, double params[]){
    double alfa = params[ALFA];
    double beta = params[BETA];
    double d = x[0], i = x[1], c = x[2];
    y[0] = - alfa * d * i;
    y[1] =   alfa * d * i - beta * i * (i + c);
    y[2] =                  beta * i * (i + c);
}

bool dk_stop(double t, double *x, double params[]){
	return x[1] < 1.0/params[N];
}

model_t dk = {3, 1, dk_deriv, dk_stop};

/**************** Vector utility **************/

void fprint_vec(FILE *fp, double t, double *x, int n){
	if (fp){
		fprintf(fp, "%lf ", t);
		int i;
		for (i=0; i < n; i++){
			fprintf(fp, "%lf ", x[i]);
		}
		fprintf(fp, "\n");
	}
}

void copy_vec(double *to, double *from, int n){
		int i;
    for (i=0; i < n; i++){
			to[i] = from[i];
		}
}

/**************** Numerical integration **************/

void rk2_step(deriv f, double *x_next, double *x, int n, double h, double params[]){
    int i;
    double k1[MAX_STATE], y1[MAX_STATE], y2[MAX_STATE];
    
    f(&y1[0], x, params);
    
    for (i=0; i < n; i++){
			k1[i] = x[i] + h*y1[i];
		}
    
    f(&y2[0], &k1[0], params);
    
    for (i=0; i < n; i++){
			x_next[i] = x[i] + h/2 * (y1[i] + y2[i]);
		}
}

void rk2(model_t model, double *x0, double params[], FILE *fp, double *_t){
    int i, n = model.n_state;
    double x_next[MAX_STATE], x[MAX_STATE];
		copy_vec(&x[0], x0, n);
		
    double h = 1e-3;
    double t = 0.0;
    fprint_vec(fp, t, x, n);
    for (i=0; !model.stop(t, x, params); i++){
        rk2_step(model.f, &x_next[0], &x[0], n, h, params);
        copy_vec(&x[0], &x_next[0], n);
        
        t = (i+1)*h;
        fprint_vec(fp, t, x, n);
    }
    
    if (_t) *_t = t;
}

/******** Input functions ********/

void print_usage(){
	printf("Usage: dynamic <model> <params> [<file>]\n"
         "if file is not given, output to stdout\n"
         "Available models and mandatory parameters:\n"
         "   SI:   <n> <alfa>\n"
         "   SIS:  <n> <alfa> <beta>\n"
         "   SIR:  <n> <alfa> <beta>\n"
         "   SEIR: <n> <alfa> <beta> <gamma>\n"
         "   DK:   <n> <alfa> <beta>\n");
}

FILE* parse_args(int argc, char *argv[], model_t *model, double params[]){
	if (argc < 4) return NULL;
	
	char *model_str = argv[1];
	char *n_str     = argv[2];
	char *alfa_str  = argv[3];
	FILE *outfile = NULL;
	
	if (!strcmp(model_str, "SI"))
	{
		*model = si;
		if       (argc == 4) outfile = stdout;
		else if (argc == 5) outfile = fopen(argv[argc-1], "wt");
		else return NULL;
		
		int n; 
		sscanf(n_str, "%d", &n); 
		params[N] = (double)n;
		
		sscanf(alfa_str, "%lf", &params[ALFA]);
	} 
	else if (!strcmp(model_str, "SIS"))
	{
		*model = sis;
		if       (argc == 5) outfile = stdout;
		else if (argc == 6) outfile = fopen(argv[argc-1], "wt");
		else return NULL;
		
		char *beta_str = argv[4];
		
		int n; 
		sscanf(n_str, "%d", &n); 
		params[N] = (double)n;
		
		sscanf(alfa_str, "%lf", &params[ALFA]);
		sscanf(beta_str, "%lf", &params[BETA]);
	}
	else if (!strcmp(model_str, "SIR"))
	{
		*model = sir;
		if       (argc == 5) outfile = stdout;
		else if (argc == 6) outfile = fopen(argv[argc-1], "wt");
		else return NULL;
		
		char *beta_str = argv[4];
		
		int n; 
		sscanf(n_str, "%d", &n); 
		params[N] = (double)n;
		
		sscanf(alfa_str, "%lf", &params[ALFA]);
		sscanf(beta_str, "%lf", &params[BETA]);
	}
	else if (!strcmp(model_str, "SEIR"))
	{
		*model = seir;
		if       (argc == 6) outfile = stdout;
		else if (argc == 7) outfile = fopen(argv[argc-1], "wt");
		else return NULL;
		
		char *beta_str = argv[4];
		char *gamma_str = argv[5];
		
		int n; 
		sscanf(n_str, "%d", &n); 
		params[N] = (double)n;
		
		sscanf(alfa_str, "%lf", &params[ALFA]);
		sscanf(beta_str, "%lf", &params[BETA]);
		sscanf(gamma_str, "%lf", &params[GAMMA]);
	}
	else if (!strcmp(model_str, "DK"))
	{
		*model = dk;
		if       (argc == 5) outfile = stdout;
		else if (argc == 6) outfile = fopen(argv[argc-1], "wt");
		else return NULL;
		
		char *beta_str = argv[4];
		
		int n; 
		sscanf(n_str, "%d", &n); 
		params[N] = (double)n;
		
		sscanf(alfa_str, "%lf", &params[ALFA]);
		sscanf(beta_str, "%lf", &params[BETA]);
	}
	else
	{
		printf("Unknown model type: %s. Available types are: SI SIS SIR SEIR DK\n", model_str);
		return NULL;
	}	
	
	return outfile;
}

int main(int argc, char *argv[]){
		model_t model;
		double params[MAX_STATE];
		FILE *outfile = parse_args(argc, argv, &model, &params[0]);
		
    if (!outfile){
				print_usage();
        exit(EXIT_SUCCESS);
    }
    
    int i, n = params[N];
    double x[MAX_STATE];
    for (i=0; i < model.n_state; i++){
			x[i] = 0.0;
		}
    
    x[0] = 1 - 1.0/n;
    x[model.infectious_state] = 1.0/n;
    
    rk2(model, &x[0], params, outfile, NULL);
    if (outfile != stdout){
        fclose(outfile);
    }
    
    return 0;
}
