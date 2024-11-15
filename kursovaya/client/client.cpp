#include "../task_manager_elf_viewer/task-manager/process_info.hpp"
#include "../task_manager_elf_viewer/ui/home_screen.hpp"
#include "TaskManagerClient.hpp"
#include "logger.hpp"
#include <atomic>
#include <mutex>
#include <thread>

// Глобальный объект Logger
tmelfv::Logger logger;

std::mutex updateMutex; // Мьютекс для синхронизации обновлений

void startIoContext(boost::asio::io_context &ioContext) {
  try {
    ioContext.run(); // Запуск основного цикла обработки событий
  } catch (const std::exception &e) {
    logger.log("Exception in io_context: " + std::string(e.what()));
  }
}

int main() {
  try {
    boost::asio::io_context ioContext;
    tmelfv::TaskManagerClient client(ioContext, "127.0.0.1", "8080", logger);
    std::vector<tmelfv::ProcessInfo> initialProcesses;
    tmelfv::ElfInfo initialElf;

    std::atomic<bool> exitFlag(false); // Флаг завершения программы
    tmelfv::TaskManagerScreen screen(client, initialProcesses);
    tmelfv::ElfViewerScreen elf_screen(client, initialElf);

    client.setProcessListUpdateCallback(
        [&screen](const std::vector<tmelfv::ProcessInfo> &processes) {
          std::lock_guard<std::mutex> lock(updateMutex); // Блокируем обновление
          screen.UpdateProcesses(processes); // Обновляем процессы на экране
        });

    client.setElfUpdateCallback([&elf_screen](const tmelfv::ElfInfo &elf) {
      std::lock_guard<std::mutex> lock(updateMutex); // Блокируем обновление
      elf_screen.UpdateElfData(elf); // Обновляем процессы на экране
    });

    client.connect();
    client.readResponse();
    // setupPeriodicUpdate(client, ioContext);

    // Создаем HomeScreen с переданным флагом завершения
    tmelfv::HomeScreen hm(screen, elf_screen, exitFlag);

    // Запускаем HomeScreen и io_context в разных потоках
    std::thread ioThread([&ioContext]() {
      startIoContext(ioContext); // Запускаем основной цикл io_context
    });

    // Запускаем основной цикл для интерфейса
    std::thread uiThread([&hm]() {
      hm.MakeLoop(); // Основной цикл интерфейса
    });

    // Основной поток следит за флагом завершения
    while (!exitFlag.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Завершаем io_context и ждем завершения потоков
    ioContext.stop();
    ioThread.join();
    uiThread.join();

  } catch (const std::exception &e) {
    logger.log("Exception: " + std::string(e.what()));
  }

  // Выводим все логи после завершения работы программы
  logger.outputLogs();

  return 0;
}
