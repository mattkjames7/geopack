cmake_minimum_required(VERSION 3.21)

project(geopack LANGUAGES C CXX Fortran)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(GEOPACK_BUILD_TESTS "Build test executables" ON)
option(GEOPACK_USE_OPENMP "Enable OpenMP when available" ON)
option(GEOPACK_BUILD_SHARED "Build geopack as a shared library" ON)

include(FetchContent)

set(GEOPACK_DATETIME_REPOSITORY "https://github.com/mattkjames7/datetime.git" CACHE STRING "datetime repository")
set(GEOPACK_DATETIME_TAG "v1.1.2" CACHE STRING "datetime git tag/branch")
set(GEOPACK_LIBSPLINE_REPOSITORY "https://github.com/mattkjames7/libspline.git" CACHE STRING "libspline repository")
set(GEOPACK_LIBSPLINE_TAG "v0.3.1" CACHE STRING "libspline git tag/branch")

# Temporarily disable dependency-side tests while fetching external projects.
set(_geopack_saved_build_testing "${BUILD_TESTING}")
set(_geopack_saved_enable_tests "${ENABLE_TESTS}")
set(BUILD_TESTING OFF)
set(ENABLE_TESTS OFF)

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

FetchContent_MakeAvailable(datetime libspline)

set(BUILD_TESTING "${_geopack_saved_build_testing}")
set(ENABLE_TESTS "${_geopack_saved_enable_tests}")
unset(_geopack_saved_build_testing)
unset(_geopack_saved_enable_tests)

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
