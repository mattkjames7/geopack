# CFLAGS for CC
CFLAGS=-g -O3 -Wextra -fPIC -I../include -fopenmp
CXXFLAGS=-g -O3 -std=c++17 -Wextra -fPIC -I../include -fopenmp

OS=$(shell uname -s)
ifeq ($(OS),Linux)
	CC=gcc $(CFLAGS)
	CXX=g++ $(CXXFLAGS)
	LDFLAGS=-lm -Wl,-rpath='$$ORIGIN/../lib' -L ../lib -lgeopack -lgfortran
else
	CC=gcc -Xpreprocessor $(CFLAGS)
	CXX=g++ -Xpreprocessor $(CXXFLAGS)
	LDFLAGS=-lm -L../lib -lgeopack -lgfortran "-L/usr/local/opt/libomp/lib" -lomp
endif


# Compiler


all:
	$(CC) -o testc test.c $(LDFLAGS)
	$(CXX) -o testcc test.cc $(LDFLAGS)
	LD_LIBRARY_PATH=../lib ./testc
	LD_LIBRARY_PATH=../lib ./testcc
	cd fortran; make all

clean:
	-rm -v testc
	-rm -v testcc