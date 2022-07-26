#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2 || std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
        auto &out = argc != 2 ? std::cerr : std::cout;
        out << "Dump the content of a shared memory to stdout" << std::endl;
        out << "usage: " << std::endl;
        out << "    dump_shm [OPTION...] <shm_name>" << std::endl;
        out << std::endl;
        out << "    -h  --help     print this message" << std::endl;
        out << "    -v  --version  print version" << std::endl;
        out << std::endl;
        out << std::endl;
        out << "MIT License:" << std::endl;
        out << std::endl;
        out << "Copyright (c) 2021 Nikolas Koesling" << std::endl;
        out << std::endl;
        out << "Permission is hereby granted, free of charge, to any person obtaining a copy" << std::endl;
        out << "of this software and associated documentation files (the \"Software\"), to deal" << std::endl;
        out << "in the Software without restriction, including without limitation the rights" << std::endl;
        out << "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell" << std::endl;
        out << "copies of the Software, and to permit persons to whom the Software is" << std::endl;
        out << "furnished to do so, subject to the following conditions:" << std::endl;
        out << std::endl;
        out << "The above copyright notice and this permission notice shall be included in all" << std::endl;
        out << "copies or substantial portions of the Software." << std::endl;
        out << std::endl;
        out << "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR" << std::endl;
        out << "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY," << std::endl;
        out << "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE" << std::endl;
        out << "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" << std::endl;
        out << "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM," << std::endl;
        out << "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE" << std::endl;
        out << "SOFTWARE." << std::endl;
        exit(argc != 2 ? EX_USAGE : EX_OK);
    }

    if (std::string(argv[1]) == "-v" || (std::string(argv[1])) == "--version") {
        std::cout << "dump_shm " << PROJECT_VERSION << std::endl;
        exit(EX_OK);
    }

    const std::string name = argv[1];

    // open shared memory
    int fd = shm_open(name.c_str(), O_RDWR, 0660);
    if (fd < 0) {
        perror("shm_open");
        exit(EX_OSERR);
    }

    // get size of shared memory object
    struct stat shm_stats {};
    if (fstat(fd, &shm_stats)) {
        if (close(fd)) { perror("close"); }
        perror("fstat");
        exit(EX_OSERR);
    }
    const auto size = static_cast<std::size_t>(shm_stats.st_size);

    // map shared memory
    auto data = reinterpret_cast<uint8_t *>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
    if (data == MAP_FAILED || data == nullptr) {
        if (close(fd)) { perror("close"); }
        perror("mmap");
        exit(EX_OSERR);
    }

    // output data
    std::cout.write(reinterpret_cast<const char *>(data), static_cast<std::streamsize>(size));
    std::cout.flush();

    // unmap and close shared memory object
    if (munmap(data, size)) { perror("munmap"); }
    if (close(fd)) { perror("close"); }
}
