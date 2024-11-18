#ifndef _READ_ELF_COMMAND_HPP
#define _READ_ELF_COMMAND_HPP
#include "../Managers/TaskManager.hpp"
#include "ICommand.hpp"
#include <nlohmann/json.hpp>
namespace tmelfv {
namespace server {
using json = nlohmann::json;
class ReadElfCommand : public ICommand {
public:
  explicit ReadElfCommand(TaskManager &taskManager)
      : taskManager_(taskManager) {}

  json execute(const json &request) override {
    if (!request.contains("path")) {
      return json{{"status", "error"}, {"message", "Missing 'path' parameter"}};
    }
    std::string filePath = request["path"];
    try {
      std::string elfInfo = taskManager_.readElfHeader(filePath);
      return json{{"status", "success"}, {"elf", elfInfo}};
    } catch (const std::exception &e) {
      return json{{"status", "error"}, {"message", e.what()}};
    }
  }

private:
  TaskManager &taskManager_;
};
} // namespace server
} // namespace tmelfv
#endif // _READ_ELF_COMMAND_HPP
