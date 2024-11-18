#ifndef _I_COMMAND_HPP
#define _I_COMMAND_HPP
#include <nlohmann/json.hpp>
namespace tmelfv {
namespace server {
class ICommand {
public:
  virtual ~ICommand() = default;
  virtual nlohmann::json execute(const nlohmann::json &request) = 0;
};
}
} // namespace tmelfv
#endif // _I_COMMAND_HPP
