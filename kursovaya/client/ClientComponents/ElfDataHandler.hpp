#ifndef ELF_DATA_HANDLER_HPP
#define ELF_DATA_HANDLER_HPP

#include "../Structures/ElfInfo.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <functional>

namespace tmelfv {
namespace client {

using json = nlohmann::json;

class ElfDataHandler {
public:
  using ElfUpdateCallback = std::function<void(const ElfInfo &)>;

  static void handle(const json &elf, ElfUpdateCallback onElfUpdate,
                     std::string &logMessage) {
    if (onElfUpdate) {
      ElfInfo elfData = ElfInfo::parseElfHeader(elf);
      onElfUpdate(elfData);
      logMessage = "ELF data updated.";
    } else {
      logMessage = "Error: ELF update callback not set.";
    }
  }
};

} // namespace client
} // namespace tmelfv

#endif // ELF_DATA_HANDLER_HPP
