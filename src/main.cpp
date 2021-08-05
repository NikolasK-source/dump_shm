#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: " << std::endl;
        std::cerr << "    dump_shm <shm_name>" << std::endl;
        exit(EX_USAGE);
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
