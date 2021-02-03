CC := g++
CXXFALGS := -g -Wall -I ../../src/ -std=c++11 -pthread

DIRS := $(shell ls -l ../../src/ | grep ^d | awk '{print $$9}')
LIBSRC := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.cc))
HEADERS := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.h))
CLIBSRC := ../../src/http/http_parser.c ../../src/kcp/ikcp.c

TARGETS := client server

all: $(TARGETS)

client: client.cc HimClient.cc $(LIBSRC) $(CLIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

server: server.cc HimServer.cc $(LIBSRC) $(CLIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)


clean:
	rm $(TARGETS)
