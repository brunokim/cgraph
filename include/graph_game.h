#ifndef _GRAPH_GAME_H
#define _GRAPH_GAME_H

#include "graph.h"
#include "graph_layout.h"

typedef enum {GRAPH_GAME_COOP, GRAPH_GAME_DEFECT} graph_game_state_t;

typedef struct {
	int n;
	graph_game_state_t *state;
	float *payoff;
} graph_game_step_t;

graph_game_step_t *new_graph_game_steps(int num_steps, int n);
void delete_graph_game_steps(graph_game_step_t *step, int num_steps);

void graph_game_r
	(const graph_t *g, graph_game_state_t *init_state, 
	 float payoff[2][2], float spread, 
	 graph_game_step_t *step, int num_steps, unsigned int *seedp);

void graph_game_prisioner_r
	(const graph_t *g, double coop_fraction, float b,
	 graph_game_step_t *step, int num_steps, unsigned int *seedp);

void graph_animate_game
	(const char *folder, const graph_t *g, const coord_t *p,
	 graph_game_step_t *step, int num_steps);

#endif
