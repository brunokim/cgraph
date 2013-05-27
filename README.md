CGraph
======

Simple C graph library, not intended for massive graphs or anything of the
like. Just something I made to learn, and may be useful to others.

### `sorting`

Functions related to searching and sorting.

### `stat`

Statistical-related stuff. Do not touch it, it stinks.

### `list`

Kind of an adaptation of ArrayList from Java, but only works with integers.
    
### `set`

At the moment, it's just a wrapper to a (sorted) list, which has O(log n) 
complexity for `set_contains` and O(1) for `set_get`.
Later, it may be needed to create a hash table to back it, which has
O(1) for `set_contains` and O(n) for `set_get`.

### `graph`

An adjacency list data structure to store connections. You may notice it
lacks variable number of vertices or edge deletion. I do not find it useful
for static complex network research. Maybe I'm wrong.

### `graph_metric`

Several interesting graph metrics, such as degree correlation, centrality and
geodesic distance.

### `graph_layout`

Layouting and printing graphs into SVG files.

For more information, please check the documentation, available in `doc/main.pdf`.
