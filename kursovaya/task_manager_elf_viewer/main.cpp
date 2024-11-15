#include "task-manager/task_manager.hpp"
#include "ui/home_screen.hpp"

int main() {
  using namespace ftxui;
  tmelfv::TaskManager manager;
  tmelfv::HomeScreen home(manager);
  home.MakeLoop();
  return 0;
}
