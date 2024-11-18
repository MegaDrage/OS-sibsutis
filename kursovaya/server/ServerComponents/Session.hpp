#ifndef _SERVER_COMPONENTS_SESSION_HPP
#define _SERVER_COMPONENTS_SESSION_HPP
#include "../Managers/CommandManager.hpp"
#include <boost/asio.hpp>
#include <iostream>
namespace tmelfv {
namespace server {

using boost::asio::ip::tcp;
class Session : public std::enable_shared_from_this<Session> {
public:
  Session(tcp::socket socket, CommandManager &commandManager)
      : socket_(std::move(socket)), commandManager_(commandManager) {}

  void start() { doRead(); }

private:
  void doRead() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_, maxLength),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            std::string commandStr(data_, length);
            std::cout << "Received request: " << commandStr << '\n';
            handleRequest(commandStr);
            doRead();
          }
        });
  }

  void handleRequest(const std::string &commandStr) {
    json request, response;
    try {
      request = json::parse(commandStr);
      std::string commandName = request["command"];
      response = commandManager_.executeCommand(commandName, request);
    } catch (...) {
      response = json{{"status", "error"}, {"message", "Invalid JSON format"}};
    }
    sendResponse(response);
  }

  void sendResponse(const json &response) {
    auto responseStr = response.dump() + "\n";
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_, boost::asio::buffer(responseStr),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (ec) {
            std::cerr << "Error sending response: " << ec.message() << '\n';
            socket_.close();
          }
        });
  }

  tcp::socket socket_;
  CommandManager &commandManager_;
  static constexpr size_t maxLength = 4096;
  char data_[maxLength];
};
} // namespace server
} // namespace tmelfv
#endif // _SERVER_COMPONENTS_SESSION_HPP
