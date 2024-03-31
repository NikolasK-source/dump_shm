/*
 * Copyright (C) 2021-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "license.hpp"

#include "cxxsemaphore.hpp"
#include "cxxshm.hpp"
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <sysexits.h>

//! Help output line width
static constexpr std::size_t HELP_WIDTH = 120;

int main(int argc, char **argv) {
    const std::string exe_name = std::filesystem::path(*argv).filename().string();
    cxxopts::Options  options(exe_name, "Dump the content of a shared memory to stdout");

    options.add_options()("b,bytes", "limit number of bytes to output", cxxopts::value<std::size_t>());
    options.add_options()(
            "o,offset", "do not output the leading arg bytes", cxxopts::value<std::size_t>()->default_value("0"));
    options.add_options()("s,semaphore",
                          "protect the shared memory with an existing named semaphore against simultaneous access",
                          cxxopts::value<std::string>());
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
        std::cerr << "Failed to parse arguments: " << e.what() << '\n';
        std::cerr << "Use --help for more information." << '\n';
        return EX_USAGE;
    }

    if (opts.count("help")) {
        options.set_width(HELP_WIDTH);
        std::cout << options.help() << '\n';
        std::cout << '\n';
        std::cout << "This application uses the following libraries:" << '\n';
        std::cout << "  - cxxopts by jarro2783 (https://github.com/jarro2783/cxxopts)" << '\n';
        std::cout << "  - cxxshm (https://github.com/NikolasK-source/cxxshm)" << '\n';
        std::cout << "  - cxxsemaphore (https://github.com/NikolasK-source/cxxsemaphore)" << '\n';
        return EX_OK;
    }

    if (opts.count("version")) {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << " (compiled with " << COMPILER_INFO << " on "
                  << SYSTEM_INFO << ')' << '\n';
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
