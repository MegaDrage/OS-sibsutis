#ifndef _TASK_MANAGER_CLIENT_HPP
#define _TASK_MANAGER_CLIENT_HPP

#include "../task_manager_elf_viewer/task-manager/process_info.hpp"
#include "ElfInfo.hpp"
#include "logger.hpp"
#include <boost/asio.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>

using boost::asio::ip::tcp;
using json = nlohmann::json;

namespace tmelfv {
// Класс клиента с логированием в буфер
class TaskManagerClient {
public:
  TaskManagerClient(boost::asio::io_context &ioContext, const std::string &host,
                    const std::string &port, Logger &log)
      : ioContext_(ioContext), socket_(ioContext), stopped_(false), log_(log) {
    tcp::resolver resolver(ioContext);
    endpoints_ = resolver.resolve(host, port);
    log_.log("TaskManagerClient initialized with host: " + host +
             ", port: " + port);
  }

  void setProcessListUpdateCallback(
      std::function<void(const std::vector<ProcessInfo> &)> callback) {
    on_process_list_update_ = std::move(callback);
    log_.log("Process list update callback set.");
  }

  void setElfUpdateCallback(std::function<void(const ElfInfo &)> callback) {
    on_process_elf_update_ = std::move(callback);
    log_.log("Process list update callback set.");
  }

  void connect() {
    if (stopped_) {
      log_.log("Connection attempt stopped as client is marked stopped.");
      return;
    }
    log_.log("Attempting to connect to server...");

    boost::asio::async_connect(
        socket_, endpoints_,
        [this](boost::system::error_code ec, tcp::endpoint /*endpoint*/) {
          if (!ec) {
            log_.log("Connected to server.");
          } else {
            log_.log("Error connecting to server: " + ec.message());
          }
        });
  }

  void requestElf(std::string &path) {
    if (stopped_) {
      log_.log("Request for processes stopped as client is marked stopped.");
      return;
    }
    log_.log("Requesting process list from server...");

    json command = {{"command", "READ_ELF"}, {"path", path}};
    std::string commandStr = command.dump() + "\n";

    boost::asio::async_write(
        socket_, boost::asio::buffer(commandStr),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (ec) {
            log_.log("Error sending command: " + ec.message());
          } else {
            log_.log("Process list request sent.");
          }
        });
  }

  void requestProcesses() {
    if (stopped_) {
      log_.log("Request for processes stopped as client is marked stopped.");
      return;
    }
    log_.log("Requesting process list from server...");

    json command = {{"command", "LIST"}};
    std::string commandStr = command.dump() + "\n";

    boost::asio::async_write(
        socket_, boost::asio::buffer(commandStr),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (ec) {
            log_.log("Error sending command: " + ec.message());
          } else {
            log_.log("Process list request sent.");
          }
        });
  }

  void killProcess(int pid) {
    if (stopped_) {
      log_.log("Kill request stopped as client is marked stopped.");
      return;
    }
    log_.log("Sending kill command for PID: " + std::to_string(pid));

    json command = {{"command", "KILL"}, {"pid", pid}};
    std::string commandStr = command.dump() + "\n";

    boost::asio::async_write(
        socket_, boost::asio::buffer(commandStr),
        [](boost::system::error_code ec, std::size_t /*length*/) {
          if (ec) {
            // log_.log("Error sending kill command: " + ec.message());
          } else {
            // log_.log("Kill command sent successfully.");
          }
        });
  }

  void readResponse() {
    if (stopped_) {
      log_.log("Response reading stopped as client is marked stopped.");
      return;
    }
    log_.log("Waiting to read response from server...");

    boost::asio::async_read_until(
        socket_, buffer_, "\n",
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            std::istream responseStream(&buffer_);
            std::string responseLine;
            std::getline(responseStream, responseLine);

            try {
              json response = json::parse(responseLine);
              handleResponse(response);
            } catch (const json::parse_error &e) {
              log_.log("JSON parse error: " + std::string(e.what()));
            }

            readResponse(); // Читаем следующий ответ, если не завершено
          } else if (ec != boost::asio::error::operation_aborted) {
            log_.log("Error reading response: " + ec.message());
          }
        });
  }

  void stop() {
    stopped_ = true; // Устанавливаем флаг остановки
    boost::system::error_code ec;
    log_.log("Stopping client, shutting down socket...");

    if (socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec)) {
      log_.log("Error shutting down socket: " + ec.message());
      ec.clear();
    }
    if (socket_.close(ec)) {
      log_.log("Error closing socket: " + ec.message());
    } else {
      log_.log("Socket closed successfully.");
    }
  }

private:
  boost::asio::io_context &ioContext_;
  tcp::socket socket_;
  tcp::resolver::results_type endpoints_;
  boost::asio::streambuf buffer_;
  Logger &log_;
  bool stopped_; // Флаг остановки для завершения всех операций

  std::function<void(const std::vector<ProcessInfo> &)> on_process_list_update_;
  std::function<void(const ElfInfo &)> on_process_elf_update_;

  void handleResponse(const json &response) {
    log_.log("Received response: " + response.dump(4));

    if (response.contains("status") && response["status"] == "success") {
      if (response.contains("data")) {
        std::vector<ProcessInfo> processes = parseProcesses(response["data"]);
        if (on_process_list_update_) {
          on_process_list_update_(processes);
        }
        log_.log("Process list updated in UI callback.");
      } else if (response.contains("elf")) {
        tmelfv::ElfInfo elfData =
            tmelfv::ElfInfo::parseElfHeader(response["elf"]);
        if (on_process_elf_update_) {
          on_process_elf_update_(elfData);
        }
      } else {
        log_.log("Error: 'data' field is missing in the response.");
      }
    } else {
      log_.log("Error: Server response has status '" +
               response["status"].get<std::string>() + "'");
    }
  }

  std::vector<ProcessInfo> parseProcesses(const std::string &data) {
    std::vector<ProcessInfo> processes;
    std::istringstream stream(data);
    std::string line;

    std::getline(stream, line);
    log_.log("Parsing processes from server response...");

    while (std::getline(stream, line)) {
      std::istringstream lineStream(line);
      ProcessInfo process;
      lineStream >> process.pid >> process.name;
      processes.push_back(process);
    }
    log_.log("Parsed " + std::to_string(processes.size()) + " processes.");
    return processes;
  }
};
} // namespace tmelfv

#endif // _TASK_MANAGER_CLIENT_HPP
