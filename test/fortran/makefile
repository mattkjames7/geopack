# CFLAGS for CC
CFLAGS=-g -O3 -Wextra -fPIC -I../../include
CXXFLAGS=-g -O3 -std=c++17 -Wextra -fPIC -I../../include
FCFLAGS=-w -c -fPIC -fno-automatic -std=legacy -ffree-line-length-none -O3


OS=$(shell uname -s)
ifeq ($(OS),Linux)
	LDFLAGS=-lm -Wl,-rpath='$$ORIGIN/../../lib' -L ../../lib -lgeopack -lgfortran
else
	LDFLAGS=-lm -L../lib -lgeopack -lgfortran
endif


# Compiler
CC=gcc $(CFLAGS)
CXX=g++ $(CXXFLAGS)
FF=gfortran $(FCFLAGS)

all:
	$(FF) -o t89c.o T89c.f 
	$(CXX) -o testfortran testfortran.cc $(LDFLAGS)
	./testfortran
	-rm -v t89c.o
	-rm -v testfortran
