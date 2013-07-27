#include <stdlib.h>
#include <stdbool.h>

#include "graph.h"
#include "graph_model.h"
#include "graph_layout.h"

int main(){
	int n = 100, k = 4;
	// Creates a graph using the Barabási-Albert model
	graph_t *g = new_barabasi_albert(n, k);
	// Position of each node
	coord_t *points = malloc(n * sizeof(*points));
	
	// Styles to be printed
  int radius = 5, width = 1;
	circle_style_t circle = {
    .width = width, .radius = radius, 
    .fill = {255, 0, 0, 255}, // color is a 4-tuple
    .stroke = {0, 0, 0, 255}
  };
 
  path_style_t edge = {
    .type = GRAPH_STRAIGHT,
    .width = width,
    .color = {0, 0, 0, 255}
  };
 
  // Places vertices in shells, where higher degree vertices are 
  //internal and lower degree are external.
  int size = (int)
    graph_layout_degree_shell(g, radius, true, points);
 
  // Plots graph at file barabasi.svg
  graph_print_svg_one_style(
    "barabasi.svg", size+1, size+1, g, points, circle, edge);
       
  delete_graph(g);
  free(points);
  return 0;
}
