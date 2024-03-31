# Dump SHM

This application dumps the content of a shared memory object to stdout.

## Build dependencies

### Ubuntu/Debian
```
apt update; apt install clang cmake build-essential
```

## Build
```
cmake -B build . -DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_BUILD_TYPE=Release -DCLANG_FORMAT=OFF -DCOMPILER_WARNINGS=OFF
cmake --build build 
```

## Use
```
dump_shm <shm_name>
```

