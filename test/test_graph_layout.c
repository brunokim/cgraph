#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "graph_layout.h"

/*    1 --- 3 --- 5
 *   /|     |\  _/|
 *  / |     |_\/  |
 * 0  |   _/|  \  |  7
 *  \ | _/  |   \ | /
 *   \|/    |    \|/
 *    2 --- 4 --- 6
 */
graph_t * make_a_graph(bool is_directed){
	int is_weighted = false;
	int n = 8;
	graph_t *g = new_graph(n, is_weighted, is_directed);
	
	graph_add_edge(g, 0, 1);
	graph_add_edge(g, 0, 2);
	graph_add_edge(g, 1, 2);
	graph_add_edge(g, 1, 3);
	graph_add_edge(g, 2, 4);
	graph_add_edge(g, 2, 5);
	graph_add_edge(g, 3, 4);
	graph_add_edge(g, 3, 5);
	graph_add_edge(g, 3, 6);
	graph_add_edge(g, 4, 6);
	graph_add_edge(g, 5, 6);
	graph_add_edge(g, 6, 7);
	
	return g;
}

coord_t *make_a_position(int width, int radius){
	int n = 8;
	int x[] = {0, 1, 1, 2, 2, 3, 3, 4};
	int y[] = {1, 0, 2, 0, 2, 0, 2, 1};
	
	coord_t *p = malloc(n * sizeof(*p));
	
	int i;
	for (i=0; i < n; i++){
		int l = width+radius;
		p[i].x = l + 3*l*x[i];
		p[i].y = l + 3*l*y[i];
	}
	
	return p;
}

void test_one_style(){
	int n = 8;
	graph_t *g = make_a_graph(true);
	
	int width = 1, radius = 5;
	coord_t *p = make_a_position(width, radius);
	
	path_style_t edge_style = {
		{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
		GRAPH_STRAIGHT,
		1, {0, 0, 0, 255}
	};
	circle_style_t point_style = {
		5, 1, 
		{255, 0, 0, 255},
		{0  , 0, 0, 255}
	};
	
	graph_print_svg_one_style("test/test_one_style.svg", g, p, point_style, edge_style);
	
	delete_graph(g);
}

void test_many_styles(){
	int i, j, n = 8, m = 12;
	graph_t *g = make_a_graph(true);
	
	int width = 1, radius = 6;
	coord_t *p = make_a_position(width, radius);
	
	path_style_t *edge_style = malloc(m * sizeof(*edge_style));
	
	int e=0, adj[8]; 
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			edge_style[e].from.x = p[i].x;
			edge_style[e].from.y = p[i].y;
			edge_style[e].to.x   = p[v].x;
			edge_style[e].to.y   = p[v].y;
			edge_style[e].type = GRAPH_STRAIGHT;
			
			edge_style[e].width = ki;
			
			edge_style[e].color[0] = rand() / (RAND_MAX/256);
			edge_style[e].color[1] = rand() / (RAND_MAX/256);
			edge_style[e].color[2] = rand() / (RAND_MAX/256);
			edge_style[e].color[3] = 128 + rand() / (RAND_MAX/128);
			
			e++;
		}
	}
	assert(e == m);
	
	circle_style_t *point_style = malloc(n * sizeof(*edge_style));
	
	for (i=0; i < n; i++){
		point_style[i].radius = 1+graph_num_adjacents(g, i);
		point_style[i].width = 1;
		
		int r = rand() / (RAND_MAX/256);
		point_style[i].fill[0] = r;
		point_style[i].fill[1] = r;
		point_style[i].fill[2] = r;
		point_style[i].fill[3] = 255;
		
		point_style[i].stroke[0] = 0;
		point_style[i].stroke[1] = 0;
		point_style[i].stroke[2] = 0;
		point_style[i].stroke[3] = 255;
	}
	
	graph_print_svg("test/test_many_style.svg", g, p, point_style, edge_style);
	
	delete_graph(g);
	free(p);
	free(edge_style);
	free(point_style);
}

void copy_color(color_t copy, const color_t original){
	copy[0] = original[0];
	copy[1] = original[1];
	copy[2] = original[2];
	copy[3] = original[3];
}

void test_some_styles(){
	graph_t *g = make_a_graph(true);
	
	int width = 1, radius = 6;
	coord_t *p = make_a_position(width, radius);
	
	int num_edge_style = 3;
	int es[] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2};
	path_style_t edge_style[3];
	
	color_t solid_red   = {255, 0, 0, 255};
	color_t solid_green = {0, 255, 0, 255};
	color_t solid_blue  = {0, 0, 255, 255};
	color_t black       = {0, 0, 0,   255};
	
	// Edges within community 0 in blue
	edge_style[0].type = GRAPH_STRAIGHT;
	edge_style[0].width = 1;
	copy_color(edge_style[0].color, solid_blue);
	
	// Edges between communities 0 and 1 in red
	edge_style[1].type = GRAPH_STRAIGHT;
	edge_style[1].width = 1;
	copy_color(edge_style[1].color, solid_red);
	
	// Edges within community 1 in green
	edge_style[2].type = GRAPH_STRAIGHT;
	edge_style[2].width = 1;
	copy_color(edge_style[2].color, solid_green);
	
	int num_point_style = 2;
	int ps[] = {0, 0, 0, 1, 1, 1, 1, 1};
	circle_style_t point_style[2];
	
	// Vertices in community 0 are blue
	point_style[0].radius = radius;
	point_style[0].width = width;
	copy_color(point_style[0].fill, solid_blue);
	copy_color(point_style[0].stroke, black);
	
	// Vertices in community 1 are green
	point_style[1].radius = radius;
	point_style[1].width = width;
	copy_color(point_style[1].fill, solid_green);
	copy_color(point_style[1].stroke, black);
	
	graph_print_svg_some_styles("test/test_some_styles.svg", g, p, 
	                            ps, point_style, num_point_style,
	                            es, edge_style, num_edge_style);
	delete_graph(g);
	free(p);
}

int main(){
	test_one_style();
	test_many_styles();
	test_some_styles();
	printf("success\n");
	return 0;
}
