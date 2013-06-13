#ifndef _GRAPH_LAYOUT_H
#define _GRAPH_LAYOUT_H

/********************************* Types *************************************/
typedef struct{
	float x, y;
} coord_t;

typedef struct {
	coord_t sw, ne;
} box_t;

typedef enum {COLOR_R, COLOR_G, COLOR_B, COLOR_A} color_rgb_t;
typedef enum {COLOR_H, COLOR_S, COLOR_V} color_hsv_t;
#define COLOR_MAX 255
typedef short color_t[4];

typedef struct{
	short radius;
	short width;
	color_t fill;
	color_t stroke;
} circle_style_t;

typedef enum{GRAPH_STRAIGHT, GRAPH_PARABOLA, GRAPH_CIRCULAR} edge_type_t;

typedef struct{
	coord_t from, to, control;
	edge_type_t type;
	short width;
	color_t color;
} path_style_t;

/***************************** Color functions ********************************/
// Copy color from original to copy
void color_copy(color_t copy, const color_t original);
// Convert from HSV to RGB
void color_hsv_to_rgb(color_t rgb, const color_t hsv);
// Convert from RGB to HSV
void color_rgb_to_hsv(color_t hsv, const color_t rgb);

/********************************* Layout *************************************/
// Place points uniformly inside specified box
void graph_layout_random(box_t box, coord_t *p, int n);
// Place points with specified radius uniformly avoiding overlap with 
// probability t
void graph_layout_random_wout_overlap(int radius, double t, coord_t *p, int n);

// Place points with specified radius in a circle without overlap
double graph_layout_circle(int radius, coord_t *p, int n);
// Fill edge style for a circular layout.
void graph_layout_circle_edges
		(const graph_t *g, double size, int width, const color_t color, 
		 int *es, path_style_t edge_style[]);

// Place points in concentric shells, with highest degrees near the center
double graph_layout_degree(const graph_t *g, int radius, coord_t *p);
double graph_layout_degree2
	(const graph_t *g, int radius, coord_t *p, bool is_random_angle);
/******************************* Printing *************************************/

// Prints graph as SVG to file, using vertex coordinates given in p and
// with a style for each point and edge.
void graph_print_svg
	(const char *filename,
	 int width, int height, 
	 const graph_t *g, 
	 const coord_t *p, 
	 const circle_style_t *point_style,
	 const path_style_t *edge_style);

// Prints graph as SVG to file, using vertex coordinates given in p and
// with a single style for all points and edges.
void graph_print_svg_one_style
	(const char *filename,
	 int width, int height, 
	 const graph_t *g, 
	 const coord_t *p, 
	 const circle_style_t point_style, 
	 const path_style_t edge_style);

// Prints graph as SVG to file, using vertex coordinates given in p and
// with a number of styles given. The mapping vertex->style is given in ps,
// and the mapping edge->style is given in es.
void graph_print_svg_some_styles
	(const char *filename,
	 int width, int height, 
	 const graph_t *g, 
	 const coord_t *p, 
	 const int *ps, const circle_style_t *point_style, int num_point_style,
	 const int *es, const path_style_t *edge_style, int num_edge_style);

#endif
