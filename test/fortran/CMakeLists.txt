add_executable(testfortran testfortran.cc T89c.f)
target_link_libraries(testfortran PRIVATE geopack)
add_test(NAME geopack_test_fortran COMMAND testfortran)
