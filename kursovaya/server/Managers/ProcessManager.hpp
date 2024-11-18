#ifndef _PROCESS_MANAGER_HPP
#define _PROCESS_MANAGER_HPP
#include <array>
#include <csignal>
#include <stdexcept>
#include <memory>
#include <string>
class ProcessManager {
public:
  static std::string listProcesses() {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen("ps -e -o pid,comm,state", "r"), pclose);

    if (!pipe) {
      throw std::runtime_error("Unable to fetch processes.");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
    }
    return result;
  }

  static bool killProcess(int pid) {
    if (kill(pid, SIGTERM) == 0) {
      return true;
    } else {
      throw std::runtime_error("Failed to kill process with PID: " +
                               std::to_string(pid));
    }
  }
};

#endif // _PROCESS_MANAGER_HPP
