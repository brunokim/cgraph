#!/usr/bin/Rscript

dataset <- commandArgs(trailingOnly=TRUE)
sir <- read.table(sprintf("test/%s-sir.dat", dataset))

time_avg <- mean(sir$V1)
time_stddev <- sd(sir$V1)

n <- sir[1,3] + sir[1,5] # Number of vertices
s_avg <- mean(sir$V3)/n
s_stddev <- sd(sir$V3)/n

cat(sprintf("%*s & %d & $%.2f \\pm %.1f$ & $%.2f \\pm %.1f$ \\\\\n", 12, dataset, as.integer(n), time_avg, time_stddev, 100*s_avg, 100*s_stddev))
