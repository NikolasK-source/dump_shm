# Shared Memory Dump

This tool outputs the content of a shared memory to stdout.

## Use the Application
To use the application, simply call it with the name of the shared memory to be output as an argument.

## Build from Source

The following packages are required for building the application:
- cmake
- clang or gcc

Use the following commands to build the application:
```
git clone https://github.com/NikolasK-source/dump_shm.git
cd dump_shm
mkdir build
cmake -B build . -DCMAKE_BUILD_TYPE=Release -DCLANG_FORMAT=OFF -DCOMPILER_WARNINGS=OFF
cmake --build build
```

The binary is located in the build directory.
