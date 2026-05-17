CXXFLAGS=-fopenmp -g -O2 -std=c++17 -Wextra -fPIC -I../../include
LDFLAGS=-lm -Wl,-rpath='$$ORIGIN/../../lib' -L../../lib -lgeopack -lgfortran

CXX=g++ $(CXXFLAGS)

all: savetestdata

savetestdata: savetestdata.cc
	$(CXX) -o savetestdata savetestdata.cc $(LDFLAGS)

run: savetestdata
	./savetestdata

clean:
	-rm -f savetestdata
