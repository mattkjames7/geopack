find_package(nlohmann_json CONFIG QUIET)
if(NOT nlohmann_json_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

if(NOT TARGET GTest::gtest)
  find_package(GTest CONFIG QUIET)
endif()
if(NOT TARGET GTest::gtest)
  include(FetchContent)
  set(INSTALL_GTEST OFF CACHE BOOL "Disable googletest install target" FORCE)
  set(gtest_build_tests OFF CACHE BOOL "Disable googletest self-tests" FORCE)
  set(gmock_build_tests OFF CACHE BOOL "Disable googlemock self-tests" FORCE)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  FetchContent_MakeAvailable(googletest)
endif()

add_executable(test_geopack_golden test_geopack_golden.cc)
target_link_libraries(test_geopack_golden PRIVATE geopack nlohmann_json::nlohmann_json GTest::gtest GTest::gtest_main)
add_test(NAME geopack_golden COMMAND test_geopack_golden)
set_tests_properties(geopack_golden PROPERTIES WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/test/unit")
