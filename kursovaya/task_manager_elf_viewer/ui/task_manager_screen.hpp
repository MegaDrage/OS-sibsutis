#ifndef _TASK_MANAGER_SCREEN_HPP
#define _TASK_MANAGER_SCREEN_HPP

#include "../task-manager/process_info.hpp"
#include <ftxui/component/component_options.hpp>
#include <string> // for operator+, string, char_traits, basic_string

#include "../task-manager/task_manager.hpp"
#include <ftxui/component/component.hpp>      // for Button, Vertical, Renderer
#include <ftxui/component/component_base.hpp> // for ComponentBase
#include <ftxui/component/screen_interactive.hpp> // for ScreenInteractive
#include <ftxui/dom/elements.hpp> // for separator, text, Element, operator|, vbox, border, hbox

namespace tmelfv {
class TaskManagerScreen {

public:
  static void ProcessesScreen(const std::vector<ProcessInfo> &processes) {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();
    auto back_button = Button("Back", screen.ExitLoopClosure());

    auto header = hbox({
                      text("PID") | flex,
                      text("Name") | flex,
                      text("CPU %") | flex,
                      text("Memory %") | flex,
                  }) |
                  border;

    int selected = 0;
    MenuOption opt;
    auto available_processes = TaskManager::RenderProcesses(processes);
    auto scrollbox = Menu(&available_processes, &selected, opt);
    auto renderer = Renderer(scrollbox, [&] {
      return scrollbox->Render() | vscroll_indicator | frame | border | flex;
    });

    auto layout = Container::Vertical({
        scrollbox,
        back_button,
    });

    auto main_renderer = Renderer(layout, [&] {
      return vbox({
                 text("Current Processes") | bold | center | border,
                 header,
                 renderer->Render(),
                 separator(),
                 back_button->Render(),
             }) |
             border;
    });

    screen.Loop(main_renderer);
  }
};
} // namespace tmelfv

#endif // _TASK_MANAGER_SCREEN_HPP
