#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "graph.h"
#include "graph_layout.h"

/********************************* Layout *************************************/

void graph_layout_random(box_t box, coord_t *p, int n){
	
}
void graph_layout_random_wout_overlap(int radius, double t, coord_t *p, int n){
	
}

/******************************* Printing *************************************/

void graph_print_svg
		(const char *filename,
		 const graph_t *g, 
		 const coord_t *p, 
		 const circle_style_t *point_style,
		 const path_style_t *edge_style){
	assert(filename);
	assert(g);
	assert(p);
	assert(point_style);
	assert(edge_style);
	
	
}

void graph_print_svg_one_style
		(const char *filename,
		 const graph_t *g, 
		 const coord_t *p, 
		 const circle_style_t point_style,
		 const path_style_t edge_style){
	assert(filename);
	assert(g);
	assert(p);
	
}

void graph_print_svg_some_styles
		(const char *filename,
		 const graph_t *g, 
		 const coord_t *p, 
		 const int *ps, const circle_style_t *point_style, int num_point_style,
		 const int *es, const path_style_t edge_style, int num_edge_style){
	assert(filename);
	assert(g);
	assert(p);
	assert(ps); assert(point_style); 
	assert(num_point_style > 0);
	assert(es); assert(edge_style); 
	assert(num_edge_style > 0);
			 
}
