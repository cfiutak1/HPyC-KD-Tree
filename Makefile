CFLAGS = -Wall -Wextra -pedantic -std=c++14 -flto -faligned-new -pthread -O3
CDEBUGFLAGS = -g -DDEBUG
CC = g++
VFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all -v


all: main.o KNNQueue.o KDTree.o
	$(CC) $(CFLAGS) main.o KDTree.o KNNQueue.o -o program2

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

KDTree.o: kdtree/KDTree.hpp kdtree/KDTree.cpp
	$(CC) $(CFLAGS) -c kdtree/KDTree.cpp

KNNQueue.o: kdtree/KNNQueue.hpp kdtree/KNNQueue.cpp
	$(CC) $(CFLAGS) -c kdtree/KNNQueue.cpp

KNNSingleQuerySearcher.o: singlequerysearcher/KNNSingleQuerySearcher.cpp singlequerysearcher/KNNSingleQuerySearcher.hpp
	$(CC) $(CFLAGS) -c singlequerysearcher/KNNSingleQuerySearcher.cpp

SingleQueryWorker.o: singlequerysearcher/SingleQueryWorker.cpp singlequerysearcher/SingleQueryWorker.hpp
	$(CC) $(CFLAGS) -c singlequerysearcher/SingleQueryWorker.cpp

run: all
	./program2 1 data/training_10000000_5.dat data/query_100000_5_10.dat results.out

memcheck: all
	valgrind $(VFLAGS) ./program2 1 data/training_1000_3.dat data/query_17231658.dat results_memcheck.out

clean:
	rm *.o program2 results.out

performance_test: all
	for i in 1 2 3 4 5 6 7 8 9 10; do \
  		./program2 4 data/training_10000000_5.dat data/query_100000_5_10.dat results2.out; \
	done