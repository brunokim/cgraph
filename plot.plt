#!/usr/bin/gnuplot

## Command-line variables ##
  if (!exists("DEGREE_LOG_X")) DEGREE_LOG_X = 0
  if (!exists("DEGREE_LOG_Y")) DEGREE_LOG_Y = 0
  if (!exists("CLUSTERING_LOG_Y")) CLUSTERING_LOG_Y = 0
  if (!exists("BETWEENNESS_LOG_Y")) BETWEENNESS_LOG_Y = 0
  if (!exists("EIGENVECTOR_LOG_Y")) EIGENVECTOR_LOG_Y = 0
  if (!exists("PAGERANK_LOG_Y")) PAGERANK_LOG_Y = 0
  if (!exists("CLOSENNESS_LOG_Y")) CLOSENNESS_LOG_Y = 0
  if (!exists("CORE_LOG_Y")) CORE_LOG_Y = 0

## Common settings ##
  unset key
  set terminal svg
  set boxwidth 0.6 relative
  set style fill solid 1.0 noborder

## Degree ##
  set output 'degree.svg'
  
  set xlabel 'Degree'
  set ylabel 'Number of vertices'
  
  if (DEGREE_LOG_X) set logscale x; set xrange [1:*]; set xtics 10; else set xrange [0:*]
  if (DEGREE_LOG_Y) set logscale y; set yrange [0.1:*]; set ytics 10; else set yrange [0:*]
  set grid
  
  plot 'degree.dat' with points pt 6 lc 1
  
  unset output
  unset grid
  if (DEGREE_LOG_X) unset logscale x
  if (DEGREE_LOG_Y) unset logscale y
  set xtics autofreq
  set ytics autofreq

## Degree correlation ##
  set output 'correlation.svg'
  set size ratio -1
  
  set xrange [0:*]
  set yrange [0:*]
  
  set xlabel 'Degree'
  set ylabel 'Average neighbor degree'
  
  plot 'metrics.dat' u 2:4 w points pt 0 lc 1 notitle
  repl 'knn.dat' u 1:2 w points pt 6 lc 3 notitle
  
  set size ratio 0
  unset output
  set xtics autofreq
  set ytics autofreq

## Clustering ##
  set output 'clustering.svg'
  
  if (CLUSTERING_LOG_Y) set logscale y; set yrange [0.1:*]; set ytics 10; else set yrange [0:*]
  
  set xlabel 'Local clustering coefficient'
  set ylabel 'Number of vertices'
	
	set xrange [0:1]
	plot 'clustering.dat' with boxes
  
  if (CLUSTERING_LOG_Y) unset logscale y
  unset output
  set xtics autofreq
  set ytics autofreq

## Betweenness ##
  set output 'betweenness.svg'
  
  set xlabel 'Betwenness'
  set ylabel 'Number of vertices'
	
  if (BETWEENNESS_LOG_Y) set logscale y; set yrange [0.1:*]; set ytics 10; else set yrange [0:*]
  
  set xrange [0:*]
  plot 'betweenness.dat' with boxes
  
  if (BETWEENNESS_LOG_Y) unset logscale y
  unset output
  set xtics autofreq
  set ytics autofreq

## Eigenvector ##
  set output 'eigenvector.svg'
  
  if (EIGENVECTOR_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Eigenvector centrality'
  set ylabel 'Number of vertices'
	
	set xrange [*:*]
	plot 'eigenvector.dat' with boxes
  
  if (EIGENVECTOR_LOG_Y) unset logscale y
  unset output
  set xtics autofreq
  set ytics autofreq

## Pagerank ##
  set output 'pagerank.svg'
  
  if (PAGERANK_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Pagerank centrality'
  set ylabel 'Number of vertices'
	
	set xrange [*:*]
	plot 'pagerank.dat' with boxes
  
  if (PAGERANK_LOG_Y) unset logscale y
  unset output
  set xtics autofreq
  set ytics autofreq
  
## Closenness ##
  set output 'closenness.svg'
  
  if (CLOSENNESS_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Closenness'
  set ylabel 'Number of vertices'
	
	set xrange [*:*]
	plot 'closenness.dat' with boxes
  
  if (CLOSENNESS_LOG_Y) unset logscale y
  unset output
  set xtics autofreq
  set ytics autofreq
  
## K-Core ##
  set output 'k-core.svg'
  
  if (CORE_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Core'
  set ylabel 'Number of vertices'
  
  set xrange [*:*]
  plot 'k-core.dat' with boxes
  
  if (CORE_LOG_Y) unset logscale y
  unset output
  set xtics autofreq
  set ytics autofreq

## Distance ##
  set output 'distance.svg'

  set logscale y
  set xrange [0:*]
  set yrange [1:*]
  set grid

  set xlabel 'Length'
  set ylabel 'Number of paths'
  
  plot 'distance.dat' u 1:2 with lines ls 1 notitle

  unset grid
  unset logscale y
  unset output

# Finish processing
  set term wxt
