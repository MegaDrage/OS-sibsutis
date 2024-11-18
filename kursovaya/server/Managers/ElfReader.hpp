#ifndef _ELF_READER_HPP
#define _ELF_READER_HPP
#include <elf.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

class ElfReader {
public:
  static std::string readElfHeader(const std::string &filePath) {
    int fd = open(filePath.c_str(), O_RDONLY);
    if (fd == -1) {
      throw std::runtime_error("Unable to open ELF file: " + filePath);
    }

    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
      close(fd);
      throw std::runtime_error("Unable to get file size for: " + filePath);
    }

    void *map = mmap(nullptr, fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
      close(fd);
      throw std::runtime_error("Unable to map ELF file: " + filePath);
    }

    std::ostringstream ss;
    try {
      Elf64_Ehdr *elfHeader = reinterpret_cast<Elf64_Ehdr *>(map);

      if (elfHeader->e_ident[EI_MAG0] != ELFMAG0 ||
          elfHeader->e_ident[EI_MAG1] != ELFMAG1 ||
          elfHeader->e_ident[EI_MAG2] != ELFMAG2 ||
          elfHeader->e_ident[EI_MAG3] != ELFMAG3) {
        throw std::runtime_error("Not a valid ELF file: " + filePath);
      }

      ss << "ELF Header:\n";
      ss << "  Magic:   ";
      for (int i = 0; i < EI_NIDENT; ++i) {
        ss << std::hex << static_cast<int>(elfHeader->e_ident[i]) << " ";
      }
      ss << "\n";
      ss << "  Class:                             "
         << (elfHeader->e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64" : "ELF32")
         << "\n";
      ss << "  Data:                              "
         << (elfHeader->e_ident[EI_DATA] == ELFDATA2LSB
                 ? "2's complement, little endian"
                 : "Unknown")
         << "\n";
      ss << "  Version:                           "
         << static_cast<int>(elfHeader->e_ident[EI_VERSION]) << "\n";
      ss << "  OS/ABI:                            "
         << static_cast<int>(elfHeader->e_ident[EI_OSABI]) << "\n";
      ss << "  Type:                              " << elfHeader->e_type
         << "\n";
      ss << "  Machine:                           " << elfHeader->e_machine
         << "\n";
      ss << "  Version:                           " << elfHeader->e_version
         << "\n";
      ss << "  Entry point address:              " << std::hex
         << elfHeader->e_entry << "\n";
      ss << "  Start of program headers:         " << elfHeader->e_phoff
         << "\n";
      ss << "  Start of section headers:         " << elfHeader->e_shoff
         << "\n";
      ss << "  Flags:                             " << elfHeader->e_flags
         << "\n";
      ss << "  Size of this header:              " << elfHeader->e_ehsize
         << "\n";

      munmap(map, fileStat.st_size);
    } catch (...) {
      munmap(map, fileStat.st_size);
      close(fd);
      throw;
    }

    close(fd);
    return ss.str();
  }
};
#endif // _ELF_READER_HPP
