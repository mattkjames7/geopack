# CFLAGS for CC
CFLAGS=-g -O3 -std=c++17 -Wextra -fPIC
FFLAGS=-fPIC -fno-automatic -ffree-line-length-none
LDFLAGS=-lm -lstdc++ -L../lib -ldatetime -L../lib -lspline

# Compiler
CCo=g++ -c $(CFLAGS)
CC=g++ $(CFLAGS)
CCWo=x86_64-w64-mingw32-g++-win32 -c $(CFLAGS)
CCW=x86_64-w64-mingw32-g++-win32 $(CFLAGS)

FF=gfortran $(FFLAGS)

ifndef BUILDDIR 
	BUILDDIR=../build
endif

ifeq ($(OS),Windows_NT)
#windows stuff here
	MD=mkdir
	LIBFILE=libgeopack.dll
	CCo=g++ -c $(CFLAGS) -fopenmp $(IFLAGS)
	CC=g++ $(CFLAGS) -fopenmp $(IFLAGS)
else
#linux and mac here
	OS=$(shell uname -s)
	MD=mkdir -p
	ifeq ($(OS),Linux)
		CCo=g++ -c $(CFLAGS) -fopenmp $(IFLAGS)
		CC=g++ $(CFLAGS) -fopenmp $(IFLAGS)
		LIBFILE=libgeopack.so
	else
		LDFLAGS=-lm -lstdc++ -L../lib -ldatetime -L../lib -lspline "-L/usr/local/opt/libomp/lib" -lomp
		CPPFLAGS="-I/usr/local/opt/libomp/include"
		CCo=g++ -Xpreprocessor -fopenmp -c $(CFLAGS) $(CPPFLAGS) $(IFLAGS)
		CC=g++ -Xpreprocessor -fopenmp $(CFLAGS) $(CPPFLAGS) $(IFLAGS)
		LIBFILE=libgeopack.dylib
	endif
endif



all: obj lib

obj:
	cd fortran;	make obj
	cd modelparams; make obj
	cd tracing; make obj
	cd tools; make obj
	cd matrix; make obj
	
	$(CCo) modelfield.cc -o $(BUILDDIR)/modelfield.o
	$(CCo) withinmp.cc -o $(BUILDDIR)/withinmp.o
	$(CCo) dummyfunc.cc -o $(BUILDDIR)/dummyfunc.o
	$(CCo) ConvCoords.cc -o $(BUILDDIR)/ConvCoords.o
	$(CCo) getdipoletilt.cc -o $(BUILDDIR)/getdipoletilt.o
	$(CCo) recalc.cc -o $(BUILDDIR)/recalc.o
	
lib:
	$(FF) -shared -o ../lib/$(LIBFILE) $(BUILDDIR)/*.o $(LDFLAGS)
		

