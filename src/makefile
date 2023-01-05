# CFLAGS for CC
CFLAGS=-lm -g -O3 -std=c++17 -Wextra -fPIC
FFLAGS=-fPIC -fno-automatic -ffree-line-length-none -lm -lstdc++ -lgomp

# Compiler
CCo=g++ -c $(CFLAGS)
CC=g++ $(CFLAGS)
CCWo=x86_64-w64-mingw32-g++-win32 -c $(CFLAGS)
CCW=x86_64-w64-mingw32-g++-win32 $(CFLAGS)

FF=gfortran $(FFLAGS)

all: obj lib

obj:
	cd fortran;	make all
	cd spline; make all
	cd libdatetime; make all
	cd modelparams; make all
	cd tracing; make all
	cd tools; make all
	#cd matrix; make all
	
	$(CCo) modelfield.cc -o $(BUILDDIR)/modelfield.o
	$(CCo) withinmp.cc -o $(BUILDDIR)/withinmp.o
	$(CCo) dummyfunc.cc -o $(BUILDDIR)/dummyfunc.o
	$(CCo) ConvCoords.cc -o $(BUILDDIR)/ConvCoords.o
	$(CCo) getdipoletilt.cc -o $(BUILDDIR)/getdipoletilt.o
	$(CCo) recalc.cc -o $(BUILDDIR)/recalc.o
	
lib:
	gfortran -fPIC -fno-automatic -ffree-line-length-none -shared -o libgeopack.so -lm \
		fortran/*.o \
		spline/*.o \
		libdatetime/*.o \
		modelparams/*.o \
		tools/*.o \
		tracing/*.o \
		*.o -lstdc++ -lgomp
		#matrix/*.o \
		

tests:
	cd test; make all
	

clean:
	cd fortran;	make clean
	cd spline; make clean
	cd libdatetime; make clean
	cd modelparams; make clean
	cd tracing; make clean
	cd tools; make clean
	cd matrix; make clean
	cd test; make clean
	rm *.o
	rm -v  *.so
