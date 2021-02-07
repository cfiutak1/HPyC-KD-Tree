CFLAGS = -Wall -Wextra -pedantic -std=c++17 -O3 -march=native
CC = g++
PYFLAGS = -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -Wall -Wextra -pedantic -std=c++17 -pthread -fopenmp -O3 -march=native -fno-wrapv -pthread -Wl,-O3 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro -g -fwrapv -O3 -Wl,-Bsymbolic-functions -Wl,-z,relro -g -fwrapv -O3 -Wall -Wextra -pedantic -std=c++17 -pthread -fopenmp -O3 -march=native -fno-wrapv -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -shared
PYFLAGS2 = -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -Wall -Wextra -pedantic -std=c++17 -pthread -fopenmp -O3 -march=native -fno-wrapv -pthread -Wl,-O3 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro -g -fwrapv -O3 -Wl,-Bsymbolic-functions -Wl,-z,relro -g -fwrapv -O3 -Wall -Wextra -pedantic -std=c++17  -pthread -fopenmp -O3 -march=native -fno-wrapv -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2
VFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all -v

NUM_POINTS = 16777216

TRAINING_FILE = data/training/uniform/$(NUM_POINTS)_5.dat
QUERY_FILE = data/query/uniform/1_5_10.dat

all: main.o
	$(CC) $(CFLAGS) main.o -o program2

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

run: all
	./program2 $(TRAINING_FILE) $(QUERY_FILE) results.out

memcheck: all
	valgrind $(VFLAGS) ./program2 $(TRAINING_FILE) $(QUERY_FILE) results_memcheck.out

cython:
	python3 setup.py build_ext --inplace

clean:
	rm *.o program2
	rm -r build/
	rm hpyc.cpp
	rm hpyc.cpython-38-x86_64-linux-gnu.so
