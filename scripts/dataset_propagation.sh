#!/bin/bash

datasets="mac95 cat mangwet mangdry baywet baydry netscience email facebook powergrid pgp astrophysics internet enron 15m"

for dataset in $datasets
do
	bin/propagation -d datasets/$dataset -p SIR 1.0 0.5 -r 50 -o test/$dataset-sir.dat &
done
wait

for dataset in $datasets
do
	scripts/dataset_propagation.r $dataset &
done
wait

scripts/dataset_propagation_boxplot.r $datasets
