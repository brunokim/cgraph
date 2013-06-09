CC     = gcc
CFLAGS = -Iinclude -Wall -g

MODULES = sorting stat list set graph graph_metric graph_layout graph_model graph_propagation
TESTS = $(patsubst %, test/test_%, $(MODULES))

DATASETS = cat mac95 email powergrid astrophysics internet netscience
FOLDERS = $(patsubst %, datasets/%, $(DATASETS))

BIN = metrics propagation

.PHONY: all doc run-metrics run-tests clean-binaries clean-test clean

all: $(patsubst %,bin/%, $(BIN)) $(TESTS)

doc: doc/main.pdf 

run-metrics: bin/metrics
	bin/metrics $(FOLDERS)
	
run-tests: $(TESTS)
	for test in $(TESTS); do echo $$test && ./$$test; done

clean-binaries:
	rm obj/*
	rm $(TESTS)
	rm bin/*

clean-test:
	rm test/*.svg
	rm test/*.dat
	for dir in test/*/; do rm $${dir}*; done

clean: clean-binaries clean-test

# Documentation

doc/main.pdf: doc/main.tex $(patsubst %,doc/%.tex, $(MODULES))
	pdflatex -output-directory doc $<
	pdflatex -output-directory doc $<

# Binaries

bin/metrics : obj/metrics.o obj/graph_metric.o obj/graph.o obj/set.o obj/list.o obj/sorting.o obj/stat.o
	$(CC) $(CFLAGS) -o $@ $^ -pthread -lm -std=c89

bin/propagation : obj/propagation.o obj/graph_propagation.o obj/graph_layout.o obj/graph_model.o obj/graph.o obj/set.o obj/list.o obj/sorting.o obj/stat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm -std=c89

# Test binaries

test/test_graph_propagation: obj/test_graph_propagation.o obj/graph_propagation.o \
 obj/graph_layout.o obj/graph_model.o obj/graph.o obj/set.o obj/list.o obj/sorting.o obj/stat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

test/test_graph_model: obj/test_graph_model.o obj/graph_model.o obj/graph.o obj/set.o obj/list.o obj/sorting.o obj/stat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

test/test_graph_layout: obj/test_graph_layout.o obj/graph_model.o obj/graph_layout.o obj/graph.o obj/set.o obj/list.o obj/sorting.o obj/stat.o
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

test/test_sorting : obj/test_sorting.o obj/sorting.o
	$(CC) $(CFLAGS) -o $@ $^

## Test objects

obj/test_graph_propagation.o : test/test_graph_propagation.c include/error.h include/graph_propagation.h include/graph.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/test_graph_model.o : test/test_graph_model.c include/error.h include/graph_model.h include/graph.h
	$(CC) $(CFLAGS) -o $@ -c $<

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

obj/test_stat.o    : test/test_stat.c include/error.h include/stat.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/test_sorting.o : test/test_sorting.c include/sorting.h
	$(CC) $(CFLAGS) -o $@ -c $<

## Basic objets

obj/propagation.o : src/propagation.c include/graph_propagation.h include/graph.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/metrics.o   : src/metrics.c include/graph_metric.h include/graph.h include/set.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/graph_propagation.o : src/graph_propagation.c include/graph_propagation.h include/graph.h
	$(CC) $(CFLAGS) -o $@ -c $<

obj/graph_model.o : src/graph_model.c include/graph_model.h include/graph.h
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
