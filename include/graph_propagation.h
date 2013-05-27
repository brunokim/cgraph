#ifndef _GRAPH_PROPAGATION_H
#define _GRAPH_PROPAGATION_H

#include "graph.h"
#include "graph_layout.h"

#define GRAPH_PROPAGATION_K 10

typedef struct {
	int orig, dest;
} edge_t;

typedef struct {
	short *state;   // state vector
	int n;          // number of vertices
	
	edge_t *message; // messages exchanged
	int num_message; // number of messages exchanged
} propagation_step_t;

typedef enum {
	GRAPH_SI_S, GRAPH_SI_I, GRAPH_SI_NUM_STATE
} graph_state_si_t;

typedef enum {
	GRAPH_SIS_S, GRAPH_SIS_I, GRAPH_SIS_NUM_STATE
} graph_state_sis_t;

typedef enum {
	GRAPH_SIR_S, GRAPH_SIR_I, GRAPH_SIR_R, GRAPH_SIR_NUM_STATE
} graph_state_sir_t;

typedef enum {
	GRAPH_SEIR_S, GRAPH_SEIR_E, GRAPH_SEIR_I, GRAPH_SEIR_R, GRAPH_SEIR_NUM_STATE
} graph_state_seir_t;

void delete_propagation_steps(propagation_step_t *step, int num_step);

propagation_step_t *graph_propagation_si
	(const graph_t *g, const short *init_state, int *num_step);
propagation_step_t *graph_propagation_si_r
	(const graph_t *g, const short *init_state, int *num_step, 
	 unsigned int *seedp);
	
void graph_animate_propagation
	(const char *folder, const graph_t *g, const coord_t *p, 
	 int num_state,
	 const propagation_step_t *step, int num_step);

#endif
