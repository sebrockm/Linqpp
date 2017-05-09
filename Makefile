all: *.cpp *.hpp
	g++ -std=c++14 test.cpp -o test.out -Wall -pedantic

test: all
	./test.out

clean:
	rm -f *.out *.o
