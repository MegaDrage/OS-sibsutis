#ifndef _PROCESS_INFO_HPP
#define _PROCESS_INFO_HPP
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace tmelfv {
namespace client {
struct ProcessInfo {
  int pid;
  std::string name;

  static std::vector<ProcessInfo> parseProcesses(const std::string &data) {
    std::vector<ProcessInfo> processes;
    std::istringstream stream(data);
    std::string line;

    std::getline(stream, line);

    while (std::getline(stream, line)) {
      std::istringstream lineStream(line);
      ProcessInfo process;
      lineStream >> process.pid >> process.name;
      processes.push_back(process);
    }
    return processes;
  }
};
} // namespace client
} // namespace tmelfv
#endif // _PROCESS_INFO_HPP
