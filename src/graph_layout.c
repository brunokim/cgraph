#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "sorting.h"
#include "graph.h"
#include "graph_layout.h"

void copy_color(color_t copy, const color_t original){
	copy[0] = original[0];
	copy[1] = original[1];
	copy[2] = original[2];
	copy[3] = original[3];
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
	copy_color(edge_style[0].color, color);
	
	// Parabolic style, for edges that do not lie in the circle
	edge_style[1].type = GRAPH_PARABOLA;
	edge_style[1].control.x = size/2;
	edge_style[1].control.y = size/2;
	edge_style[1].width = width;
	copy_color(edge_style[1].color, color);
	
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

double graph_layout_degree(const graph_t *g, int radius, coord_t *p){
	assert(g);
	assert(radius > 0);
	assert(p);
	
	int i, j, k, n = graph_num_vertices(g);
	int *degree = malloc(n * sizeof(*degree));
	memset(degree, 0, n*sizeof(*degree));
	
	int kmax = 0;
	int num_layer = 0;
	for (i=0; i < n; i++){
		k = graph_num_adjacents(g, i);
		
		if (degree[k] == 0){ num_layer++; }
		degree[k]++;
		kmax = k > kmax ? k : kmax;
	}
	
	int *freq = malloc(num_layer * sizeof(*freq));
	int *layer = malloc(num_layer * sizeof(*layer));
	double drmin = 0.0;
	j = 0;
	for (k=kmax; k >= 0; k--){
		if (degree[k] > 0){
			layer[j] = k;
			freq[j] = degree[k];
			
			double dr = (2*radius)*freq[j]/(2*M_PI*(1+j));
			drmin = dr < drmin ? drmin : dr;
			
			j++;
		}
	}
	assert(j == num_layer);
	free(degree);
	
	double dR = 1.25*(2*radius);
	dR = dR < drmin ? drmin : dR;
	double R = dR*num_layer;
	
	// Random initial angle
	double *t0 = malloc(num_layer * sizeof(*t0));
	for (i=0; i < num_layer; i++){
		t0[i] = 2*M_PI*(double)rand()/RAND_MAX;
	}
	
	int *count = malloc(num_layer * sizeof(*count));
	memset(count, 0, num_layer * sizeof(*count));
	
	for (i=0; i < n; i++){
		k = graph_num_adjacents(g, i);
		for (j=0; j < num_layer; j++){
			if (layer[j] == k){
				break;
			}
		}
		
		double theta = t0[j] + (2 * M_PI * count[j]) / freq[j];
		count[j]++;
		
		p[i].x = R + dR*(1+j) * cos(theta);
		p[i].y = R + dR*(1+j) * sin(theta);
	}
	
	free(t0);
	free(freq);
	free(layer);
	free(count);
	
	return 2*R + 2*radius;
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
