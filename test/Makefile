NPROCS = $(shell grep -c 'processor' /proc/cpuinfo)
MAKEFLAGS += -j$(NPROCS)

CC = g++-5
CFLAGS = -std=c++14 -Wall -pedantic -Wfatal-errors -O3 -pthread -I..

TEST_SRC = $(wildcard *test.cpp)
TEST_OBJ = $(patsubst %.cpp, %.o, $(TEST_SRC))
HEADERS = $(wildcard ../*.hpp ../Iterator/*.hpp)

all: unit_tests readme

$(TEST_OBJ):%.o:%.cpp $(HEADERS) test_template.hpp
	$(CC) $(CFLAGS) -c $<

catch.o: catch.cpp catch.hpp
	$(CC) $(CFLAGS) -c $<

unit_tests.out: $(TEST_OBJ) catch.o 
	$(CC) $(CFLAGS) $^ -o $@

readme.out: readme.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

unit_tests: unit_tests.out

readme: readme.out

clean:
	rm -f *.o readme.out unit_tests.out

.PHONY: all unit_tests readme clean
