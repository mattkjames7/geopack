# geopack
Geopack library

## Build (CMake)

```bash
cmake -S . -B build-cmake
cmake --build build-cmake -j
ctest --test-dir build-cmake --output-on-failure
```

### Common options

- `-DGEOPACK_BUILD_SHARED=ON|OFF`  
  Build `geopack` as shared (`ON`, default) or static (`OFF`).
- `-DGEOPACK_BUILD_TESTS=ON|OFF`  
  Enable/disable geopack test targets.
- `-DGEOPACK_USE_OPENMP=ON|OFF`  
  Enable/disable OpenMP linking when available.

Dependencies (`datetime`, `libspline`) are fetched with CMake and configured so their own tests are not built.

## API

The C API is exposed as global `extern "C"` functions through `geopack.h`.
C++ classes and helper functions are in the `geopack::` namespace, for example `geopack::Trace`.

## Install

```bash
cmake -S . -B build-cmake -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build-cmake -j
cmake --install build-cmake
```

Installed artifacts:

- `${CMAKE_INSTALL_PREFIX}/lib` : `libgeopack` (`.so`/`.a` depending on build mode)
- `${CMAKE_INSTALL_PREFIX}/include` : `geopack.h`
