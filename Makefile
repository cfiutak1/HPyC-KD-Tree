CFLAGS = -Wall -Wextra -pedantic -std=c++17 -pthread -Ofast
CDEBUGFLAGS = -g -DDEBUG
CC = g++
VFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all -v


all: main.o KNNQueue.o KDTree.o
	$(CC) $(CFLAGS) main.o KDTree.o KNNQueue.o -o program2

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

KDTree.o: kdtree/KDTree.cpp kdtree/KDTree.hpp
	$(CC) $(CFLAGS) -c kdtree/KDTree.cpp

array_KDTree.o: array_kdtree/KDTree.cpp array_kdtree/KDTree.hpp
	$(CC) $(CFLAGS) -c array_kdtree/KDTree.cpp -o array_KDTree.o

KNNQueue.o: kdtree/KNNQueue.cpp kdtree/KNNQueue.hpp
	$(CC) $(CFLAGS) -c kdtree/KNNQueue.cpp

array_KNNQueue.o: array_kdtree/KNNQueue.cpp array_kdtree/KNNQueue.hpp
	$(CC) $(CFLAGS) -c array_kdtree/KNNQueue.cpp -o array_KNNQueue.o

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
