CGraph
======

C graph library for complex networks research. Graphs are stored in-memory 
in the most compact form possible, hoping to be the most efficient and 
parallel implementation available.

The following example is available in `example/printing.c`, and showcases
how to create a Barabási-Albert network and plot it as an SVG.

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

![Barabási-Albert graph](https://github.com/brunokim/cgraph/raw/master/barabasi.png)

## Version

The current version is 0.1 - id est, alpha stage, when API (most surely)
will break.

## Organization

Code is factored in modules as follow. Each module has corresponding archives

* src/_module_.c
* include/_module_.h
* test/test\__module_.c
* doc/_module_.tex

### `sorting`

Functions related to searching and sorting.

### `stat`

Statistical-related stuff. Needs a major refactoring.

### `list`

Array list implementation for ints, with support for sorted operations that
allow O(log n) searching and insertion.
    
### `set`

Hash set implementation for (positive) ints, with O(1) insertion, and O(n) deletion.

### `graph`

Basic operations for creating and populating graphs with a fixed number of vertices.
Edge deletion is not supported, and needs some refactorings to support directed graphs
properly.

### `graph_metric`

Several interesting graph metrics, such as degree correlation, centrality and
geodesic distance.

### `graph_layout`

Layouting and printing graphs into SVG files.

### `graph_model`

Common models in Complex Network research: clique, random, scale-free and 
small-world included.

### `graph_propagation`

Information propagation models in networks: SI, SIS, SIR, SEIR and 
Daley-Kendall.

### `graph_game`

Game theory module, with Iterated Prisioner Dillema implementation.

## Further documentation

For more information, please check the documentation, available in `doc/main.pdf`.
