#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "stat.h"
#include "graph.h"
#include "graph_game.h"
#include "graph_layout.h"

graph_game_step_t *new_graph_game_steps(int num_steps, int n){
	assert(num_steps > 0);
	assert(n > 0);
	
	graph_game_step_t *step = malloc(num_steps * sizeof(*step));
	int i;
	for (i=0; i < num_steps; i++){
		step[i].state = malloc(n * sizeof(*step[i].state));
		step[i].payoff = malloc(n * sizeof(*step[i].payoff));
	}
	
	return step;
}

void delete_graph_game_steps(graph_game_step_t *step, int num_steps){
	int i;
	for (i=0; i < num_steps; i++){
		free(step[i].state);
		free(step[i].payoff);
	}
	free(step);
}

void graph_game_r
		(const graph_t *g, graph_game_state_t *init_state, 
		 float payoff[2][2], float spread, 
		 graph_game_step_t *step, int num_steps, unsigned int *seedp){
	assert(g);
	assert(init_state);
	assert(step);
	assert(num_steps > 0);
	
	int i, j, n = graph_num_vertices(g);
	
	int t=0;
	for (i=0; i < n; i++){
		step[t].state[i] = init_state[i];
		step[t].payoff[i] = 0.0f;
	}
	
	int *adj = malloc (n * sizeof(*adj));
	int *random_neighbor = malloc(n * sizeof(*adj));
	
	for (t = 1; t < num_steps; t++){
		// At first, everyone keeps its strategy
		for (i=0; i < n; i++){
			step[t].state[i] = step[t-1].state[i];
		}
		
		// Compute payoffs for each individual
		for (i=0; i < n; i++){
			step[t].payoff[i] = 0.0f;
			int ki = graph_adjacents(g, i, adj);
			for (j=0; j < ki; j++){
				int v = adj[j];
				step[t].payoff[i] += payoff[ step[t-1].state[i] ][ step[t-1].state[v] ];
			}
			
			// Choose random neighbor
			if (ki > 0){ random_neighbor[i] = adj[ uniform(ki, seedp) ]; }
		}
		
		// Choose neighbor's strategy if it gives higher payoff
		for (i=0; i < n; i++){
			int ki = graph_num_adjacents(g, i);
			if (ki > 0){
				int v = random_neighbor[i];
				
				double pi = step[t].payoff[i];
				double pv = step[t].payoff[v];
				if (pv > pi){
					int kv = graph_num_adjacents(g, v);
					int kmax = ki > kv ? ki : kv;
					double prob = (pv - pi)/(kmax * spread);
					double r = (double) rand() / RAND_MAX;
					if (r < prob){
						step[t].state[i] = step[t-1].state[v];
					}
				}
			}
		}
	}
	
	free(adj);
	free(random_neighbor);
}

void graph_game_prisioner_r
		(const graph_t *g, double coop_fraction, float b,
		 graph_game_step_t *step, int num_steps, unsigned int *seedp){
	assert(g);
	assert(coop_fraction >= 0.0 && coop_fraction <= 1.0);
	assert(b > 1.0f && b <= 2.0f);
	assert(step);
	assert(num_steps > 0);
	
	int i, n = graph_num_vertices(g);
	graph_game_state_t *init_state = malloc(n * sizeof(*init_state));
	
	for (i=0; i < n; i++){
		double r = (double) rand()/RAND_MAX;
		if (r < coop_fraction){ init_state[i] = GRAPH_GAME_COOP; }
		else                  { init_state[i] = GRAPH_GAME_DEFECT; }
	}
	
	float payoff[2][2] = {{1.0f, 0.0f}, {b, 0.0f}};
	graph_game_r(g, init_state, payoff, b, step, num_steps, seedp);
}

void graph_animate_game
		(const char *folder, const graph_t *g, const coord_t *p, 
		 graph_game_step_t *step, int num_steps){
	assert(folder);
	assert(g);
	assert(p);
	assert(step);
	assert(num_steps > 0);
	
	int i, j, n = graph_num_vertices(g), m = graph_num_edges(g);
	
	color_t red       = {255, 0,   0,   255};
	color_t blue      = {0,   0,   255, 255};
	color_t black_25  = {0,   0,   0,   64};
	color_t black_100 = {0,   0,   0,   255};
	
	path_style_t *edge_style = malloc(m * sizeof(*edge_style));
	circle_style_t *point_style = malloc(n * sizeof(*point_style));
	
	int *adj = malloc(n * sizeof(*adj));
	
	int t;
	for (t=0; t < num_steps; t++){
	  char filename[256];
		sprintf(filename, "%s/frame%05d.svg", folder, t);
		
		int e = 0;
		for (i=0; i < n; i++){
			point_style[i].radius = (int) (1.0f + step[t].payoff[i]);
			point_style[i].width = 1;			
			color_copy(point_style[i].stroke, black_100);
			color_copy(
				point_style[i].fill, 
				step[t].state[i] == GRAPH_GAME_COOP ? red : blue);
			
			int ki = graph_adjacents(g, i, adj);
			for (j=0; j < ki; j++){
				int v = adj[j];
				if (i < v){
					edge_style[e].from.x = p[i].x; edge_style[e].from.y = p[i].y;
					edge_style[e].to.x   = p[v].x; edge_style[e].to.y   = p[v].y;
					edge_style[e].type = GRAPH_STRAIGHT;
					edge_style[e].width = 1;
					color_copy(edge_style[e].color, black_25);
					e++;
				}
			}
		}
		
		graph_print_svg(filename, 800, 800, g, p, point_style, edge_style);
	}
	
	free(adj);
	free(point_style);
	free(edge_style);
}
