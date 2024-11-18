#ifndef _I_COMMAND_REGISTRER_HPP
#define _I_COMMAND_REGISTRER_HPP
#include "../Managers/CommandManager.hpp"
#include "../Managers/TaskManager.hpp"
namespace tmelfv {
namespace server {
class ICommandRegistrer {
public:
  virtual ~ICommandRegistrer() = default;
  virtual void registerCommands(CommandManager &commandManager,
                                TaskManager &taskManager) = 0;
};
} // namespace server
} // namespace tmelfv

#endif // _I_COMMAND_REGISTRER_HPP
