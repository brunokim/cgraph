#!/bin/bash

for d in cat mac95 email powergrid astrophysics internet
do
	cd $d
	echo "Plotting in $d..."
	gnuplot plot.plt
	cd ..
done

