#ifndef RESPONSE_VALIDATOR_HPP
#define RESPONSE_VALIDATOR_HPP

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace tmelfv {
namespace client {
class ResponseValidator {
public:
  static bool isValid(const json &response, std::string &errorMessage) {
    if (!response.contains("status")) {
      errorMessage = "Response missing 'status' field.";
      return false;
    }

    if (response["status"] != "success") {
      errorMessage = "Server response indicates failure: " +
                     response["status"].get<std::string>();
      return false;
    }

    return true;
  }
};

} // namespace client
} // namespace tmelfv
#endif // RESPONSE_VALIDATOR_HPP
