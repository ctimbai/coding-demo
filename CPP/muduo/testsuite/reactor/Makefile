CC := g++
CXXFALGS := -g -Wall -I ../../src/ -std=c++11 -pthread

DIRS := $(filter-out http, $(shell ls -l ../../src/ | grep ^d | awk '{print $$9}'))
LIBSRC := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.cc))
HEADERS := $(foreach dir, $(DIRS), $(wildcard ../../src/$(dir)/*.h))

TARGETS := test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 \
            test_client UdpServer_test UdpClient_test

all: $(TARGETS)

test1: test1.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test2: test2.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test3: test3.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test4: test4.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test5: test5.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test6: test6.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test7: test7.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test8: test8.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test9: test9.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test10: test10.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

test_client: test_client.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

UdpServer_test: UdpServer_test.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

UdpClient_test: UdpClient_test.cc $(LIBSRC)
	$(CC) $^ -o $@ $(CXXFALGS)

clean:
	rm $(TARGETS)
