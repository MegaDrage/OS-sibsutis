#ifndef _UI_TASK_ELF_MENU_HPP
#define _UI_TASK_ELF_MENU_HPP

#include "ElfViewerScreen.hpp"
#include "TaskManagerScreen.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <string>
#include <vector>

namespace tmelfv {
namespace client {
enum MenuTypes { PROCESSES_VIEWER, ELF_VIEWER, EXIT };

class HomeScreen {
public:
  HomeScreen() = delete;

  // Обычный конструктор
  HomeScreen(TaskManagerScreen &tm_screen, ElfViewerScreen &elf_screen,
             std::atomic<bool> &exitFlag)
      : tm_screen_(tm_screen), elf_screen_(elf_screen) {
    using namespace ftxui;
    option.on_enter = [&] {
      if (selected_mode == MenuTypes::PROCESSES_VIEWER) {
        tm_screen_.ShowProcessesScreen();
      }
      if (selected_mode == MenuTypes::ELF_VIEWER) {
        elf_screen_.ShowElfScreen();
      }
      if (selected_mode == MenuTypes::EXIT) {
        exitFlag = true;
        screen.ExitLoopClosure()();
      }
    };

    menu = ftxui::Menu(&menu_entries, &selected_mode, option);
    menu_renderer = Renderer(menu, [&] {
      return window(text("Task Manager and ELF Viewer") | bold | center,
                    vbox({hbox({
                              menu->Render() | border | flex,
                          }) |
                          flex}));
    });
  }

  void MakeLoop() { screen.Loop(menu_renderer); }

private:
  ftxui::MenuOption option;
  ftxui::Component menu;
  ftxui::Component menu_renderer;
  std::vector<std::string> menu_entries = {"Processes", "ELF Viewer", "Exit"};
  int selected_mode = 0;
  ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
  TaskManagerScreen &tm_screen_;
  ElfViewerScreen &elf_screen_;
};
} // namespace client
} // namespace tmelfv
#endif // _UI_TASK_ELF_MENU_HPP
