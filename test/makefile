# CFLAGS for CC
CFLAGS=-g -O3 -Wextra -fPIC -I../include -fopenmp
CXXFLAGS=-g -O3 -std=c++17 -Wextra -fPIC -I../include -fopenmp

OS=$(shell uname -s)
ifeq ($(OS),Linux)
	LDFLAGS=-lm -Wl,-rpath='$$ORIGIN/../lib' -L ../lib -lgeopack -lgfortran
else
	LDFLAGS=-lm -L../lib -lgeopack -lgfortran
endif


# Compiler
CC=gcc $(CFLAGS)
CXX=g++ $(CXXFLAGS)

all:
	$(CC) -o testc test.c $(LDFLAGS)
	$(CXX) -o testcc test.cc $(LDFLAGS)
	LD_LIBRARY_PATH=../lib ./testc
	LD_LIBRARY_PATH=../lib ./testcc
	cd fortran; make all

clean:
	-rm -v testc
	-rm -v testcc