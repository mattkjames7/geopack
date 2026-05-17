add_subdirectory(fortran)
add_subdirectory(matrix)
add_subdirectory(modelparams)
add_subdirectory(tools)
add_subdirectory(tracing)

set(_geopack_lib_type STATIC)
if(GEOPACK_BUILD_SHARED)
  set(_geopack_lib_type SHARED)
endif()

add_library(geopack ${_geopack_lib_type}
  modelfield.cc
  withinmp.cc
  dummyfunc.cc
  ConvCoords.cc
  getdipoletilt.cc
  recalc.cc
  ${GEOPACK_FORTRAN_SOURCES}
  ${GEOPACK_MATRIX_SOURCES}
  ${GEOPACK_MODELPARAM_SOURCES}
  ${GEOPACK_TOOL_SOURCES}
  ${GEOPACK_TRACING_SOURCES}
)
unset(_geopack_lib_type)

set_source_files_properties(
  fortran/T89c.f
  fortran/T96.f
  fortran/T01_01.f
  fortran/TS04c.f
  fortran/Geopack-2008_mkj_dp.f
  PROPERTIES Fortran_FORMAT FIXED
)

target_compile_options(geopack PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-O3 -Wextra>
  $<$<COMPILE_LANGUAGE:Fortran>:-fno-automatic -ffree-line-length-none>
)

target_include_directories(geopack
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${datetime_SOURCE_DIR}/include
    ${libspline_SOURCE_DIR}/include
)

target_link_libraries(geopack
  PRIVATE
    ${GEOPACK_DATETIME_TARGET}
    ${GEOPACK_LIBSPLINE_TARGET}
)

if(OpenMP_CXX_FOUND)
  target_link_libraries(geopack PRIVATE OpenMP::OpenMP_CXX)
endif()
if(OpenMP_Fortran_FOUND)
  target_link_libraries(geopack PRIVATE OpenMP::OpenMP_Fortran)
endif()

install(TARGETS geopack)
install(FILES ${PROJECT_SOURCE_DIR}/include/geopack.h DESTINATION include)
