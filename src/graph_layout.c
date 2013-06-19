#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "sorting.h"
#include "stat.h"
#include "graph.h"
#include "graph_metric.h"
#include "graph_layout.h"

/***************************** Color functions ********************************/

#define MIN2(a,b) ((a)<(b)?(a):(b))
#define MIN3(a,b,c) (MIN2(MIN2(a,b),c))

#define MAX2(a,b) ((a)>(b)?(a):(b))
#define MAX3(a,b,c) (MAX2(MAX2(a,b),c))

void color_copy(color_t copy, const color_t original){
	copy[0] = original[0];
	copy[1] = original[1];
	copy[2] = original[2];
	copy[3] = original[3];
}

typedef struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

typedef struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;

// Adapted from http://stackoverflow.com/a/14733008/946814
void color_hsv_to_rgb(color_t rgb, const color_t hsv)
{
    rgb[COLOR_A] = hsv[COLOR_A];
    short h = hsv[COLOR_H], s = hsv[COLOR_S], v = hsv[COLOR_V];
    
    if (s == 0)
    {
        rgb[0] = rgb[1] = rgb[2] = v;
        return;
    }

    short region = h / 43;
    short remainder = (h - (region * 43)) * 6; 

    short p = (v * (255 - s)) >> 8;
    short q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    short t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            rgb[0] = v; rgb[1] = t; rgb[2] = p;
            break;
        case 1:
            rgb[0] = q; rgb[1] = v; rgb[2] = p;
            break;
        case 2:
            rgb[0] = p; rgb[1] = v; rgb[2] = t;
            break;
        case 3:
            rgb[0] = p; rgb[1] = q; rgb[2] = v;
            break;
        case 4:
            rgb[0] = t; rgb[1] = p; rgb[2] = v;
            break;
        default:
            rgb[0] = v; rgb[1] = p; rgb[2] = q;
            break;
    }
}

// Adapted from http://stackoverflow.com/a/14733008/946814
void color_rgb_to_hsv(color_t hsv, const color_t rgb){
	hsv[COLOR_A] = rgb[COLOR_A];
	
	short r = rgb[COLOR_R], g = rgb[COLOR_G], b = rgb[COLOR_B];
	
	short min = r < g ? (r < b ? r : b) : (g < b ? g : b);
  short max = r > g ? (r > b ? r : b) : (g > b ? g : b);
  
  hsv[COLOR_V] = max;

	if (hsv[COLOR_V] == 0)
	{
			hsv[COLOR_H] = 0;
			hsv[COLOR_S] = 0;
			return;
	}
	
	long delta = max - min;
  hsv[COLOR_S] = 255 * delta / hsv[COLOR_V];
	if (hsv[COLOR_S] == 0)
	{
			hsv[COLOR_H] = 0;
			return;
	}

	if (max == r)
	{
		hsv[COLOR_H] = 0 + 43 * (g - b) / delta;
	}
	else if (max == g)
	{
		hsv[COLOR_H] = 85 + 43 * (b - r) / delta;
	}
	else
	{
		hsv[COLOR_H] = 171 + 43 * (r - g) / delta;
	}
}

/********************************* Layout *************************************/

void graph_layout_random(box_t box, coord_t *p, int n){
	assert(box.sw.x < box.ne.x);
	assert(box.sw.y < box.ne.y);
	assert(p);
	assert(n >= 0);
	
	float xrange = box.ne.x - box.sw.x;
	float yrange = box.ne.y - box.sw.y;
	
	int i;
	for (i=0; i < n; i++){
		p[i].x = box.sw.x + xrange * (float)rand()/RAND_MAX;
		p[i].y = box.sw.y + yrange * (float)rand()/RAND_MAX;
	}
}
void graph_layout_random_wout_overlap(int radius, double t, coord_t *p, int n){
	assert(radius > 0);
	assert(0.0 <= t && t <= 1.0);
	assert(p);
	assert(n > 0);
	
	float l = n * radius/2.0 * sqrt(2*M_PI / -log(1-t));
	box_t box = {{0.0f, 0.0f}, {l, l}};
	graph_layout_random(box, p, n);
}

double graph_layout_circle(int radius, coord_t *p, int n){
	assert(radius > 0);
	assert(p);
	assert(n > 0);
	double R = 1.25 * (2*radius)*n/(2*M_PI);
	double size = 2*R + 2*radius;
	
	int i;
	for (i=0; i < n; i++){
		p[i].x = R+radius + R*cos((2*M_PI*i)/n);
		p[i].y = R+radius + R*sin((2*M_PI*i)/n);
	}
	
	return size;
}

void graph_layout_circle_edges
		(const graph_t *g, double size, int width, const color_t color, 
		 int *es, path_style_t edge_style[]){
	assert(g);
	assert(size > 0.0);
	assert(width > 0);
	assert(color);
	assert(es);
	
	// Circular style, for edges that lie in the circle when |i - v| == 1 
	edge_style[0].type = GRAPH_CIRCULAR;
	edge_style[0].control.x = size/2;
	edge_style[0].control.y = size/2;
	edge_style[0].width = width;
	color_copy(edge_style[0].color, color);
	
	// Parabolic style, for edges that do not lie in the circle
	edge_style[1].type = GRAPH_PARABOLA;
	edge_style[1].control.x = size/2;
	edge_style[1].control.y = size/2;
	edge_style[1].width = width;
	color_copy(edge_style[1].color, color);
	
	int e = 0; // edge count
	int i, j, n = graph_num_vertices(g);
	int *adj = malloc (n*sizeof(*adj));
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			if (graph_is_directed(g) || i < v){
				if (v-i == 1 || i-v == 1 || i-v == n || v-i == n){
					es[e] = 0; // circular
				} else {
					es[e] = 1; // parabola
				}
				e++;
			}
		}
	}
	assert(e == graph_num_edges(g));
	free(adj);
}

// Places vertices in concentric shells in inverse direction to its shell[i]
//value
double graph_layout_shell
		(const graph_t *g, int radius, int *shell, 
		 bool is_inverse, bool is_random_angle,
		 coord_t *p){
	assert(g);
	assert(radius > 0);
	assert(shell);
	assert(p);
	
	int i, j, n = graph_num_vertices(g);
	int num_shell;
	pair_t *freq = stat_frequencies(shell, n, &num_shell);
	
	// Calculates minimum dr so that at each shell the circles do not overlap
	double dr_min = 0.0;
	for (i=0; i < num_shell; i++){
		int index = !is_inverse ? i : (num_shell - i-1);
		double shell_perimeter = 2 * M_PI * (1+index);
		double dr = (2*radius) * freq[i].value / shell_perimeter;
		dr_min = dr < dr_min ? dr_min : dr;
	}
	
	// Calculates minimum dr so that consecutive shells do not overlap as well
	double dR = 1.25*(2*radius);
	dR = dR < dr_min ? dr_min : dR;
	double R = dR*num_shell;
	
	// Random initial angle
	double *t0 = malloc(num_shell * sizeof(*t0));
	for (i=0; i < num_shell; i++){
		if (is_random_angle){ t0[i] = 2*M_PI*(double)rand()/RAND_MAX; }
		else                { t0[i] = 0.0; }
	}
	
	// Counts how many vertices were already placed at each shell
	int *count = malloc(num_shell * sizeof(*count));
	memset(count, 0, num_shell * sizeof(*count));
	
	// Place the vertices
	for (i=0; i < n; i++){
		// Linear search
		for (j=0; j < num_shell; j++){
			if (freq[j].key == shell[i]){
				break;
			}
		}
		int index = !is_inverse ? j : (num_shell - j-1);
		
		double theta = t0[index] + (2 * M_PI * count[index]) / freq[j].value;
		count[index]++;
		
		p[i].x = R+radius + dR*(1+index) * cos(theta);
		p[i].y = R+radius + dR*(1+index) * sin(theta);
	}
	
	free(count);
	free(t0);
	free(freq);
	
	return 2*R + 2*radius;
}

double graph_layout_degree_shell
		(const graph_t *g, int radius, bool is_random_angle, coord_t *p){
	assert(g);
	int n = graph_num_vertices(g);
	int *degree = malloc(n * sizeof(*degree));
	
	graph_degree(g, degree);
	double boxsize;
	boxsize = graph_layout_shell(g, radius, degree, true, is_random_angle, p);
	
	free(degree);
	return boxsize;
}

double graph_layout_core_shell
		(const graph_t *g, int radius, bool is_random_angle, coord_t *p){
	assert(g);
	int n = graph_num_vertices(g);
	int *core = malloc(n * sizeof(*core));
	
	graph_kcore(g, core);
	double boxsize;
	boxsize = graph_layout_shell(g, radius, core, true, is_random_angle, p);
	
	free(core);
	return boxsize;
}

/******************************* Printing *************************************/

// Finds a circle that contains p1 and p2 as close as possible to pc.
// The circle is defined by its center c and radius.
// theta is the angle in degrees between the vector (p2 - p1) and the vertical.
void graph_find_circle
		(const coord_t p1, const coord_t p2, const coord_t pc,
	   coord_t c, float *radius, float *theta){
	coord_t r = { p2.x - p1.x, p2.y - p1.y};
	coord_t m = { (p1.x + p2.x)/2, (p1.y + p2.y)/2 };
				
	float lambda = (pc.y - m.y) * r.x - (pc.x - m.x) * r.y;
	float d = hypot(r.x, r.y);
	
	c.x = m.x - lambda*(r.y / d);
	c.y = m.y + lambda*(r.x / d);
	
	if (radius){ *radius = hypot(c.x - p1.x, c.y - p1.y); }
	if (theta){ *theta = atan2(c.y - m.y, c.x - m.x) * (180/M_PI); }
}

void graph_print_svg_path(FILE *fp, const path_style_t style){
	float x1 = style.from.x,    y1 = style.from.y;
	float x2 = style.to.x,      y2 = style.to.y;
	float xc = style.control.x, yc = style.control.y;
				
	// Print "d" attribute according to type
	fprintf(fp, "  <path d=\"");
	switch(style.type){
		case GRAPH_STRAIGHT:
			fprintf(fp, "M%d,%d L%d,%d", (int)x1, (int)y1, (int)x2, (int)y2);
			break;
		case GRAPH_PARABOLA:
			fprintf(fp, "M%d,%d Q%d,%d %d,%d",
							(int)x1, (int)y1, (int)xc, (int)yc, (int)x2, (int)y2);
			break;
		case GRAPH_CIRCULAR:
			{
				coord_t c;
				float radius, theta;
				graph_find_circle(style.from, style.to, style.control, 
				                  c, &radius, &theta);
				fprintf(fp, "M%d,%d A%d,%d %d 0,1 %d,%d", 
								(int)x1, (int)y1, (int)radius, (int)radius, (int)theta,
								(int)x2, (int)y2);
			}
			break;
	}
	fprintf(fp, "\" ");
	
	fprintf(fp, "fill=\"none\" ");
	fprintf(fp, "stroke-width=\"%d\" ", style.width);
	fprintf(fp, "stroke=\"rgb(%d,%d,%d)\" ", 
					style.color[COLOR_R], 
					style.color[COLOR_G], 
					style.color[COLOR_B]);				
	fprintf(fp, "stroke-opacity=\"%.3f\" ", 
					(float)style.color[COLOR_A]/COLOR_MAX);
	fprintf(fp, "/>\n");
}

void graph_print_svg_circle
		(FILE *fp, const coord_t p, const circle_style_t style){
	fprintf(fp, "  <circle ");
	fprintf(fp, "cx=\"%d\" cy=\"%d\" ", (int)p.x, (int)p.y);
	fprintf(fp, "r=\"%d\" ", style.radius);
	fprintf(fp, "fill=\"rgb(%d,%d,%d)\" ", 
					style.fill[COLOR_R], style.fill[COLOR_G], style.fill[COLOR_B]);
	fprintf(fp, "fill-opacity=\"%.3f\" ", 
					(float)style.fill[COLOR_A] / COLOR_MAX);
	fprintf(fp, "stroke=\"rgb(%d,%d,%d)\" ", 
					style.stroke[COLOR_R], style.stroke[COLOR_G], style.stroke[COLOR_B]);
	fprintf(fp, "stroke-opacity=\"%.3f\" ", 
					(float)style.stroke[COLOR_A] / COLOR_MAX);
	fprintf(fp, "stroke-width=\"%d\" ", style.width);
	fprintf(fp, "/>\n");
}

void graph_print_svg
		(const char *filename,
		 int width, int height, 
		 const graph_t *g, 
		 const coord_t *p, 
		 const circle_style_t *point_style,
		 const path_style_t *edge_style){
	assert(filename);
	assert(g);
	assert(p);
	assert(point_style);
	assert(edge_style);
	
	FILE *fp = fopen(filename, "wt");
	if (!fp){ return; }
	
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" ");
	if (width > 0){ fprintf(fp, "width=\"%dpx\" ", width); }
	if (height > 0){ fprintf(fp, "height=\"%dpx\" ", height); }
	fprintf(fp, ">\n");
	
	// Print edges
	int i, j, n = graph_num_vertices(g), m = graph_num_edges(g);
	int *adj = malloc(n * sizeof(*adj));
	
	int e=0; //Edge counter
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			if (graph_is_directed(g) || i < v){
				graph_print_svg_path(fp, edge_style[e]);
				e++;
			}
		}
	}
	assert(e == m);
	free(adj);
	
	//Print vertices
	for (i=0; i < n; i++){
		graph_print_svg_circle(fp, p[i], point_style[i]);
	}
	
	fprintf(fp, "</svg>");
	fclose(fp);
}

void graph_print_svg_one_style
		(const char *filename,
		 int width, int height,
		 const graph_t *g, 
		 const coord_t *p, 
		 const circle_style_t point_style,
		 const path_style_t edge_style){
	assert(filename);
	assert(g);
	assert(p);
	
	FILE *fp = fopen(filename, "wt");
	if (!fp){ return; }
	
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" ");
	if (width > 0){ fprintf(fp, "width=\"%dpx\" ", width); }
	if (height > 0){ fprintf(fp, "height=\"%dpx\" ", height); }
	fprintf(fp, ">\n");
	
	// Print edges
	int i, j, n = graph_num_vertices(g), m = graph_num_edges(g);
	int *adj = malloc(n * sizeof(*adj));
	
	int e=0; //Edge counter
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			if (graph_is_directed(g) || i < v){
				path_style_t style = {
					{p[i].x, p[i].y}, {p[v].x, p[v].y}, {0.0f, 0.0f},
					GRAPH_STRAIGHT,
					edge_style.width,
					{edge_style.color[0], edge_style.color[1], edge_style.color[2],
					edge_style.color[3]}
				};
				graph_print_svg_path(fp, style);
				e++;
			}
		}
	}
	assert(e == m);
	free(adj);
	
	//Print vertices
	for (i=0; i < n; i++){
		graph_print_svg_circle(fp, p[i], point_style);
	}
	
	fprintf(fp, "</svg>");
	fclose(fp);
}

void graph_print_svg_some_styles
		(const char *filename,
		 int width, int height,
		 const graph_t *g, 
		 const coord_t *p, 
		 const int *ps, const circle_style_t *point_style, int num_point_style,
		 const int *es, const path_style_t *edge_style, int num_edge_style){
	assert(filename);
	assert(g);
	assert(p);
	assert(ps); assert(point_style); 
	assert(num_point_style > 0);
	assert(es); assert(edge_style); 
	assert(num_edge_style > 0);
	
	FILE *fp = fopen(filename, "wt");
	if (!fp){ return; }
	
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" ");
	if (width > 0){ fprintf(fp, "width=\"%dpx\" ", width); }
	if (height > 0){ fprintf(fp, "height=\"%dpx\" ", height); }
	fprintf(fp, ">\n");
	
	// Print edges
	int i, j, n = graph_num_vertices(g), m = graph_num_edges(g);
	int *adj = malloc(n * sizeof(*adj));
	
	int e=0; //Edge counter
	for (i=0; i < n; i++){
		int ki = graph_adjacents(g, i, adj);
		for (j=0; j < ki; j++){
			int v = adj[j];
			if (graph_is_directed(g) || i < v){
				int s = es[e];
				path_style_t style = {
					{p[i].x, p[i].y}, {p[v].x, p[v].y}, 
					{edge_style[s].control.x, edge_style[s].control.y},
					edge_style[s].type,
					edge_style[s].width,
					{edge_style[s].color[0], 
					 edge_style[s].color[1], 
					 edge_style[s].color[2],
					 edge_style[s].color[3]}
				};
				graph_print_svg_path(fp, style);
				e++;
			}
		}
	}
	assert(e == m);
	free(adj);
	
	//Print vertices
	for (i=0; i < n; i++){
		graph_print_svg_circle(fp, p[i], point_style[ ps[i] ]);
	}
	
	fprintf(fp, "</svg>");
	fclose(fp);
}
