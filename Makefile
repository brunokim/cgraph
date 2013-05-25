CC       = gcc
CFLAGS = -Wall -g

all: experiment test_list test_set test_graph test_metric test_stat 

doc: doc.pdf

run-tests: test_list test_set test_graph test_metric test_stat test_layout
	./test_list
	./test_stat
	./test_set
	./test_graph
	./test_metric
	./test_layout

*.pdf: *.tex
	pdflatex $^
	
experiment : experiment.o graph_metric.o
	$(CC) $(CFLAGS) -o experiment experiment.o graph_metric.o graph.o set.o list.o sorting.o stat.o -pthread -lm -std=c89

test_layout: test_layout.o graph_layout.o
	$(CC) $(CFLAGS) -o test_layout test_layout.o graph_layout.o graph.o set.o list.o sorting.o stat.o -lm

test_metric: test_metric.o graph_metric.o
	$(CC) $(CFLAGS) -o test_metric test_metric.o graph_metric.o graph.o set.o list.o sorting.o stat.o -lm

test_graph : test_graph.o graph.o
	$(CC) $(CFLAGS) -o test_graph test_graph.o graph.o set.o list.o sorting.o -lm

test_set : test_set.o set.o
	$(CC) $(CFLAGS) -o test_set test_set.o set.o list.o sorting.o -lm 

test_stat : test_stat.o stat.o
	$(CC) $(CFLAGS) -o test_stat test_stat.o stat.o sorting.o -lm

test_list : test_list.o list.o
	$(CC) $(CFLAGS) -o test_list test_list.o list.o sorting.o 

graph_layout.o : graph_layout.h graph.o
graph_metric.o : error.h graph_metric.h graph.o stat.o
stat.o : error.h stat.h sorting.o
graph.o : error.h graph.h set.o
set.o : error.h set.h list.o
list.o : error.h list.h sorting.o
sorting.o : sorting.h
