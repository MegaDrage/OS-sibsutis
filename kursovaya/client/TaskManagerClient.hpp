#ifndef _TASK_MANAGER_CLIENT_HPP
#define _TASK_MANAGER_CLIENT_HPP

#include "ClientComponents/DataHandler.hpp"
#include "ClientComponents/NetworkSender.hpp"
#include "ClientComponents/ResponseValidator.hpp"
#include "Commands/CommandBuilder.hpp"
#include "logger.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tmelfv {
namespace client {
using boost::asio::ip::tcp;
using json = nlohmann::json;
class TaskManagerClient {
public:
  TaskManagerClient(boost::asio::io_context &ioContext, const std::string &host,
                    const std::string &port, Logger &log)
      : ioContext_(ioContext), socket_(ioContext), log_(log),
        networkSender_(socket_, log_) {
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

  void requestElf(const std::string &path) {
    log_.log("Requesting ELF data from server...");
    std::string command = CommandBuilder::buildReadElfCommand(path);
    networkSender_.sendAsync(command);
  }

  void requestProcesses() {
    log_.log("Requesting process list from server...");
    std::string command = CommandBuilder::buildListCommand();
    networkSender_.sendAsync(command);
  }

  void killProcess(int pid) {
    log_.log("Sending kill command for PID: " + std::to_string(pid));
    std::string command = CommandBuilder::buildKillCommand(pid);
    networkSender_.sendAsync(command);
  }

  void readResponse() {
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
  NetworkSender networkSender_;

  std::function<void(const std::vector<ProcessInfo> &)> on_process_list_update_;
  std::function<void(const ElfInfo &)> on_process_elf_update_;

  void handleResponse(const json &response) {
    log_.log("Received response: " + response.dump(4));

    std::string errorMessage;
    if (!ResponseValidator::isValid(response, errorMessage)) {
      log_.log("Error: " + errorMessage);
      return;
    }

    std::string logMessage;
    DataHandler::handleResponse(response, on_process_list_update_,
                                on_process_elf_update_, logMessage);

    log_.log(logMessage);
  }
};
} // namespace client
} // namespace tmelfv

#endif // _TASK_MANAGER_CLIENT_HPP
