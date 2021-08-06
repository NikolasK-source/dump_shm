# Dump SHM

This application dumps the content of a shared memory object to stdout.

## Build
```
mkdir build
cd build
cmake .. -DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_BUILD_TYPE=Release -DCLANG_FORMAT=OFF
cmake --build . 
```

## Use
```
dump_shm <shm_name>
```

