#include <stdlib.h>

#include "graph.h"
#include "math.h"
#include "graph_game.h"
#include "graph_model.h"
#include "graph_layout.h"

void test_prisioner(){
	graph_t *g = new_barabasi_albert(128, 6);
	int i, n = graph_num_vertices(g);
	
	int num_steps = 800;
	graph_game_step_t *step = new_graph_game_steps(num_steps, n);
	
	graph_game_prisioner_r(g, 0.3, 1.2f, step, num_steps, NULL);
	
	int t; float min = n*n, max = 0.0;
	for (t=0; t < num_steps; t++){
		for (i=0; i < n; i++){
			if (step[t].payoff[i] > max)
			{
				max = step[t].payoff[i];
			} 
			else if (step[t].payoff[i] < min)
			{
				min = step[t].payoff[i];
			}
		}
	}
	printf("payoff: min: %f max: %f\n", min, max);
	
	FILE *fp = fopen("test/anim_pd/freq.dat", "wt");
	for (t=0; t < num_steps; t++){
		int num_coop[2] = {0, 0};
		float payoff[2] = {0.0f, 0.0f};
		
		for (i=0; i < n; i++){
			num_coop[step[t].state[i]] += 1;
			payoff[step[t].state[i]] += step[t].payoff[i];
		}
		
		fprintf(fp, "%d %d %f %f\n", 
		       num_coop[GRAPH_GAME_COOP], num_coop[GRAPH_GAME_DEFECT], 
		       payoff[GRAPH_GAME_COOP],   payoff[GRAPH_GAME_DEFECT]);
	}
	fclose(fp);
	int width = 1, radius = 5;
	coord_t *p = malloc(n * sizeof(*p));
	graph_layout_degree_shell(g, width+radius, true, p);
	
	graph_animate_game("test/anim_pd", g, p, step, num_steps);
	
	delete_graph_game_steps(step, num_steps);
	free(p);
	delete_graph(g);
}

int main(){
	test_prisioner();
	printf("success\n");
	return 0;
}
