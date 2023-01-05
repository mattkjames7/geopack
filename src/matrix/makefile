# CFLAGS for CC
CFLAGS=-g -O3 -std=c++17 -Wextra -fPIC
LDFLAGS=-lm 

# Compiler
CCo=g++ -c $(CFLAGS)
CC=g++ $(CFLAGS)

# set the build directory
ifndef BUILDDIR 
	BUILDDIR=.
endif

all: obj lib

obj:
	$(CCo) matrix.cc -o $(BUILDDIR)/matrix.o
	$(CCo) matrixarray.cc -o $(BUILDDIR)/matrixarray.o
	$(CCo) matrixmath.cc -o $(BUILDDIR)/matrixmath.o
	$(CCo) identity.cc -o $(BUILDDIR)/identity.o
	$(CCo) rotmatrix.cc -o $(BUILDDIR)/rotmatrix.o
	$(CCo) libmatrix.cc -o $(BUILDDIR)/libmatrix.o


lib: 
	$(CC) -shared -o libmatrix.so $(BUILDDIR)/*.o $(LDFLAGS)

clean:
	rm -v $(BUILDDIR)/*.o
