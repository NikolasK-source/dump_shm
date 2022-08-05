# Shared Memory Dump

This tool outputs the content of a shared memory to stdout.

## Use the Application
To use the application, simply call it with the name of the shared memory to be output as an argument.

To display the content you can for example use ```hexdump```:
```
dump-shm | hexdump -C -v
```

## Using the Flatpak package
The flatpak package can be installed via the .flatpak file.
This can be downloaded from the GitHub [projects release page](https://github.com/NikolasK-source/dump-shm/releases):

```
flatpak install --user dump-shm.flatpak
```

The application is then executed as follows:
```
flatpak run network.koesling.dump-shm
```

To enable calling with ```modbus-rtu-client-shm``` [this script](https://gist.github.com/NikolasK-source/6f323942544d5de39323771dceed4023) can be used.
In order to be executable everywhere, the path in which the script is placed must be in the ```PATH``` environment variable.


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


## Links to related projects

### General Shared Memory Tools
- [Shared Memory Dump](https://nikolask-source.github.io/dump_shm/)
- [Shared Memory Write](https://nikolask-source.github.io/write_shm/)
- [Shared Memory Random](https://nikolask-source.github.io/shared_mem_random/)

### Modbus Clients
- [RTU](https://nikolask-source.github.io/modbus_rtu_client_shm/)
- [TCP](https://nikolask-source.github.io/modbus_tcp_client_shm/)

### Modbus Shared Memory Tools
- [STDIN to Modbus](https://nikolask-source.github.io/stdin_to_modbus_shm/)
- [Float converter](https://nikolask-source.github.io/modbus_conv_float/)


## License

MIT License

Copyright (c) 2021-2022 Nikolas Koesling

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
