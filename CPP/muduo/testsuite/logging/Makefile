CC := g++
CXXFALGS := -g -Wall -I ../../src/ -std=c++11 -pthread -O2
DIRS := $(filter buffer logging, $(shell ls -l ../../src/ | grep ^d | awk '{print $$9}'))
LIBSRC := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.cc)) ../../src/datetime/TimeStamp.cc
HEADERS := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.h)) ../../src/datetime/TimeStamp.h

TARGETS = Logging_test LogFile_test

all: Logging_test

Logging_test: Logging_test.cc $(LIBSRC) $(HEADERS)  
	$(CC) $^ -o $@ $(CXXFALGS)
	
LogFile_test: LogFile_test.cc 
	
    
clean:
	rm -rf $(TARGETS)
	rm -rf *.log