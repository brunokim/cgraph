#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "graph.h"
#include "graph_layout.h"

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
	printf("l: %f\n", l);
	box_t box = {{0.0f, 0.0f}, {l, l}};
	graph_layout_random(box, p, n);
	
	int i;
	for (i=0; i < n; i++){
		printf("(%6.3f, %6.3f)\n", p[i].x, p[i].y);
	}
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
	
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n");
	
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
		 const graph_t *g, 
		 const coord_t *p, 
		 const circle_style_t point_style,
		 const path_style_t edge_style){
	assert(filename);
	assert(g);
	assert(p);
	
	FILE *fp = fopen(filename, "wt");
	if (!fp){ return; }
	
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n");
	
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
	
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n");
	
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
					{p[i].x, p[i].y}, {p[v].x, p[v].y}, {0.0f, 0.0f},
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
