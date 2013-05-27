#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "stat.h"
#include "graph.h"
#include "graph_propagation.h"

void delete_propagation_steps(propagation_step_t *step, int num_step){
	int i;
	for (i=0; i < num_step; i++){
		free(step[i].state);
		free(step[i].message);
	}
	free(step);
}

int graph_count_state(int state, short *v, int n){
	int i;
	int num_state = 0;
	for (i=0; i < n; i++){
		if (v[i] == state){
			num_state++;
		}
	}
	return num_state;
}

int graph_find_edge(const graph_t *g, int orig, int dest){
	if (!graph_is_directed(g) && orig > dest){ 
		int aux = orig; orig = dest; dest = aux;
	}
	
	int i, j, n = graph_num_vertices(g);
	int *adj = malloc(n * sizeof(*adj));
	
	int e=0;
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			if (graph_is_directed(g) || i < v){
				if (i == orig && v == dest) break;
				e++;
			}
		}
		if (i == orig && adj[j] == dest) break;
	}
	
	free(adj);
	return e;
}

propagation_step_t *graph_propagation_si
		(const graph_t *g, const short *init_state, int *_num_step){
	return graph_propagation_si_r(g, init_state, _num_step, NULL);
}

propagation_step_t *graph_propagation_si_r
		(const graph_t *g, const short *init_state, int *_num_step, 
		 unsigned int *seedp){
	assert(g);
	assert(init_state);
	assert(_num_step);
	
	int i, n = graph_num_vertices(g);
	
	int num_step = 0;
	int size = 8;
	propagation_step_t *step = malloc(size * sizeof(*step));
	step[num_step].state = malloc(n * sizeof(short));
	
	int *adj = malloc(n * sizeof(*adj));
	
	for (i=0; i < n; i++){
		step[num_step].state[i] = init_state[i];
		step[num_step].n = n;
	}
	
	int count_infected;
	do{
		count_infected = graph_count_state(GRAPH_SI_I, step[num_step].state, n);
		step[num_step].num_message = count_infected;
		step[num_step].message = malloc(count_infected * sizeof(edge_t));
		
		// Create messages from infected to random adjacents
		int m = 0;
		for (i=0; i < n; i++){
			if (step[num_step].state[i] == GRAPH_SI_I){
				step[num_step].message[m].orig = i;
				
				int ki = graph_adjacents(g, i, adj);
				int v = adj[ uniform(ki, seedp) ];
				step[num_step].message[m].dest = v;
				
				m++;
			}
		}
		assert(m == step[num_step].num_message);
		
		// Next step creation
		num_step++;
		if (num_step == size){
			step = realloc(step, 2*size * sizeof(*step));
			size *= 2;
		}
		step[num_step].state = malloc(n * sizeof(short));
		step[num_step].n = n;
		for (i=0; i < n; i++){
			step[num_step].state[i] = GRAPH_SI_S;
		}
		
		// The destination of a message is always infected
		for (i=0; i < step[num_step-1].num_message; i++){
			int orig = step[num_step-1].message[i].orig;
			int dest = step[num_step-1].message[i].dest;
			step[num_step].state[orig] = GRAPH_SI_I;
			step[num_step].state[dest] = GRAPH_SI_I;
		}
	} while (count_infected < n && 
	          num_step < GRAPH_PROPAGATION_K * log2(n));
	
	step = realloc(step, num_step * sizeof(*step));
	*_num_step = num_step;
	return step;
}

void graph_animate_propagation
		(const char *folder, const graph_t *g, const coord_t *p,
		 int num_state,
		 const propagation_step_t *step, int num_step){
	assert(folder);
	assert(g);
	assert(p);
	assert(num_state > 0);
	assert(step);
	assert(num_step > 0);
	
	circle_style_t *point_style = malloc(num_state * sizeof(*point_style));
	path_style_t *edge_style = malloc((num_state+1) * sizeof(*edge_style));
	
	color_t black_50 = {0, 0, 0, 128};
	color_t black_100 = {0, 0, 0, 255};
	int radius = 5;
	int width = 1;
	
	// Edge style 0: transparent black
	edge_style[0].type = GRAPH_STRAIGHT;
	edge_style[0].width = width;	
	color_copy(edge_style[0].color, black_50);
	
	int i;
	for (i=0; i < num_state; i++){
		color_t hsv = {(255 * i)/num_state, 255, 255, 255};
		color_t rgb; color_hsv_to_rgb(rgb, hsv);
		
		point_style[i].width = width;
		point_style[i].radius = radius;
		color_copy(point_style[i].fill, rgb);
		color_copy(point_style[i].stroke, black_100);
		
		edge_style[i+1].type = GRAPH_STRAIGHT;
		edge_style[i+1].width = 2*width;
		color_copy(edge_style[i+1].color, rgb);
	}
	
	int n = graph_num_vertices(g), m = graph_num_edges(g);
	
	int *ps = malloc(n * sizeof(*ps));
	int *es = malloc(m * sizeof(*es));
	
	int *adj = malloc(n * sizeof(*adj));
	
	int s;
	for (s=0; s < num_step; s++){
		char filename[256];
		sprintf(filename, "%s/frame%05d.svg", folder, s);
		
		for (i=0; i < step[s].n; i++){
			ps[i] = step[s].state[i];
		}
		
		memset(es, 0, m * sizeof(*es));
		for (i=0; i < step[s].num_message; i++){
			int orig = step[s].message[i].orig;
			int dest = step[s].message[i].dest;
			
			int e = graph_find_edge(g, orig, dest);
			es[e] = 1 + step[s].state[dest];
		}
		
		graph_print_svg_some_styles(filename, 0, 0, g, p, 
		                            ps, point_style, num_state,
		                            es, edge_style, num_state+1);
	}
	
	free(adj);
	free(ps);
	free(es);
	free(point_style);
	free(edge_style);
}
