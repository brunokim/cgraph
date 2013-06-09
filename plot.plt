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

## General useful functions
  min(x) = (_min = x < _min ? x : _min, rand(0))
  max(x) = (_max = x > _max ? x : _max, rand(0))
  minmax(x) = (\
    _min = x < _min ? x : _min,\
    _max = x > _max ? x : _max,\
    rand(0))
  
  process_data(func, file, col) = \
		sprintf("_min = 1e38;").\
		sprintf("_max = -1e38;").\
		sprintf("set table 'tmp.dat';").\
		sprintf("plot '%s' using %d:(%s($%d));", file, col, func, col).\
		sprintf("unset table")
	
	approx_minmax_data(file, col) = \
	  sprintf("eval process_data(\"minmax\", '%s', %d);", file, col).\
	  sprintf("delta = _max - _min;").\
	  sprintf("order = (o = log10(delta), o < 0 ? floor(o) : ceil(o));").\
	  sprintf("max = order < 0 ? (ceil(_max * 10**(-order)) * 10**(order)) : (ceil(_max * 10**(1-order)) * 10**(order-1));").\
	  sprintf("min = order < 0 ? (floor(_min * 10**(-order)) * 10**(order)): (floor(_min * 10**(1-order)) * 10**(order-1));").\
	  sprintf("print '_min: ', _min,', _max: ',_max,', min:',min,', max:',max,', delta:',delta,', order:',order")
	
## Histogram functions ##
  bin_number(x) = floor(x/bin_width)
  rounded(x) = bin_width * ( bin_number(x) + 0.5 )
  
	plot_histogram(min, max, file, col) = \
	  sprintf("delta = %f - %f;", max, min).\
	  sprintf("set xrange [%f:%f];", min, max).\
	  sprintf("set xtics %f,delta/10.0,%f;", min, max).\
	  sprintf("bin_width = delta/20.0;").\
	  sprintf("set boxwidth 0.6*bin_width absolute;").\
	  sprintf("plot '%s' using (rounded($%d)):(1) smooth frequency with boxes", file, col)

# Start processing
  unset key
  set terminal svg
  set style fill solid 1.0 noborder

## Distance ##
  set output 'distance.svg'

  set logscale y
  set xrange [0:*]
  set yrange [1:*]
  set grid

  set xlabel 'Length'
  set ylabel 'Number of paths'
  
  plot 'distance.dat' u 1:2 w lines ls 1 notitle

  unset grid
  unset logscale y
  unset output

## Degree ##
  set output 'degree.svg'
  
  if (DEGREE_LOG_X) set logscale x; set xrange [1:*]
  if (DEGREE_LOG_Y) set logscale y
  set xrange [1:*]
  set yrange [1:*]
  set grid
  
  set xtics autofreq
  
  set xlabel 'Degree'
  set ylabel 'Number of vertices'
  
  plot 'metrics.dat' u 2:(1) smooth freq w points pt 6 lc 1 notitle
  
  unset output
  unset grid
  if (DEGREE_LOG_X) unset logscale x
  if (DEGREE_LOG_Y) unset logscale y

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

## Clustering ##
  set output 'clustering.svg'
  
  if (CLUSTERING_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Local clustering coefficient'
  set ylabel 'Number of vertices'
	
	print "Clustering..."
	eval plot_histogram(0.0, 1.0, "metrics.dat", 3)

  if (CLUSTERING_LOG_Y) unset logscale y
  unset output

## Betweenness ##
  set output 'betweenness.svg'
  
  set xlabel 'Betwenness'
  set ylabel 'Number of vertices'
	
  if (BETWEENNESS_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  print "Betweenness..."
  eval approx_minmax_data("metrics.dat", 5) 
  eval plot_histogram(min, max, "metrics.dat", 5)
  
  if (BETWEENNESS_LOG_Y) unset logscale y
  unset output

## Eigenvector ##
  set output 'eigenvector.svg'
  
  if (EIGENVECTOR_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Eigenvector centrality'
  set ylabel 'Number of vertices'
	
	print "Eigenvector..."
  eval approx_minmax_data("metrics.dat", 6) 
  eval plot_histogram(min, max, "metrics.dat", 6)
  
  if (EIGENVECTOR_LOG_Y) unset logscale y
  unset output

## Pagerank ##
  set output 'pagerank.svg'
  
  if (PAGERANK_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Pagerank centrality'
  set ylabel 'Number of vertices'
	
	print "Pagerank..."
  eval approx_minmax_data("metrics.dat", 7) 
  eval plot_histogram(min, max, "metrics.dat", 7)
  
  if (PAGERANK_LOG_Y) unset logscale y
  unset output
  
## Closenness ##
  set output 'closenness.svg'
  
  if (CLOSENNESS_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Closenness'
  set ylabel 'Number of vertices'
	
	print "Closenness..."
  eval approx_minmax_data("metrics.dat", 8) 
  eval plot_histogram(min, max, "metrics.dat", 8)
  
  if (CLOSENNESS_LOG_Y) unset logscale y
  unset output
  
## K-Core ##
  set output 'k-core.svg'
  
  if (CORE_LOG_Y) set logscale y; set yrange [0.1:*]; else set yrange [0:*]
  
  set xlabel 'Core'
  set ylabel 'Number of vertices'
  
  print "K-core..."
  set xrange [*:*]
  set xtics autofreq
  
  set boxwidth 0.6 absolute
  plot 'metrics.dat' u 9:(1) smooth freq with boxes
  
  if (CORE_LOG_Y) unset logscale y
  unset output

# Finish processing
  set term wxt
