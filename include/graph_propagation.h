#ifndef _GRAPH_PROPAGATION_H
#define _GRAPH_PROPAGATION_H

#include <stdbool.h>

#include "graph.h"
#include "graph_layout.h"

/********************************* Constants **********************************/

#ifndef GRAPH_PROPAGATION_K
 #define GRAPH_PROPAGATION_K 10
#endif

/*********************************** Types ************************************/

typedef struct {
	int orig, dest;
} message_t;

typedef struct {
	short *state;   // state vector
	int n;          // number of vertices
	
	message_t *message; // messages exchanged
	int num_message; // number of messages exchanged
} propagation_step_t;

// Callback for state transition
typedef void (*state_transition_f)
	(short *next, const propagation_step_t curr, int n, 
	 const void *params, unsigned int *seedp);

// Callback to test for simulation end
typedef bool (*is_propagation_end)
	(const short *state, int n, int num_step, const void *params);
	
typedef struct {
	const char *name;                // Model name
	short infectious_state;          // Infectious state value
	state_transition_f transition; // Transition callback
	is_propagation_end is_end;     // Ending predicate callback
	int num_state;                   // Total number of states
} propagation_model_t;

/********************************* Functions **********************************/
// Counts number of individuals in s that are in the given state
int graph_count_state(int state, const short *s, int n);

// Simulates a propagation in graph with the given state vector and model
propagation_step_t *graph_propagation
	(const graph_t *g, const short *init_state, int *num_step,
	 propagation_model_t model, const void *params);

// Simulates a propagation using a seed state variable
propagation_step_t *graph_propagation_r
	(const graph_t *g, const short *init_state, int *num_step,
	 propagation_model_t model, const void *params, unsigned int *seedp);

// Deallocate a step array that was allocated with graph_propagation.
void delete_propagation_steps(propagation_step_t *step, int num_step);

// Creates animation frames of a propagation in the given graph.
void graph_animate_propagation
	(const char *folder, const graph_t *g, const coord_t *p, 
	 int num_state,
	 const propagation_step_t *step, int num_step);

// Compute the number of individuals in each state at each propagation step.
void graph_propagation_freq
	(const propagation_step_t *step, int num_step, int **freq, int num_state);

/********************************* SI model ***********************************/
typedef struct { 
	double alpha; // Infection probability
} graph_si_params_t;

typedef enum {
	GRAPH_SI_S, GRAPH_SI_I, GRAPH_SI_NUM_STATE
} graph_state_si_t;

void graph_si_transition
	(short *next, const propagation_step_t curr, int n, 
	 const void *params, unsigned int *seedp);

bool is_si_end
	(const short *state, int n, int num_step, const void *params);

extern const propagation_model_t si;

/********************************* SIS model **********************************/
typedef struct { 
	double alpha; // Infection probability
	double beta;  // Cure probability
	int num_iter; // Maximum number of iterations
} graph_sis_params_t;

typedef enum {
	GRAPH_SIS_S, GRAPH_SIS_I, GRAPH_SIS_NUM_STATE
} graph_state_sis_t;

void graph_sis_transition
	(short *next, const propagation_step_t curr, int n, 
	 const void *params, unsigned int *seedp);

bool is_sis_end
	(const short *state, int n, int num_step, const void *params);

extern const propagation_model_t sis;

/********************************* SIR model **********************************/
typedef struct { 
	double alpha; // Infection probability
	double beta;  // Cure probability
} graph_sir_params_t;

typedef enum {
	GRAPH_SIR_S, GRAPH_SIR_I, GRAPH_SIR_R, GRAPH_SIR_NUM_STATE
} graph_state_sir_t;

void graph_sir_transition
	(short *next, const propagation_step_t curr, int n, 
	 const void *params, unsigned int *seedp);

bool is_sir_end
	(const short *state, int n, int num_step, const void *params);

extern const propagation_model_t sir;

/********************************* SEIR model *********************************/
typedef struct { 
	double alpha; // Exposure probability
	double beta;  // Cure probability
	double gamma; // Infection probability
} graph_seir_params_t;

typedef enum {
	GRAPH_SEIR_S, GRAPH_SEIR_E, GRAPH_SEIR_I, GRAPH_SEIR_R, GRAPH_SEIR_NUM_STATE
} graph_state_seir_t;

void graph_seir_transition
	(short *next, const propagation_step_t curr, int n, 
	 const void *params, unsigned int *seedp);

bool is_seir_end
	(const short *state, int n, int num_step, const void *params);

extern const propagation_model_t seir;

/**************************** Daley-Kendall model *****************************/
typedef struct { 
	double alpha; // Spreading probability
	double beta;  // Stifling probability
} graph_dk_params_t;

typedef enum {
	GRAPH_DK_X, GRAPH_DK_Y, GRAPH_DK_Z, GRAPH_DK_NUM_STATE
} graph_state_dk_t;

void graph_dk_transition
	(short *next, const propagation_step_t curr, int n, 
	 const void *params, unsigned int *seedp);

bool is_dk_end
	(const short *state, int n, int num_step, const void *params);

extern const propagation_model_t dk;

#endif
