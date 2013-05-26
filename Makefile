CC     = gcc
CFLAGS = -Iinclude -Wall -g
TESTS = stat list set graph graph_metric graph_layout
BIN = experiment

all: $(patsubst %,bin/%, $(BIN)) $(patsubst %,test/test_%, $(TESTS))

doc: doc/doc.pdf

run-tests: $(patsubst %,test/test_%, $(TESTS))
	test/test_list
	test/test_stat
	test/test_set
	test/test_graph
	test/test_metric
	test/test_layout

# Documentation

doc/doc.pdf: doc/doc.tex
	pdflatex -output-directory doc $^

# Binaries

bin/experiment : obj/experiment.o obj/graph_metric.o obj/graph.o obj/set.o obj/list.o obj/sorting.o obj/stat.o
	$(CC) $(CFLAGS) -o $@ $^ -pthread -lm -std=c89

# Test binaries

test/test_graph_layout: obj/test_graph_layout.o obj/graph_layout.o obj/graph.o obj/set.o obj/list.o obj/sorting.o obj/stat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

test/test_graph_metric: obj/test_graph_metric.o obj/graph_metric.o obj/graph.o obj/set.o obj/list.o obj/sorting.o obj/stat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

test/test_graph : obj/test_graph.o obj/graph.o obj/set.o obj/list.o obj/sorting.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

test/test_set : obj/test_set.o obj/set.o obj/list.o obj/sorting.o
	$(CC) $(CFLAGS) -o $@ $^ -lm 

test/test_stat : obj/test_stat.o obj/stat.o obj/sorting.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

test/test_list : obj/test_list.o obj/list.o obj/sorting.o
	$(CC) $(CFLAGS) -o $@ $^ 

## Test objects

obj/test_graph_layout.o : test/test_graph_layout.c include/error.h include/graph_layout.h include/graph.h include/set.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/test_graph_metric.o : test/test_graph_metric.c include/error.h include/graph_metric.h include/graph.h include/set.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/test_graph.o   : test/test_graph.c include/error.h include/graph.h include/set.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/test_set.o     : test/test_set.c include/error.h include/set.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/test_list.o    : test/test_list.c include/error.h include/list.h
	$(CC) $(CFLAGS) -o $@ -c $<

## Basic objets

obj/experiment.o   : src/experiment.c include/graph_metric.h include/graph.h include/set.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/graph_layout.o : src/graph_layout.c include/graph_layout.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/graph_metric.o : src/graph_metric.c include/error.h include/graph_metric.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/stat.o         : src/stat.c include/error.h include/stat.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/graph.o        : src/graph.c include/error.h include/graph.h include/set.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/set.o          : src/set.c include/error.h include/set.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/list.o         : src/list.c include/error.h include/list.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/sorting.o      : src/sorting.c include/sorting.h
	$(CC) $(CFLAGS) -o $@ -c $<
