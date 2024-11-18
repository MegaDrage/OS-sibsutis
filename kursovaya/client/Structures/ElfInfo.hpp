#ifndef _ELF_INFO_HPP
#define _ELF_INFO_HPP
#include <cstdint>
#include <regex>
#include <string>

namespace tmelfv {
namespace client {
struct ElfInfo {
  std::string magic;    // Магическое число ELF
  std::string elfClass; // 32 или 64 бита (ELF32 или ELF64)
  std::string dataEncoding; // Кодировка данных (little endian / big endian)
  int version;              // Версия ELF
  int osAbi;                // ОС/ABI
  int type;                 // Тип ELF
  int machine;              // Машина (платформа)
  int version2;             // Вторая версия ELF
  uint64_t entryPoint;          // Адрес точки входа
  uint64_t programHeaderOffset; // Смещение заголовка программы
  uint64_t sectionHeaderOffset; // Смещение заголовка секции
  int flags;                    // Флаги
  int headerSize;               // Размер заголовка ELF

  static ElfInfo parseElfHeader(const std::string &header) {
    ElfInfo elfInfo;

    // Регулярные выражения для извлечения данных
    std::regex magicRegex(R"(Magic:\s+([0-9a-fA-F\s]+))");
    std::regex classRegex(R"(Class:\s+(\S+))");
    std::regex dataEncodingRegex(R"(Data:\s+(.+))");
    std::regex versionRegex(R"(Version:\s+(\d+))");
    std::regex osAbiRegex(R"(OS/ABI:\s+(\d+))");
    std::regex typeRegex(R"(Type:\s+(\d+))");
    std::regex machineRegex(R"(Machine:\s+(\d+))");
    std::regex entryPointRegex(R"(Entry point address:\s+([0-9a-fA-F]+))");
    std::regex programHeaderOffsetRegex(
        R"(Start of program headers:\s+([0-9a-fA-F]+))");
    std::regex sectionHeaderOffsetRegex(
        R"(Start of section headers:\s+([0-9a-fA-F]+))");
    std::regex flagsRegex(R"(Flags:\s+(\d+))");
    std::regex headerSizeRegex(R"(Size of this header:\s+(\d+))");

    // Сопоставляем строки с регулярными выражениями
    std::smatch match;

    if (std::regex_search(header, match, magicRegex)) {
      elfInfo.magic = match[1].str();
    }
    if (std::regex_search(header, match, classRegex)) {
      elfInfo.elfClass = match[1].str();
    }
    if (std::regex_search(header, match, dataEncodingRegex)) {
      elfInfo.dataEncoding = match[1].str();
    }
    if (std::regex_search(header, match, versionRegex)) {
      elfInfo.version = std::stoi(match[1].str());
    }
    if (std::regex_search(header, match, osAbiRegex)) {
      elfInfo.osAbi = std::stoi(match[1].str());
    }
    if (std::regex_search(header, match, typeRegex)) {
      elfInfo.type = std::stoi(match[1].str());
    }
    if (std::regex_search(header, match, machineRegex)) {
      elfInfo.machine = std::stoi(match[1].str());
    }
    if (std::regex_search(header, match, entryPointRegex)) {
      elfInfo.entryPoint = std::stoull(match[1].str(), nullptr, 16);
    }
    if (std::regex_search(header, match, programHeaderOffsetRegex)) {
      elfInfo.programHeaderOffset = std::stoull(match[1].str(), nullptr, 16);
    }
    if (std::regex_search(header, match, sectionHeaderOffsetRegex)) {
      elfInfo.sectionHeaderOffset = std::stoull(match[1].str(), nullptr, 16);
    }
    if (std::regex_search(header, match, flagsRegex)) {
      elfInfo.flags = std::stoi(match[1].str());
    }
    if (std::regex_search(header, match, headerSizeRegex)) {
      elfInfo.headerSize = std::stoi(match[1].str());
    }

    return elfInfo;
  }
};

} // namespace client
} // namespace tmelfv
#endif
