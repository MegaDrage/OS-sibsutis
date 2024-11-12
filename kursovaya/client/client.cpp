#include "../task_manager_elf_viewer/task-manager/task_manager.hpp"
#include "../task_manager_elf_viewer/ui/home_screen.hpp"
#include "TCPClient.hpp"
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <nlohmann/json.hpp>

int main() {
  using namespace ftxui;
  try {
    boost::asio::io_context io_context;
    TCPClient client(io_context, "127.0.0.1", "12345");
    io_context.run();
    client.update_processes();
    client.read();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  tmelfv::TaskManager manager;
  tmelfv::HomeScreen home(manager);
  home.MakeLoop();
  return 0;
}
// int main() {
//   try {
//     boost::asio::io_context io_context;
//     TCPClient client(io_context, "127.0.0.1", "12345");
//     io_context.run();
//   } catch (std::exception &e) {
//     std::cerr << "Exception: " << e.what() << std::endl;
//   }
//
//   return 0;
// }
