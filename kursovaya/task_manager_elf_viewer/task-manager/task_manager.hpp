#ifndef _TASK_MANAGER_HPP
#define _TASK_MANAGER_HPP

#include "process_info.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace tmelfv {

class TaskManager {
public:
  using processes_vec = std::vector<ProcessInfo>;
  using processes_info = std::vector<std::string>;

  void LoadInfo() {
    client_.update_processes([this](const std::vector<json> &processes) {
      processes_.clear();
      for (const auto &process : processes) {
        ProcessInfo info;
        info.pid = process["Pid"].get<int>();
        info.name = process["Name"].get<std::string>();
        processes_.push_back(info);
      }
    });
  }

  processes_vec GetProcessesInfo() { return processes_; }

  static processes_info RenderProcesses(const processes_vec &processes) {
    std::vector<std::string> rows;
    for (const auto &process : processes) {
      rows.push_back(std::to_string(process.pid) + " " + process.name);
    }
    return rows;
  }

  processes_info RenderProcesses() {
    std::vector<std::string> rows;
    for (const auto &process : processes_) {
      rows.push_back(std::to_string(process.pid) + " " + process.name);
    }
    return rows;
  }

private:
  processes_vec processes_;
};

} // namespace tmelfv

#endif // _TASK_MANAGER_HPP
