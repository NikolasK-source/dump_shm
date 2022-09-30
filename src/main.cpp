/*
 * Copyright (C) 2021-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "license.hpp"

#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <unistd.h>

// cxxopts, but all warnings disabled
#ifdef COMPILER_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Weverything"
#elif defined(COMPILER_GCC)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wall"
#endif

#include <cxxopts.hpp>

#ifdef COMPILER_CLANG
#    pragma clang diagnostic pop
#elif defined(COMPILER_GCC)
#    pragma GCC diagnostic pop
#endif

int main(int argc, char **argv) {
    const std::string exe_name = std::filesystem::path(argv[0]).filename().string();
    cxxopts::Options  options(exe_name, "Dump the content of a shared memory to stdout");

    options.add_options()("b,bytes", "limit number of bytes to output", cxxopts::value<std::size_t>());
    options.add_options()(
            "o,offset", "do not output the leading arg bytes", cxxopts::value<std::size_t>()->default_value("0"));
    options.add_options()("h,help", "print usage");
    options.add_options()("version", "print version information");
    options.add_options()("license", "show licences");

    options.add_options()("shmname", "name of the shared memory to dump", cxxopts::value<std::string>());
    options.parse_positional({"shmname"});
    options.positional_help("SHM_NAME");

    cxxopts::ParseResult opts;
    try {
        opts = options.parse(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << "Failed to parse arguments: " << e.what() << std::endl;
        std::cerr << "Use --help for more information." << std::endl;
        return EX_USAGE;
    }

    if (opts.count("help")) {
        options.set_width(120);
        std::cout << options.help() << std::endl;
        std::cout << std::endl;
        std::cout << "This application uses the following libraries:" << std::endl;
        std::cout << "  - cxxopts by jarro2783 (https://github.com/jarro2783/cxxopts)" << std::endl;
        return EX_OK;
    }

    if (opts.count("version")) {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << " (compiled with " << COMPILER_INFO << " on "
                  << SYSTEM_INFO << ')' << std::endl;
        return EX_OK;
    }

    if (opts.count("license")) {
        print_licenses(std::cout);
        return EX_OK;
    }

    if (!opts.count("shmname")) {
        std::cerr << "Shared memory name is mandatory." << std::endl;
        std::cerr << "Use --help for more information." << std::endl;
        return EX_USAGE;
    }

    const auto name = opts["shmname"].as<std::string>();

    // open shared memory
    int fd = shm_open(name.c_str(), O_RDWR, 0660);
    if (fd < 0) {
        perror("shm_open");
        return EX_OSERR;
    }

    // get size of shared memory object
    struct stat shm_stats {};
    if (fstat(fd, &shm_stats)) {
        if (close(fd)) { perror("close"); }
        perror("fstat");
        return EX_OSERR;
    }
    const auto size = static_cast<std::size_t>(shm_stats.st_size);

    // map shared memory
    auto data = reinterpret_cast<uint8_t *>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
    if (data == MAP_FAILED || data == nullptr) {
        if (close(fd)) { perror("close"); }
        perror("mmap");
        return EX_OSERR;
    }

    // output data
    const auto offset     = opts["offset"].as<std::size_t>();
    const auto bytes      = opts.count("bytes") ? opts["bytes"].as<std::size_t>() : size;
    const auto write_size = std::min(size - offset, bytes);
    std::cout.write(reinterpret_cast<const char *>(data) + offset, static_cast<std::streamsize>(write_size));
    std::cout.flush();

    // unmap and close shared memory object
    if (munmap(data, size)) { perror("munmap"); }
    if (close(fd)) { perror("close"); }
}
