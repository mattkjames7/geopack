CXXFLAGS=-fopenmp -g -O2 -std=c++17 -Wextra -fPIC -I../../include -Ithird_party/googletest/googletest/include -I../third_party/json/single_include
LDFLAGS=-lm -Wl,-rpath='$$ORIGIN/../../lib' -L../../lib -lgeopack -lgfortran -pthread

CXX=g++ $(CXXFLAGS)
GTEST_DIR=third_party/googletest
GTEST_LIB=$(GTEST_DIR)/build/lib/libgtest.a
GTEST_MAIN_LIB=$(GTEST_DIR)/build/lib/libgtest_main.a
JSON_DIR=../third_party/json

all: test_geopack_golden

$(GTEST_DIR):
	git clone https://github.com/google/googletest.git $(GTEST_DIR)
	cd $(GTEST_DIR) && git checkout v1.14.0

$(GTEST_LIB): $(GTEST_DIR)
	cmake -S $(GTEST_DIR) -B $(GTEST_DIR)/build
	cmake --build $(GTEST_DIR)/build

test_geopack_golden: test_geopack_golden.cc $(GTEST_LIB) $(JSON_DIR)
	$(CXX) -o test_geopack_golden test_geopack_golden.cc $(GTEST_LIB) $(GTEST_MAIN_LIB) $(LDFLAGS)

$(JSON_DIR):
	mkdir -p ../third_party
	git clone https://github.com/nlohmann/json.git $(JSON_DIR)
	cd $(JSON_DIR) && git checkout v3.11.3

run: test_geopack_golden
	./test_geopack_golden

clean:
	-rm -f test_geopack_golden
