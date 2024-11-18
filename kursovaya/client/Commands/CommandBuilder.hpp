#ifndef COMMAND_BUILDER_HPP
#define COMMAND_BUILDER_HPP

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace tmelfv {
namespace client {
class CommandBuilder {
public:
  static std::string buildReadElfCommand(const std::string &path) {
    json command = {{"command", "READ_ELF"}, {"path", path}};
    return command.dump() + "\n";
  }

  static std::string buildListCommand() {
    json command = {{"command", "LIST"}};
    return command.dump() + "\n";
  }

  static std::string buildKillCommand(int pid) {
    json command = {{"command", "KILL"}, {"pid", pid}};
    return command.dump() + "\n";
  }
};

} // namespace client
} // namespace tmelfv

#endif // COMMAND_BUILDER_HPP
