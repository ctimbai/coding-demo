CC := g++
CXXFALGS := -g -Wall -I ../../src/ -std=c++11 -pthread -O2
DIRS := $(filter datetime reactor logging, $(shell ls -l ../../src/ | grep ^d | awk '{print $$9}'))
LIBSRC := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.cc))
HEADERS := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.h))

TARGETS = benchmark

all: $(TARGETS)

benchmark: benchmark.cc $(LIBSRC) $(HEADERS)
	$(CC) $^ -o $@ $(CXXFALGS)

clean:
	rm -rf $(TARGETS)