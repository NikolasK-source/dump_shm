# Dump SHM

This application dumps the content of a shared memory object to stdout.

## Dependencies
- cxxopts by jarro2783 (https://github.com/jarro2783/cxxopts) (only required for building the application)
- cxxshm (https://github.com/NikolasK-source/cxxshm)
- cxxsemaphore (https://github.com/NikolasK-source/cxxsemaphore)

On Arch linux they are available via the official repositories and the AUR:
- https://archlinux.org/packages/extra/any/cxxopts/
- https://aur.archlinux.org/packages/cxxshm
- https://aur.archlinux.org/packages/cxxsemaphore

## Build
```
cmake -B build . -DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_BUILD_TYPE=Release -DCLANG_FORMAT=OFF -DCOMPILER_WARNINGS=OFF
cmake --build build 
```

## Use
```
dump_shm <shm_name>
```
