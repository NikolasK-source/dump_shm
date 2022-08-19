/*
 * Copyright (C) 2021-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "license.hpp"

#include <fcntl.h>
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
        out << "        --license  show license" << std::endl;
        exit(argc != 2 ? EX_USAGE : EX_OK);
    }

    if (std::string(argv[1]) == "-v" || (std::string(argv[1])) == "--version") {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << " (compiled with " << COMPILER_INFO << " on "
                  << SYSTEM_INFO << ')' << std::endl;
        return EX_OK;
    }

    if (std::string(argv[1]) == "--license") {
        print_licenses(std::cout);
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
