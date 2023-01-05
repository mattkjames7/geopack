# CFLAGS for CC
CFLAGS=-lm -g -O3 -std=c++17 -Wextra -fPIC
FFLAGS=-fPIC -fno-automatic -ffree-line-length-none -lm -lstdc++ -lgomp

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
else
#linux and mac here
	OS=$(shell uname -s)
	MD=mkdir -p
	ifeq ($(OS),Linux)
		LIBFILE=libgeopack.so
	else
		LIBFILE=libgeopack.dylib
	endif
endif

all: obj lib

obj:
	cd fortran;	make obj
	cd spline; make obj
	cd libdatetime; make obj
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
	gfortran -fPIC -fno-automatic -ffree-line-length-none -shared -o ../lib/$(LIBFILE) -lm \
		$(BUILDDIR)/*.o -lstdc++ -lgomp
		#matrix/*.o \
		

