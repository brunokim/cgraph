#!/usr/bin/gnuplot

FILE = 'test/test_clustering_propagation.dat'

unset key
set title 'WS, N=10^4, k=8'
set terminal svg

set xlabel 'beta'
set ylabel 'Transitivity'
set output 'test/beta-transitivity.svg'
plot FILE u 1:2 w lines
unset output

set xlabel 'beta'
set ylabel 'Average local clustering'
set output 'test/beta-clustering.svg'
plot FILE u 1:3:4 w yerrorbars
repl FILE u 1:3 w lines lc 3
unset output

set xlabel 'Transitivity'
set ylabel 'Average local clustering'
set output 'test/transitivity-clustering.svg'
plot FILE u 2:3:4 w yerrorbars
repl FILE u 2:3 w lines lc 3
unset output

set xlabel 'Transitivity'
set ylabel 'Proportion of susceptibles'
set output 'test/transitivity-susceptibles.svg'
plot FILE u 2:5:6 w yerrorbars
repl FILE u 2:5 w lines lc 3
unset output

set xlabel 'Transitivity'
set ylabel 'Time'
set output 'test/transitivity-time.svg'
plot FILE u 2:7:8 w yerrorbars
repl FILE u 2:7 w lines lc 3
unset output
