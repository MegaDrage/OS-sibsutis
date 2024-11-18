#ifndef DATA_HANDLER_HPP
#define DATA_HANDLER_HPP
#include "ElfDataHandler.hpp"
#include "ProcessDataHandler.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace tmelfv {
namespace client {

using json = nlohmann::json;
class DataHandler {
public:
  using ProcessListCallback = ProcessDataHandler::ProcessListCallback;
  using ElfUpdateCallback = ElfDataHandler::ElfUpdateCallback;

  static void handleResponse(const json &response,
                             ProcessListCallback onProcessListUpdate,
                             ElfUpdateCallback onElfUpdate,
                             std::string &logMessage) {
    if (response.contains("data")) {
      ProcessDataHandler::handle(response["data"], onProcessListUpdate,
                                 logMessage);
    } else if (response.contains("elf")) {
      ElfDataHandler::handle(response["elf"], onElfUpdate, logMessage);
    } else {
      logMessage = "Error: 'data' or 'elf' field is missing in the response.";
    }
  }
};

} // namespace client
} // namespace tmelfv
#endif // DATA_HANDLER_HPP
