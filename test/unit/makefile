CXXFLAGS=-fopenmp -g -O2 -std=c++17 -Wextra -fPIC -I../../include -Ithird_party/googletest/googletest/include
LDFLAGS=-lm -Wl,-rpath='$$ORIGIN/../../lib' -L../../lib -lgeopack -lgfortran -pthread

CXX=g++ $(CXXFLAGS)
GTEST_DIR=third_party/googletest
GTEST_LIB=$(GTEST_DIR)/build/lib/libgtest.a
GTEST_MAIN_LIB=$(GTEST_DIR)/build/lib/libgtest_main.a

all: test_geopack_golden

$(GTEST_DIR):
	git clone https://github.com/google/googletest.git $(GTEST_DIR)
	cd $(GTEST_DIR) && git checkout v1.14.0

$(GTEST_LIB): $(GTEST_DIR)
	cmake -S $(GTEST_DIR) -B $(GTEST_DIR)/build
	cmake --build $(GTEST_DIR)/build

test_geopack_golden: test_geopack_golden.cc minijson.h $(GTEST_LIB)
	$(CXX) -o test_geopack_golden test_geopack_golden.cc $(GTEST_LIB) $(GTEST_MAIN_LIB) $(LDFLAGS)

run: test_geopack_golden
	./test_geopack_golden

clean:
	-rm -f test_geopack_golden
