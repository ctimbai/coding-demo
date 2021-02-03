CC := g++
CXXFALGS := -g -Wall -I ../../src/ -std=c++11 -pthread

DIRS := $(filter-out thread kcp, $(shell ls -l ../../src/ | grep ^d | awk '{print $$9}'))
LIBSRC := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.cc)) ../../src/http/http_parser.c
HEADERS := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.h))

TARGETS = HttpServer_test

all: $(TARGETS)

HttpServer_test: HttpServer_test.cc $(LIBSRC) $(HEADERS)
	@$(CC) $^ -o $@ $(CXXFALGS)

clean:
	rm -rf $(TARGETS)