CFLAGS = -g -Wall -Wextra -DDEBUG -pedantic -std=c++17 -pthread -Ofast
CC = g++
VFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all -v


all: main.o KNNQueue.o KNNSearcher.o KNNSingleQuerySearcher.o SingleQueryWorker.o KDTree.o
	$(CC) $(CFLAGS) main.o KDTree.o KNNQueue.o KNNSearcher.o KNNSingleQuerySearcher.o SingleQueryWorker.o -o program2

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

KDTree.o: kdtree/KDTree.cpp kdtree/KDTree.hpp
	$(CC) $(CFLAGS) -c kdtree/KDTree.cpp

KNNQueue.o: kdtree/KNNQueue.cpp kdtree/KNNQueue.hpp
	$(CC) $(CFLAGS) -c kdtree/KNNQueue.cpp

KNNSearcher.o: kdtree/KNNSearcher.cpp kdtree/KNNSearcher.hpp
	$(CC) $(CFLAGS) -c kdtree/KNNSearcher.cpp

KNNSingleQuerySearcher.o: singlequerysearcher/KNNSingleQuerySearcher.cpp singlequerysearcher/KNNSingleQuerySearcher.hpp
	$(CC) $(CFLAGS) -c singlequerysearcher/KNNSingleQuerySearcher.cpp

SingleQueryWorker.o: singlequerysearcher/SingleQueryWorker.cpp singlequerysearcher/SingleQueryWorker.hpp
	$(CC) $(CFLAGS) -c singlequerysearcher/SingleQueryWorker.cpp

run: all
	./program2

memcheck: all
	valgrind $(VFLAGS) ./program2 2 data/data_2024712.dat data/query_2024722.dat results.out

clean:
	rm *.o program2
