# CFLAGS for CC
CFLAGS=-g -O3 -std=c++17 -Wextra -fPIC
LDFLAGS=-lm


# Compiler
CCo=g++ -c $(CFLAGS)
CC=g++ $(CFLAGS)
CCWo=x86_64-w64-mingw32-g++-win32 -c $(CFLAGS)
CCW=x86_64-w64-mingw32-g++-win32 $(CFLAGS)

# list of files
SRC=$(wildcard *.cc)
OBJ=$(SRC:.cc=.o)

# set the build directory
ifndef BUILDDIR 
	BUILDDIR=.
endif

all: obj lib

%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $(BUILDDIR)/$@

obj: $(OBJ)

lib: 
	$(CC) -shared -o libdatetime.so $(BUILDDIR)/*.o $(LDFLAGS)

clean:
	rm -v *.o *.so
