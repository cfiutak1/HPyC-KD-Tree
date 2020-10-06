CFLAGS = -Wall -Wextra -pedantic -std=c++14 -flto -faligned-new -pthread -fopenmp -O3 -g
CDEBUGFLAGS = -g -DDEBUG
CC = g++
VFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all -v
NUM_THREADS = 1


all: main.o KDTree.o
	$(CC) $(CFLAGS) main.o KDTree.o -o program2

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

KDTree.o: kdtree/KDTree.hpp kdtree/KDTree.cpp
	$(CC) $(CFLAGS) -c kdtree/KDTree.cpp

ParallelKDTree.o: parallel_kdtree/ParallelKDTree.hpp parallel_kdtree/ParallelKDTree.cpp
	$(CC) $(CFLAGS) -c parallel_kdtree/ParallelKDTree.cpp

#KNNQueue.o: kdtree/KNNQueue.hpp kdtree/KNNQueue.cpp
#	$(CC) $(CFLAGS) -c kdtree/KNNQueue.cpp

KNNSingleQuerySearcher.o: singlequerysearcher/KNNSingleQuerySearcher.cpp singlequerysearcher/KNNSingleQuerySearcher.hpp
	$(CC) $(CFLAGS) -c singlequerysearcher/KNNSingleQuerySearcher.cpp

SingleQueryWorker.o: singlequerysearcher/SingleQueryWorker.cpp singlequerysearcher/SingleQueryWorker.hpp
	$(CC) $(CFLAGS) -c singlequerysearcher/SingleQueryWorker.cpp

run: all
	./program2 $(NUM_THREADS) data/training_10000000_5.dat data/query_100000_5_10.dat results.out

small_test: all
	./program2 $(NUM_THREADS) data/training_1000_3.dat data/query_18211359.dat small_results.out

memcheck: all
	valgrind $(VFLAGS) ./program2 $(NUM_THREADS) data/training_1000_3.dat data/query_18211359.dat results_memcheck.out

clean:
	rm *.o program2

performance_test: all
	for i in 1 2 3 4 5 6 7 8 9 10; do \
  		./program2 1 data/training_10000000_5.dat data/query_100000_5_10.dat results2.out; \
	done