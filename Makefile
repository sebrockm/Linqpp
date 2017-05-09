all: *.cpp *.hpp
	g++ -std=c++14 test.cpp -o test.out -Wall -pedantic -O3

test: all
	./test.out

clean:
	rm -f *.out *.o
