CC = g++-5
CFLAGS = -std=c++14 -Wall -pedantic -O3 -pthread -I..

all: test readme

%.o: %.cpp ../*.hpp
	$(CC) $(CFLAGS) -c $<

test: test.o
	$(CC) $(CFLAGS) $< -o test.out

readme: readme.o
	$(CC) $(CFLAGS) $< -o readme.out

runreadme: readme
	./readme.out

runtest: test
	./test.out

clean:
	rm -f *.out *.o
