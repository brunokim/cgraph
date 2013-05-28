#!/bin/bash

for d in cat mac95 email powergrid astrophysics as
do
	echo "Converting files in $d"
	cd $d
	for f in *.svg
	do
		convert $f ${f%.*}.pdf
	done
	cd ..
done
