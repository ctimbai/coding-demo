CC := g++
CXXFALGS := -g -Wall -I ../../src/ -std=c++11 -pthread

DIRS := $(filter-out http, $(shell ls -l ../../src/ | grep ^d | awk '{print $$9}'))
LIBSRC := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.cc)) ../../src/kcp/ikcp.c
HEADERS := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.h))

TARGETS := KcpServer_test KcpClient_test

all: $(TARGETS)

KcpServer_test: KcpServer_test.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

KcpClient_test: KcpClient_test.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

clean:
	rm $(TARGETS)
