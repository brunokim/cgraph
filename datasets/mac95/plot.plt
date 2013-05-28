#mac95
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
unset logscale y
unset output

#############

set output 'degree.svg'
set xtics 15
set ytics 4
set grid
set yrange [0:16]

set xlabel 'Degree'
set ylabel 'Number of vertices'

plot 'degree.dat' u 1:2 w points pt 6 lc 1  notitle

unset grid
unset output

#############

set output 'correlation.svg'
set xtics 15
set ytics 15
set size ratio -1
set xrange [0:75]
set yrange [0:75]

set xlabel 'Degree'
set ylabel 'Average neighbor degree'

plot 'correlation.dat' u 1:2 w points pt 0 lc 1 notitle
repl 'knn.dat' u 1:2 w points pt 6 lc 3 notitle

set size ratio 0
unset output

#############

set output 'clustering.svg'
set xrange [0:1]
set yrange [0:15]
set xtics 0.1
set ytics 3

set xlabel 'Local clustering coefficient'
set ylabel 'Number of vertices'

plot 'clustering.dat' u (($1+$2)/2):3 w impulses ls 1 lw 4 notitle

set autoscale
unset output

#############

set output 'betweenness.svg'
set xrange [0:1]
set yrange [0:30]
set xtics 0.1
set ytics 5

set xlabel 'Normalized Betweenness'
set ylabel 'Number of vertices'

plot 'betweenness.dat' u (($1+$2)/2):3 w impulses ls 1 lw 4 notitle

set autoscale
unset output

#############

unset grid
set terminal wxt
