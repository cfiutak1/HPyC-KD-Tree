CFLAGS = -Wall -Wextra -pedantic -std=c++17 -flto -pthread -fopenmp -O3 -march=native -g
CDEBUGFLAGS = -g -DDEBUG
CC = g++
PYFLAGS = -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -Wall -Wextra -pedantic -std=c++17 -flto -pthread -fopenmp -O3 -march=native -fno-wrapv
VFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all -v
NUM_THREADS = 1


all: main.o
	$(CC) $(PYFLAGS) main.o -o program2

main.o: main.cpp
	$(CC) $(PYFLAGS) -c main.cpp

#KDTree.o: kdtree/KDTree.hpp kdtree/KDTree.cpp
#	$(CC) $(CFLAGS) -c kdtree/KDTree.cpp
#
#ParallelKDTree.o: parallel_kdtree/ParallelKDTree.hpp parallel_kdtree/ParallelKDTree.cpp
#	$(CC) $(CFLAGS) -c parallel_kdtree/ParallelKDTree.cpp
#
#KNNQueue.o: kdtree/KNNQueue.hpp kdtree/KNNQueue.cpp
#	$(CC) $(CFLAGS) -c kdtree/KNNQueue.cpp

run: all
	./program2 $(NUM_THREADS) data/training_10000000_5.dat data/query_100000_5_10.dat results.out

small_test: all
	./program2 $(NUM_THREADS) data/training_1000_3.dat data/query_18211359.dat small_results.out

big_test: all
	./program2 $(NUM_THREADS) data/training_16777216_5.dat data/query_1_5_10.dat big_results.out
	#./program2 8 data/training_16777216_5.dat data/query_1_5_10.dat big_results_multithreaded.out
	#python3 scripts/solver.py data/training_16777216_5.dat data/query_1_5_10.dat sklearn_big_results.out

memcheck: all
	valgrind $(VFLAGS) ./program2 $(NUM_THREADS) data/training_1000_3.dat data/query_18211359.dat results_memcheck.out

clean:
	rm *.o program2 results.out big_results.out sklearn_big_results.out

performance_test: all
	for i in 1 2 3 4 5 6 7 8 9 10; do \
  		./program2 1 data/training_10000000_5.dat data/query_100000_5_10.dat results2.out; \
	done