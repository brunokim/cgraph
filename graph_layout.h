#ifndef _GRAPH_LAYOUT_H
#define _GRAPH_LAYOUT_H

/********************************* Types *************************************/
typedef struct{
	float x, y;
} coord_t;

typedef struct {
	coord_t sw, ne;
} box_t;

typedef struct{
	int radius;
	int width;
	int color[4];
} circle_style_t;

typedef enum{GRAPH_STRAIGHT, GRAPH_PARABOLA, GRAPH_CIRCULAR} edge_type_t;

typedef struct{
	coord_t from, to, control;
	edge_type_t type;
	int width;
	int color[4];
	
} path_style_t;

/********************************* Layout *************************************/

// Place points uniformly inside specified box
void graph_layout_random(box_t box, coord_t *p, int n);
// Place points with specified radius uniformly avoiding overlap with 
// probability t
void graph_layout_random_wout_overlap(int radius, double t, coord_t *p, int n);

/******************************* Printing *************************************/

// Prints graph as SVG to file, using vertex coordinates given in p and
// with a style for each point and edge.
void graph_print_svg
	(const char *filename,
	 const graph_t *g, 
	 const coord_t *p, 
	 const circle_style_t *point_style,
	 const path_style_t *edge_style);

// Prints graph as SVG to file, using vertex coordinates given in p and
// with a single style for all points and edges.
void graph_print_svg_one_style
	(const char *filename,
	 const graph_t *g, 
	 const coord_t *p, 
	 const circle_style_t point_style, 
	 const path_style_t edge_style);

// Prints graph as SVG to file, using vertex coordinates given in p and
// with a number of styles given. The mapping vertex->style is given in ps,
// and the mapping edge->style is given in es.
void graph_print_svg_some_styles
	(const char *filename,
	 const graph_t *g, 
	 const coord_t *p, 
	 const int *ps, const circle_style_t *point_style, int num_point_style,
	 const int *es, const path_style_t edge_style, int num_edge_style);

#endif
