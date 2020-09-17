CFLAGS = -Wall -Wextra -pedantic -std=c++14 -O3 -flto -faligned-new
CDEBUGFLAGS = -g -DDEBUG
CC = g++
VFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all -v


all: main.o KNNQueue.o KDTree.o
	$(CC) $(CFLAGS) main.o KDTree.o KNNQueue.o -o program2

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

KDTree.o: kdtree kdtree
	$(CC) $(CFLAGS) -c kdtree/KDTree.cpp

KNNQueue.o: kdtree kdtree
	$(CC) $(CFLAGS) -c kdtree/KNNQueue.cpp

KNNSearcher.o: kdtree kdtree
	$(CC) $(CFLAGS) -c kdtree/KNNSearcher.cpp

KNNSingleQuerySearcher.o: singlequerysearcher/KNNSingleQuerySearcher.cpp singlequerysearcher/KNNSingleQuerySearcher.hpp
	$(CC) $(CFLAGS) -c singlequerysearcher/KNNSingleQuerySearcher.cpp

SingleQueryWorker.o: singlequerysearcher/SingleQueryWorker.cpp singlequerysearcher/SingleQueryWorker.hpp
	$(CC) $(CFLAGS) -c singlequerysearcher/SingleQueryWorker.cpp

run: all
	./program2 1 data/training_10000000_5.dat data/query_100000_5_10.dat results.out

memcheck: all
	valgrind $(VFLAGS) ./program2 4 data/training_300_5.dat data/query_1_5_3.dat results.out

clean:
	rm *.o program2 results.out


performance_test: all
	for i in 1 2 3 4 5 6 7 8 9 10; do \
  		./program2 4 data/training_10000000_5.dat data/query_100000_5_10.dat results2.out; \
	done