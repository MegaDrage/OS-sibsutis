#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <ftxui/util/ref.hpp>

#include <elfio/elfio.hpp>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace ftxui;

struct ProcessInfo {
  int pid;
  std::string name;
  float cpu_usage;
  float memory_usage;
};

struct ElfHeaderInfo {
  std::string type;
  std::string machine;
  std::string version;
  std::string entry_point;
};

std::vector<ProcessInfo> GetProcessesInfo() {
  std::vector<ProcessInfo> processes;
  std::string ps_output = "ps -eo pid,comm,%cpu,%mem --no-headers";
  FILE *pipe = popen(ps_output.c_str(), "r");
  if (!pipe)
    return processes;

  char buffer[128];
  while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
    std::istringstream iss(buffer);
    ProcessInfo info;
    iss >> info.pid >> info.name >> info.cpu_usage >> info.memory_usage;
    processes.push_back(info);
  }
  pclose(pipe);
  return processes;
}

Element RenderProcesses(const std::vector<ProcessInfo> &processes) {
  Elements rows;
  for (const auto &process : processes) {
    rows.push_back(hbox({
        text(std::to_string(process.pid)) | flex,
        text(process.name) | flex,
        text(std::to_string(process.cpu_usage) + "%") | flex,
        text(std::to_string(process.memory_usage) + "%") | flex,
    }));
  }
  return vbox(hbox({
                  text("PID") | flex,
                  text("Name") | flex,
                  text("CPU %") | flex,
                  text("Memory %") | flex,
              }) | border,
              vbox(rows) | border);
}

ElfHeaderInfo GetElfHeaderInfo(const std::string &filename) {
  ELFIO::elfio reader;
  if (!reader.load(filename)) {
    throw std::runtime_error("Cannot open file " + filename);
  }

  ElfHeaderInfo info;
  info.type = std::to_string(reader.get_type());
  info.machine = std::to_string(reader.get_machine());
  info.version = std::to_string(reader.get_version());
  info.entry_point = std::to_string(reader.get_entry());

  return info;
}

Element RenderElfHeader(const ElfHeaderInfo &info) {
  return vbox({
             text("Type: " + info.type),
             text("Machine: " + info.machine),
             text("Version: " + info.version),
             text("Entry Point: " + info.entry_point),
         }) |
         border;
}

int main() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::vector<ProcessInfo> processes = GetProcessesInfo();
  std::string elf_filename = "liblist.so"; // Замените на имя вашего ELF-файла
  ElfHeaderInfo elf_info = GetElfHeaderInfo(elf_filename);

  int selected_mode = 0; // 0 - Processes, 1 - ELF Viewer, 2 - Exit

  std::vector<std::string> menu_entries = {"Processes", "ELF Viewer", "Exit"};

  auto back_button = Button("Back To Menu", [&] { selected_mode = 0; });

  auto processes_window = Window({});

  auto processes_renderer = Renderer(processes_window, [&] {
    return window(text("Processes") | bold | center,
                  vbox({RenderProcesses(processes), back_button->Render() | center}));
  });

  // MenuOption option;
  // option.on_enter = [&] {
  //   if (selected_mode == 1) {
  //     processes_renderer->Render();
  //   }
  //   if (selected_mode == 2) {
  //     screen.ExitLoopClosure()();
  //   }
  // };
  //
  auto menu = Menu(&menu_entries, &selected_mode);

  auto menu_renderer = Renderer(menu, [&] {
    return window(text("Task Manager and ELF Viewer") | bold | center,
                  vbox({hbox({
                            menu->Render() | border | flex,
                        }) |
                        flex}));
  });

  auto main_renderer = Renderer([&] {
        if (selected_mode == 0) {
            return menu_renderer->Render();
        } else if (selected_mode == 1) {
            return processes_renderer->Render();
        // } else if (selected_mode == 2) {
            // return elf_renderer->Render();
        } else {
            return text("Exit") | center;
        }
    });

  screen.Loop(main_renderer);
  return 0;
}
