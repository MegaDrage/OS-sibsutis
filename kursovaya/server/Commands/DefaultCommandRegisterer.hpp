#ifndef _DEFAULT_COMMAND_REGISTERER_HPP
#define _DEFAULT_COMMAND_REGISTERER_HPP
#include "../Managers/CommandManager.hpp"
#include "../Managers/TaskManager.hpp"
#include "ICommandRegistrer.hpp"
#include "KillProcessCommand.hpp"
#include "ListProcessesCommand.hpp"
#include "ReadElfCommand.hpp"
namespace tmelfv {
namespace server {
class DefaultCommandRegistrer : public ICommandRegistrer {
public:
  void registerCommands(CommandManager &commandManager,
                        TaskManager &taskManager) override {
    commandManager.registerCommand(
        "LIST", std::make_shared<ListProcessesCommand>(taskManager));
    commandManager.registerCommand(
        "KILL", std::make_shared<KillProcessCommand>(taskManager));
    commandManager.registerCommand(
        "READ_ELF", std::make_shared<ReadElfCommand>(taskManager));
  }
};
} // namespace server
} // namespace tmelfv
#endif // _DEFAULT_COMMAND_REGISTERER_HPP
