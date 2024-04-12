/*
 * Copyright (C) 2021-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "generated/version_info.hpp"
#include "license.hpp"

#include "cxxsemaphore.hpp"
#include "cxxshm.hpp"
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <sys/ioctl.h>
#include <sysexits.h>

int main(int argc, char **argv) {
    const std::string exe_name = std::filesystem::path(*argv).filename().string();
    cxxopts::Options  options(exe_name, "Dump the content of a shared memory to stdout");

    options.add_options("shared memory")("b,bytes", "limit number of bytes to output", cxxopts::value<std::size_t>());
    options.add_options("shared memory")(
            "o,offset", "do not output the leading arg bytes", cxxopts::value<std::size_t>()->default_value("0"));
    options.add_options("shared memory")(
            "s,semaphore",
            "protect the shared memory with an existing named semaphore against simultaneous access",
            cxxopts::value<std::string>());
    options.add_options("other")("h,help", "print usage");
    options.add_options("version information")("version", "print version and exit");
    options.add_options("version information")("longversion",
                                               "print version (including compiler and system info) and exit");
    options.add_options("version information")("shortversion", "print version (only version string) and exit");
    options.add_options("version information")("git-hash", "print git hash");
    options.add_options("other")("license", "show licences");

    options.add_options("shared memory")("shmname", "name of the shared memory to dump", cxxopts::value<std::string>());
    options.parse_positional({"shmname"});
    options.positional_help("SHM_NAME");

    cxxopts::ParseResult opts;
    try {
        opts = options.parse(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << "Failed to parse arguments: " << e.what() << '\n';
        std::cerr << "Use --help for more information." << '\n';
        return EX_USAGE;
    }

    if (opts.count("help")) {
        static constexpr std::size_t MIN_HELP_SIZE = 80;
        if (isatty(STDIN_FILENO)) {
            struct winsize w {};
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1) {  // NOLINT
                options.set_width(std::max(static_cast<decltype(w.ws_col)>(MIN_HELP_SIZE), w.ws_col));
            }
        } else {
            options.set_width(MIN_HELP_SIZE);
        }

        std::cout << options.help() << '\n';
        std::cout << '\n';
        std::cout << "This application uses the following libraries:" << '\n';
        std::cout << "  - cxxopts by jarro2783 (https://github.com/jarro2783/cxxopts)" << '\n';
        std::cout << "  - cxxshm (https://github.com/NikolasK-source/cxxshm)" << '\n';
        std::cout << "  - cxxsemaphore (https://github.com/NikolasK-source/cxxsemaphore)" << '\n';
        return EX_OK;
    }

    // print version
    if (opts.count("shortversion")) {
        std::cout << PROJECT_VERSION << '\n';
        return EX_OK;
    }

    if (opts.count("version")) {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << '\n';
        return EX_OK;
    }

    if (opts.count("longversion")) {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << '\n';
        std::cout << "   compiled with " << COMPILER_INFO << '\n';
        std::cout << "   on system " << SYSTEM_INFO
#ifndef OS_LINUX
                  << "-nonlinux"
#endif
                  << '\n';
        std::cout << "   from git commit " << RCS_HASH << '\n';
        return EX_OK;
    }

    if (opts.count("git-hash")) {
        std::cout << RCS_HASH << '\n';
        return EX_OK;
    }

    if (opts.count("license")) {
        print_licenses(std::cout);
        return EX_OK;
    }

    if (!opts.count("shmname")) {
        std::cerr << "Shared memory name is mandatory." << '\n';
        std::cerr << "Use --help for more information." << '\n';
        return EX_USAGE;
    }

    const auto name = opts["shmname"].as<std::string>();

    std::unique_ptr<cxxshm::SharedMemory> shared_memory;
    try {
        shared_memory = std::make_unique<cxxshm::SharedMemory>(name, true);
    } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
        return EX_SOFTWARE;
    }

    std::unique_ptr<cxxsemaphore::Semaphore> semaphore;
    if (opts.count("semaphore")) {
        try {
            semaphore = std::make_unique<cxxsemaphore::Semaphore>(opts["semaphore"].as<std::string>());
        } catch (std::exception &e) {
            std::cerr << e.what() << '\n';
            return EX_SOFTWARE;
        }
    }

    // output data
    const auto offset     = opts["offset"].as<std::size_t>();
    const auto bytes      = opts.count("bytes") ? opts["bytes"].as<std::size_t>() : shared_memory->get_size();
    const auto write_size = std::min(shared_memory->get_size() - offset, bytes);
    if (semaphore) semaphore->wait();
    std::cout.write(shared_memory->get_addr<const char *>() + offset, static_cast<std::streamsize>(write_size));
    if (semaphore) semaphore->post();
    std::cout.flush();
}
