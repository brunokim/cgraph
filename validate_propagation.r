#!/usr/bin/Rscript

model <- commandArgs(trailingOnly=TRUE)

dynamic <- read.table(sprintf("test/%s-dynamic.dat", model))
num_steps <- dim(dynamic)[1]

clique <- read.table(sprintf("test/%s-clique.dat", model))

time_avg <- mean(clique$V1)
time_stddev <- sd(clique$V1)
s_avg <- mean(clique$V3)
s_stddev <- sd(clique$V3)

cat(sprintf("%s dynamic -- time: %f, susceptible: %f\n", model, dynamic[num_steps,1], dynamic[num_steps, 2]))
cat(sprintf("%s clique  -- time: %f+-%f, susceptible: %f+-%f\n", model, time_avg, time_stddev, s_avg, s_stddev))
