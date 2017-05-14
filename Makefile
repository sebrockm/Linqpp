all: *.cpp *.hpp
	g++-5 -std=c++1y test.cpp -o test.out -Wall -pedantic -O3 -pthread

test: all
	./test.out

clean:
	rm -f *.out *.o
