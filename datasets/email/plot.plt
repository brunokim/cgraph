#email
set terminal svg

#############

set output 'distance.svg'
set logscale y
set xtics 1
set yrange [1:]
set grid

set xlabel 'Length'
set ylabel 'Number of paths'

plot 'distance.dat' u 1:2 w lines ls 1 notitle

unset grid
unset logscale
unset output

#############

set output 'degree.svg'
set grid
set xtics 10
set ytics 10
set xrange [1:100]
set yrange [1:1000]
set logscale

set xlabel 'Degree'
set ylabel 'Number of vertices'

plot 'degree.dat' u 1:2 w points pt 6 lc 1  notitle

unset grid
unset logscale
unset output

#############

set output 'correlation.svg'
set xtics 10
set ytics 10
set size ratio -1
set xrange [1:100]
set yrange [1:100]
set logscale

set xlabel 'Degree'
set ylabel 'Average neighbor degree'

plot 'correlation.dat' u 1:2 w points pt 0 lc 1 notitle
repl 'knn.dat' u 1:2 w points pt 6 lc 3 notitle

unset logscale  
set size ratio 0
unset output

#############

set output 'clustering.svg'
set xrange [0:1]
set yrange [0.1:1000]
set xtics 0.1
set ytics 10
set logscale y

set xlabel 'Local Clustering Coefficient'
set ylabel 'Number of vertices'

plot 'clustering.dat' u (($1+$2)/2):3 w impulses ls 1 lw 4 notitle

unset logscale
set autoscale
unset output

#############

set output 'betweenness.svg'
set xrange [0:1]
set yrange [0.1:1000]
set xtics 0.1
set logscale y

set xlabel 'Betweenness'
set ylabel 'Number of vertices'

plot 'betweenness.dat' u (($1+$2)/2):3 w impulses ls 1 lw 4 notitle

unset logscale
set autoscale
unset output

#############

unset grid
set terminal wxt
