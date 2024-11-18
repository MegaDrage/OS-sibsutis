#include "ClientComponents/AppRunner.hpp"
#include "Structures/ElfInfo.hpp"
#include "Structures/ProcessInfo.hpp"
#include "TaskManagerClient.hpp"
#include "UI/ElfViewerScreen.hpp"
#include "UI/HomeScreen.hpp"
#include "logger.hpp"
#include <atomic>

// Глобальный объект Logger
using namespace tmelfv::client;
tmelfv::Logger logger;

std::mutex updateMutex;

int main() {
  try {
    boost::asio::io_context ioContext;
    TaskManagerClient client(ioContext, "127.0.0.1", "8080", logger);

    std::vector<ProcessInfo> initialProcesses;
    ElfInfo initialElf;
    TaskManagerScreen taskScreen(client, initialProcesses);
    ElfViewerScreen elfScreen(client, initialElf);
    client.setProcessListUpdateCallback(
        [&taskScreen](const std::vector<ProcessInfo> &processes) {
          std::lock_guard<std::mutex> lock(updateMutex);
          taskScreen.UpdateProcesses(processes);
        });

    client.setElfUpdateCallback([&elfScreen](const ElfInfo &elf) {
      std::lock_guard<std::mutex> lock(updateMutex);
      elfScreen.UpdateElfData(elf);
    });

    client.connect();
    client.readResponse();
    std::atomic<bool> exitFlag(false);

    auto homeScreen =
        std::make_shared<HomeScreen>(taskScreen, elfScreen, exitFlag);

    AppRunner appRunner(ioContext, exitFlag, logger);
    appRunner.setHomeScreen(homeScreen);
    appRunner.run();

  } catch (const std::exception &e) {
    logger.log("Exception: " + std::string(e.what()));
  }

  logger.outputLogs();

  return 0;
}
