CFLAGS = -Wall -Wextra -pedantic -std=c++17 -flto -pthread -fopenmp -O3 -march=native -g
CDEBUGFLAGS = -g -DDEBUG
CC = g++
PYFLAGS = -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -Wall -Wextra -pedantic -std=c++17 -flto -pthread -fopenmp -O3 -march=native -fno-wrapv -pthread -Wl,-O3 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro -g -fwrapv -O3 -Wl,-Bsymbolic-functions -Wl,-z,relro -g -fwrapv -O3 -Wall -Wextra -pedantic -std=c++17 -flto -pthread -fopenmp -O3 -march=native -fno-wrapv -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -shared
PYFLAGS2 = -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -Wall -Wextra -pedantic -std=c++17 -flto -pthread -fopenmp -O3 -march=native -fno-wrapv -pthread -Wl,-O3 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro -g -fwrapv -O3 -Wl,-Bsymbolic-functions -Wl,-z,relro -g -fwrapv -O3 -Wall -Wextra -pedantic -std=c++17 -flto -pthread -fopenmp -O3 -march=native -fno-wrapv -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2
VFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all -v
NUM_THREADS = 1

TRAINING_FILE = data/training_16777216_5.dat
QUERY_FILE = data/query_1_5_10.dat

all: main.o
	$(CC) $(PYFLAGS2) main.o -o program2

main.o: main.cpp
	$(CC) $(PYFLAGS) -c main.cpp

run: all
	./program2 $(NUM_THREADS) $(TRAINING_FILE) $(QUERY_FILE) results.out

memcheck: all
	valgrind $(VFLAGS) ./program2 $(NUM_THREADS) $(TRAINING_FILE) $(QUERY_FILE) results_memcheck.out

cython:
	python3 setup.py build_ext --inplace

clean:
	rm *.o program2
	rm -r build/
	rm hpyc.cpp
	rm hpyc.cpython-38-x86_64-linux-gnu.so
