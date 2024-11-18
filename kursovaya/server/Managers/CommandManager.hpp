#ifndef _COMMAND_MANAGER_HPP
#define _COMMAND_MANAGER_HPP
#include "../Commands/ICommand.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace tmelfv {
namespace server {
using json = nlohmann::json;

class CommandManager {
public:
  void registerCommand(const std::string &name,
                       std::shared_ptr<ICommand> command) {
    commands_[name] = command;
  }

  json executeCommand(const std::string &name, const json &request) {
    if (commands_.find(name) != commands_.end()) {
      return commands_[name]->execute(request);
    }
    return json{{"status", "error"}, {"message", "Unknown command"}};
  }

private:
  std::unordered_map<std::string, std::shared_ptr<ICommand>> commands_;
};
} // namespace server
} // namespace tmelfv
#endif // _COMMAND_MANAGER_HPP
