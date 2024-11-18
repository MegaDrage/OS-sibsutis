#ifndef _LIST_PROCESSES_COMMAND_HPP
#define _LIST_PROCESSES_COMMAND_HPP
#include "../Managers/TaskManager.hpp"
#include "ICommand.hpp"
namespace tmelfv {
namespace server {
using json = nlohmann::json;

class ListProcessesCommand : public ICommand {
public:
  explicit ListProcessesCommand(TaskManager &taskManager)
      : taskManager_(taskManager) {}

  json execute(const json & /*request*/) override {
    return json{{"status", "success"}, {"data", taskManager_.listProcesses()}};
  }

private:
  TaskManager &taskManager_;
};

} // namespace server

} // namespace tmelfv
#endif //_LIST_PROCESSES_COMMAND_HPP
