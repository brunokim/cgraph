CGraph
======

Simple C graph library, not intended for massive graphs or anything of the
like. Just something I made to learn, and may be useful to others.

### `sorting`

Functions related to searching and sorting.

### `stat`

Statistical-related stuff. Do not touch it, it stinks.

### `list`

Kind of an adaptation of ArrayList from Java, but obviously only works with
integers.

A list is considered sorted if
* It has just been created; 
* It has just been sorted; or
* Since its creation or sorting, it had elements added with `list_insert`.

So, a list can be _logically_ sorted, but not be _considered_ sorted.

    list_t *list = new_list(0);
    list_push(list, 0);
    list_push(list, 1);
    list_push(list, 4);
    printf("%s", list_is_sorted(list) ? "false" : "true"); //false
    list_sort(list);
    list_insert(list, 3);
    printf("%s", list_is_sorted(list) ? "false" : "true"); //true
    
### `set`

At the moment, it's just a wrapper to a (sorted) list, which has O(log n) 
complexity for `set_contains` and O(1) for `set_get`.
Later, it may be needed to create a hash table to back it, which has
O(1) for `set_contains` and O(n) for `set_get`.

### `graph`

An adjacency list data structure to store connections. You may notice it
lacks variable number of vertices or edge deletion. I do not find it useful
for static complex network research. Maybe I'm wrong.

A common usage pattern I've noticed I use to traverse all adjacents of 
a vertex is like the following:

    int i, j, n = graph_num_vertices(graph);
    int *adj = malloc(n * sizeof(*adj));
    for (i=0; i < n; i++){
      int ki = graph_adjacent(graph, i, adj);
      // Now adj has ki indices, all adjacents to i
      for (j=0; j < ki; j++){
        int v = adj[j];
        assert(graph_is_adjacent(g, i, v)); // Always true
      }
    }

### `graph_metric`

Several interesting metrics.
