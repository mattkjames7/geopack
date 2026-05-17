add_subdirectory(fortran)
add_subdirectory(savetestdata)
add_subdirectory(unit)

add_executable(testc test.c)
target_link_libraries(testc PRIVATE geopack)

add_executable(testcc test.cc)
target_link_libraries(testcc PRIVATE geopack)

add_test(NAME geopack_test_c COMMAND testc)
add_test(NAME geopack_test_cc COMMAND testcc)
