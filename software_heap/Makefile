CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

all: main

main: main.o
	$(CXX) $(CXXFLAGS) -o main main.o

main.o: main.cc heap.h
	$(CXX) $(CXXFLAGS) -c main.cc

clean:
	rm -f main main.o