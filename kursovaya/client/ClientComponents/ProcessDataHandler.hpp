#ifndef PROCESS_DATA_HANDLER_HPP
#define PROCESS_DATA_HANDLER_HPP

#include "../Structures/ProcessInfo.hpp"
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tmelfv {
namespace client {

using json = nlohmann::json;

class ProcessDataHandler {
public:
  using ProcessListCallback =
      std::function<void(const std::vector<ProcessInfo> &)>;

  static void handle(const json &data, ProcessListCallback onProcessListUpdate,
                     std::string &logMessage) {
    if (onProcessListUpdate) {
      std::vector<ProcessInfo> processes = ProcessInfo::parseProcesses(data);
      onProcessListUpdate(processes);
      logMessage = "Process list updated.";
    } else {
      logMessage = "Error: Process list callback not set.";
    }
  }
};

} // namespace client
} // namespace tmelfv

#endif // PROCESS_DATA_HANDLER_HPP
