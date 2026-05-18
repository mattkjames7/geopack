cmake_minimum_required(VERSION 3.21)

project(geopack LANGUAGES C CXX Fortran)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(WIN32)
  set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
else()
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

option(GEOPACK_BUILD_TESTS "Build test executables" ON)
option(GEOPACK_USE_OPENMP "Enable OpenMP when available" ON)
option(GEOPACK_BUILD_SHARED "Build geopack as a shared library" ON)

include(FetchContent)

set(GEOPACK_DATETIME_REPOSITORY "https://github.com/mattkjames7/datetime.git" CACHE STRING "datetime repository")
set(GEOPACK_DATETIME_TAG "v1.1.2" CACHE STRING "datetime git tag/branch")
set(GEOPACK_LIBSPLINE_REPOSITORY "https://github.com/mattkjames7/libspline.git" CACHE STRING "libspline repository")
set(GEOPACK_LIBSPLINE_TAG "v0.3.1" CACHE STRING "libspline git tag/branch")

# Temporarily disable dependency-side tests while fetching external projects.
if(DEFINED CACHE{BUILD_TESTING})
  set(_geopack_had_build_testing_cache ON)
  set(_geopack_saved_build_testing_cache "$CACHE{BUILD_TESTING}")
else()
  set(_geopack_had_build_testing_cache OFF)
endif()
if(DEFINED CACHE{ENABLE_TESTS})
  set(_geopack_had_enable_tests_cache ON)
  set(_geopack_saved_enable_tests_cache "$CACHE{ENABLE_TESTS}")
else()
  set(_geopack_had_enable_tests_cache OFF)
endif()

set(BUILD_TESTING OFF CACHE BOOL "Disable dependency tests while fetching" FORCE)
set(ENABLE_TESTS OFF CACHE BOOL "Disable dependency tests while fetching" FORCE)

FetchContent_Declare(
  datetime
  GIT_REPOSITORY ${GEOPACK_DATETIME_REPOSITORY}
  GIT_TAG ${GEOPACK_DATETIME_TAG}
)

FetchContent_Declare(
  libspline
  GIT_REPOSITORY ${GEOPACK_LIBSPLINE_REPOSITORY}
  GIT_TAG ${GEOPACK_LIBSPLINE_TAG}
)

FetchContent_GetProperties(datetime)
if(NOT datetime_POPULATED)
  FetchContent_Populate(datetime)
  if(WIN32)
    set(_geopack_datetime_cmakelists "${datetime_SOURCE_DIR}/CMakeLists.txt")
    file(READ "${_geopack_datetime_cmakelists}" _geopack_datetime_cmake_text_before)
    set(_geopack_datetime_cmake_text "${_geopack_datetime_cmake_text_before}")
    string(REPLACE
      "set(CMAKE_CXX_FLAGS \"\\${CMAKE_CXX_FLAGS} -fPIC\")"
      "if(NOT WIN32)\n    set(CMAKE_CXX_FLAGS \"\\${CMAKE_CXX_FLAGS} -fPIC\")\nendif()"
      _geopack_datetime_cmake_text
      "${_geopack_datetime_cmake_text}"
    )
    string(REPLACE " -fPIC" "" _geopack_datetime_cmake_text "${_geopack_datetime_cmake_text}")
    string(REPLACE "-fPIC" "" _geopack_datetime_cmake_text "${_geopack_datetime_cmake_text}")
    if(NOT _geopack_datetime_cmake_text STREQUAL _geopack_datetime_cmake_text_before)
      file(WRITE "${_geopack_datetime_cmakelists}" "${_geopack_datetime_cmake_text}")
    endif()
    unset(_geopack_datetime_cmakelists)
    unset(_geopack_datetime_cmake_text)
    unset(_geopack_datetime_cmake_text_before)
  endif()
  add_subdirectory("${datetime_SOURCE_DIR}" "${datetime_BINARY_DIR}")
endif()

FetchContent_MakeAvailable(libspline)

if(WIN32)
  function(geopack_disable_pic_for_target _tgt)
    if(TARGET "${_tgt}")
      get_target_property(_aliased_target "${_tgt}" ALIASED_TARGET)
      if(_aliased_target)
        set_property(TARGET "${_aliased_target}" PROPERTY POSITION_INDEPENDENT_CODE OFF)
      else()
        set_property(TARGET "${_tgt}" PROPERTY POSITION_INDEPENDENT_CODE OFF)
      endif()
    endif()
  endfunction()

  foreach(_dep_tgt
    datetime_shared DateTime::datetime_shared
    datetime_static DateTime::datetime_static
    datetime DateTime::datetime
    libspline_shared spline::spline_shared
    libspline_static spline::spline_static
    libspline spline::spline
  )
    geopack_disable_pic_for_target("${_dep_tgt}")
  endforeach()
endif()

if(_geopack_had_build_testing_cache)
  set(BUILD_TESTING "${_geopack_saved_build_testing_cache}" CACHE BOOL "Restored BUILD_TESTING value" FORCE)
else()
  unset(BUILD_TESTING CACHE)
endif()
if(_geopack_had_enable_tests_cache)
  set(ENABLE_TESTS "${_geopack_saved_enable_tests_cache}" CACHE BOOL "Restored ENABLE_TESTS value" FORCE)
else()
  unset(ENABLE_TESTS CACHE)
endif()

unset(_geopack_had_build_testing_cache)
unset(_geopack_saved_build_testing_cache)
unset(_geopack_had_enable_tests_cache)
unset(_geopack_saved_enable_tests_cache)

function(geopack_pick_target out_var)
  foreach(_cand IN LISTS ARGN)
    if(TARGET "${_cand}")
      set(${out_var} "${_cand}" PARENT_SCOPE)
      return()
    endif()
  endforeach()
  set(${out_var} "" PARENT_SCOPE)
endfunction()

if(GEOPACK_BUILD_SHARED)
  geopack_pick_target(GEOPACK_DATETIME_TARGET
    datetime_shared
    DateTime::datetime_shared
    datetime
    DateTime::datetime
  )
  geopack_pick_target(GEOPACK_LIBSPLINE_TARGET
    libspline_shared
    spline::spline_shared
    libspline
    spline::spline
  )
else()
  geopack_pick_target(GEOPACK_DATETIME_TARGET
    datetime_static
    DateTime::datetime_static
    datetime
    DateTime::datetime
  )
  geopack_pick_target(GEOPACK_LIBSPLINE_TARGET
    libspline_static
    spline::spline_static
    libspline
    spline::spline
  )
endif()

if(GEOPACK_DATETIME_TARGET STREQUAL "")
  if(GEOPACK_BUILD_SHARED)
    message(FATAL_ERROR "Could not resolve a shared datetime target from fetched dependency.")
  else()
    message(FATAL_ERROR "Could not resolve a static datetime target from fetched dependency.")
  endif()
endif()
if(GEOPACK_LIBSPLINE_TARGET STREQUAL "")
  if(GEOPACK_BUILD_SHARED)
    message(FATAL_ERROR "Could not resolve a shared libspline target from fetched dependency.")
  else()
    message(FATAL_ERROR "Could not resolve a static libspline target from fetched dependency.")
  endif()
endif()

if(GEOPACK_USE_OPENMP)
  find_package(OpenMP)
endif()

add_subdirectory(src)

if(GEOPACK_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
