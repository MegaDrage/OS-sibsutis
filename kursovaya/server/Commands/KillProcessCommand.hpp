#ifndef _KILL_PROCESS_COMMAND_HPP
#define _KILL_PROCESS_COMMAND_HPP
#include "../Managers/TaskManager.hpp"
#include "ICommand.hpp"
#include <nlohmann/json.hpp>
namespace tmelfv {
namespace server {
using json = nlohmann::json;
class KillProcessCommand : public ICommand {
public:
  explicit KillProcessCommand(TaskManager &taskManager)
      : taskManager_(taskManager) {}

  json execute(const json &request) override {
    if (!request.contains("pid")) {
      return json{{"status", "error"}, {"message", "Missing 'pid' parameter"}};
    }
    int pid = request["pid"];
    if (taskManager_.killProcess(pid)) {
      return json{{"status", "success"},
                  {"data", taskManager_.listProcesses()}};
    } else {
      return json{{"status", "error"}, {"message", "Failed to kill process"}};
    }
  }

private:
  TaskManager &taskManager_;
};
} // namespace server
} // namespace tmelfv
#endif // _KILL_PROCESS_COMMAND_HPP
