#ifndef _TASK_MANAGER_HPP
#define _TASK_MANAGER_HPP

#include "ProcessManager.hpp"
#include "ElfReader.hpp"

class TaskManager {
public:
  static std::string listProcesses() {
    return ProcessManager::listProcesses();
  }

  static bool killProcess(int pid) {
    return ProcessManager::killProcess(pid);
  }

  static std::string readElfHeader(const std::string &filePath) {
    return ElfReader::readElfHeader(filePath);
  }
};

#endif // _TASK_MANAGER_HPP
