#!/bin/bash

datasets="cat mac95 baydry baywet mangdry mangwet email powergrid pgp internet astrophysics 15m"

for dataset in $datasets
do
	bin/propagation -d datasets/$dataset -p SIR 1.0 0.5 -r 50 -o test/$dataset-sir.dat 
done
wait

for dataset in $datasets
do
	scripts/dataset_propagation.r $dataset
done
