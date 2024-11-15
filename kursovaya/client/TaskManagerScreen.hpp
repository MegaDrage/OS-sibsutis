#ifndef _TASK_MANAGER_SCREEN_HPP
#define _TASK_MANAGER_SCREEN_HPP

#include "../task_manager_elf_viewer/task-manager/process_info.hpp"
#include "TaskManagerClient.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <regex>
#include <thread>
#include <vector>

namespace tmelfv {

class TaskManagerScreen {
  std::condition_variable cv_;
  std::mutex mtx_;

public:
  TaskManagerScreen(TaskManagerClient &client,
                    const std::vector<ProcessInfo> &processes)
      : client_(client), processes_(processes), stop_thread_(false) {
    UpdateProcessStrings();
    sort_by_name_ = true; // Изначально сортируем по имени
    sort_ascending_ = true; // Изначально сортировка по возрастанию
  }

  void ShowProcessesScreen() {
    using namespace ftxui;
    stop_thread_ = false;
    client_.requestProcesses();
    // Запускаем поток для автоматического обновления
    update_thread_ = std::thread([this]() {
      while (true) {
        std::unique_lock<std::mutex> lock(mtx_);
        if (cv_.wait_for(lock, std::chrono::seconds(2),
                         [&]() { return stop_thread_; })) {
          break;
        }
        client_.requestProcesses(); // Запрос процессов
      }
    });

    int selected = 0;

    auto back_button = Button("Back", [&] {
      stop_thread_ = true;
      cv_.notify_all(); // Пробуждаем поток
      if (update_thread_.joinable()) {
        update_thread_.join();
      }
      screen.ExitLoopClosure()();
    });

    auto header = hbox({
                      text("PID") | flex,
                      text("Name") | flex,
                  }) |
                  border;

    InputOption iopt;
    iopt.on_change = [&]() {
      UpdateProcessStrings();
      screen.PostEvent(ftxui::Event{});
    };
    auto search_input = Input(&search_query, "Search processes...", iopt);

    auto sort_by_name_button = Button(
        "Sort by Name " +
            std::string(sort_by_name_ ? (sort_ascending_ ? "(Asc)" : "(Desc)")
                                      : ""),
        [&]() {
          sort_by_name_ = true;
          ToggleSortDirection();
          UpdateProcessStrings();
          screen.PostEvent(ftxui::Event{});
        });

    auto sort_by_pid_button = Button(
        "Sort by PID " +
            std::string(!sort_by_name_ ? (sort_ascending_ ? "(Asc)" : "(Desc)")
                                       : ""),
        [&]() {
          sort_by_name_ = false;
          ToggleSortDirection();
          UpdateProcessStrings();
          screen.PostEvent(ftxui::Event{});
        });

    opt.on_enter = [&]() {
      if (selected >= 0 && selected < process_strings.size()) {
        std::string selected_process_str = process_strings[selected];
        std::regex pid_regex(R"(PID:\s*(\d+))");
        std::smatch match;

        if (std::regex_search(selected_process_str, match, pid_regex)) {
          int pid_to_kill = std::stoi(match[1].str());
          std::this_thread::sleep_for(std::chrono::milliseconds(800));
          client_.killProcess(pid_to_kill);
          UpdateProcessStrings();
          screen.PostEvent(ftxui::Event{});
        }
      }
    };

    auto scrollbox = Menu(&process_strings, &selected, opt);

    auto renderer = Renderer(scrollbox, [&] {
      return scrollbox->Render() | vscroll_indicator | frame | border | flex;
    });

    auto layout = Container::Vertical({
        search_input,
        sort_by_name_button,
        sort_by_pid_button,
        renderer,
        back_button,
    });

    auto main_renderer = Renderer(layout, [&] {
      return vbox({
                 search_input->Render(),
                 sort_by_name_button->Render(),
                 sort_by_pid_button->Render(),
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

  void UpdateProcesses(const std::vector<ProcessInfo> &processes) {
    processes_ = processes;
    UpdateProcessStrings();
    screen.PostEvent(ftxui::Event{});
  }

  bool hasProcesses() const { return !processes_.empty(); }
  std::vector<ProcessInfo> GetUpdatedProcesses() { return processes_; }

  void endSession() {
    stop_thread_ = true;
    if (update_thread_.joinable()) {
      update_thread_.join();
    }
    screen.ExitLoopClosure();
  }

private:
  void UpdateProcessStrings() {
    process_strings.clear();

    if (sort_by_name_) {
      SortProcessesByName();
    } else {
      SortProcessesByPid();
    }

    for (const auto &proc : processes_) {
      if (search_query.empty() ||
          proc.name.find(search_query) != std::string::npos) {
        process_strings.push_back("PID: " + std::to_string(proc.pid) +
                                  " Name: " + proc.name);
      }
    }
  }

  void SortProcessesByName() {
    if (sort_ascending_) {
      std::sort(processes_.begin(), processes_.end(),
                [](const ProcessInfo &a, const ProcessInfo &b) {
                  return a.name < b.name;
                });
    } else {
      std::sort(processes_.begin(), processes_.end(),
                [](const ProcessInfo &a, const ProcessInfo &b) {
                  return a.name > b.name;
                });
    }
  }

  void SortProcessesByPid() {
    if (sort_ascending_) {
      std::sort(processes_.begin(), processes_.end(),
                [](const ProcessInfo &a, const ProcessInfo &b) {
                  return a.pid < b.pid;
                });
    } else {
      std::sort(processes_.begin(), processes_.end(),
                [](const ProcessInfo &a, const ProcessInfo &b) {
                  return a.pid > b.pid;
                });
    }
  }

  void ToggleSortDirection() { sort_ascending_ = !sort_ascending_; }

  TaskManagerClient &client_;
  std::vector<ProcessInfo> processes_;
  std::vector<std::string> process_strings;
  ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
  ftxui::MenuOption opt;
  std::string search_query;
  bool sort_by_name_;
  bool sort_ascending_;
  int selected = 0;

  bool stop_thread_;
  std::thread update_thread_;
};

} // namespace tmelfv

#endif // _TASK_MANAGER_SCREEN_HPP
