#ifndef _ELF_VIEWER_SCREEN_HPP
#define _ELF_VIEWER_SCREEN_HPP

#include "../Structures/ElfInfo.hpp"
#include "../TaskManagerClient.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

namespace tmelfv {
namespace client {

class ElfViewerScreen {
public:
  ElfViewerScreen(TaskManagerClient &client, const ElfInfo &processes)
      : client_(client), elf_data_(processes) {}

  void ShowElfScreen() {
    using namespace ftxui;
    int selected = 0;

    auto back_button = Button("Back", [&] { screen.ExitLoopClosure()(); });
    auto file_input_button =
        Button("Select ELF File", [&] { PromptForFilePathLowLevel(); });

    auto scrollbox = Menu(&process_strings, &selected, opt);

    auto renderer = Renderer(scrollbox, [&] {
      return scrollbox->Render() | vscroll_indicator | frame | border | flex;
    });

    auto layout = Container::Vertical({
        renderer,
        file_input_button,
        back_button,
    });

    auto main_renderer = Renderer(layout, [&] {
      return vbox({
                 text("ELF VIEW") | bold | center | border,
                 renderer->Render(),
                 separator(),
                 file_input_button->Render(),
                 separator(),
                 back_button->Render(),
             }) |
             border;
    });

    screen.Loop(main_renderer);
  }

  void PromptForFilePathLowLevel() {
    using namespace ftxui;

    ftxui::ScreenInteractive local_screen =
        ftxui::ScreenInteractive::FitComponent();
    std::string file_path;
    bool done = false;

    auto input = Input(&file_path, "Enter file path");
    auto button = Button("Submit", [&] {
      done = true;
      local_screen.ExitLoopClosure()(); // Завершаем экран
    });

    auto container = Container::Vertical({
        input,
        button,
    });

    auto renderer = Renderer(container, [&] {
      return vbox({
                 text("Enter the full path to the ELF file:") | center,
                 input->Render() | center,
                 separator(),
                 button->Render() | center,
             }) |
             border | center;
    });

    while (!done) {
      local_screen.Loop(renderer);
    }

    if (!file_path.empty()) {
      client_.requestElf(file_path);
      UpdateProcessString();
    } else {
      bool error_done = false;

      // Экран ошибки с отслеживанием нажатия любой клавиши для закрытия
      auto error_renderer = Renderer([&] {
        return vbox({
                   text("Error: File path cannot be empty!") | bold | center,
                   text("Press any key to continue...") | center,
               }) |
               border;
      });

      // Обработчик для выхода при нажатии любой клавиши
      auto error_event_handler = CatchEvent(error_renderer, [&](Event event) {
        if (event.is_character()) {
          error_done = true;
          local_screen.ExitLoopClosure()();
        }
        return false;
      });

      while (!error_done) {
        local_screen.Loop(error_event_handler);
      }
    }
  }
  void UpdateElfData(const ElfInfo &elf_data) {
    elf_data_ = elf_data;
    UpdateProcessString(); // Обновить строку процесса при обновлении данных
  }

  ElfInfo GetUpdatedElf() { return elf_data_; }

  void endSession() { screen.ExitLoopClosure(); }

private:
  void UpdateProcessString() {
    process_strings.clear();

    process_strings.push_back("Magic: " + elf_data_.magic);
    process_strings.push_back("Class: " + elf_data_.elfClass);
    process_strings.push_back("Data Encoding: " + elf_data_.dataEncoding);
    process_strings.push_back("Version: " + std::to_string(elf_data_.version));
    process_strings.push_back("OS/ABI: " + std::to_string(elf_data_.osAbi));
    process_strings.push_back("Type: " + std::to_string(elf_data_.type));
    process_strings.push_back("Machine: " + std::to_string(elf_data_.machine));
    process_strings.push_back("Entry Point: 0x" + to_hex(elf_data_.entryPoint));
    process_strings.push_back("Program Header Offset: 0x" +
                              to_hex(elf_data_.programHeaderOffset));
    process_strings.push_back("Section Header Offset: 0x" +
                              to_hex(elf_data_.sectionHeaderOffset));
    process_strings.push_back("Flags: " + std::to_string(elf_data_.flags));
    process_strings.push_back("Header Size: " +
                              std::to_string(elf_data_.headerSize));
  }

  std::string to_hex(uint64_t value) {
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
  }

  TaskManagerClient &client_;
  ElfInfo elf_data_;
  std::vector<std::string> process_strings;
  ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
  ftxui::MenuOption opt;
  int selected = 0;
};

} // namespace client
} // namespace tmelfv
#endif // _ELF_VIEWER_SCREEN_HPP
