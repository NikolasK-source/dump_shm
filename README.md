# Dump SHM

This application dumps the content of a shared memory object to stdout.

## Build
```
apt update; apt install clang cmake build-essential
git submodule init
git submodule update
mkdir build
cd build
cmake .. -DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_BUILD_TYPE=Release -DCLANG_FORMAT=OFF -DCOMPILER_WARNINGS=OFF
cmake --build . 
```

## Use
```
dump_shm <shm_name>
```

