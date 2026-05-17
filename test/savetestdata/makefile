CXXFLAGS=-fopenmp -g -O2 -std=c++17 -Wextra -fPIC -I../../include -I../third_party/json/single_include
LDFLAGS=-lm -Wl,-rpath='$$ORIGIN/../../lib' -L../../lib -lgeopack -lgfortran

CXX=g++ $(CXXFLAGS)
JSON_DIR=../third_party/json

all: savetestdata

savetestdata: savetestdata.cc $(JSON_DIR)
	$(CXX) -o savetestdata savetestdata.cc $(LDFLAGS)

$(JSON_DIR):
	mkdir -p ../third_party
	git clone https://github.com/nlohmann/json.git $(JSON_DIR)
	cd $(JSON_DIR) && git checkout v3.11.3

run: savetestdata
	./savetestdata

clean:
	-rm -f savetestdata
