#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP
#include <atomic>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using boost::asio::ip::tcp;
using json = nlohmann::json;

class TCPClient {
public:
  TCPClient(boost::asio::io_context &io_context, const std::string &host,
            const std::string &port)
      : socket_(io_context), resolver_(io_context), running_(true) {
    tcp::resolver resolver(io_context);
    connect(resolver.resolve(host, port));
  }

  void write(const std::string &message) {
    boost::asio::post(socket_.get_executor(),
                      std::bind(&TCPClient::do_write, this, message));
  }

  void read(const std::string &message) {
    boost::asio::post(socket_.get_executor(),
                      std::bind(&TCPClient::do_read, this));
  }

  void close() {
    boost::asio::post(socket_.get_executor(),
                      std::bind(&TCPClient::do_close, this));
  }

  void update_processes() {
    try {
      json request;
      request["command"] = "get_processes";
      std::string request_str = request.dump() + "\n";
      write(request_str);

    } catch (std::exception &e) {
      std::cerr << "Exception in update_processes: " << e.what() << std::endl;
    }
  }

  void kill_process(const std::string &pid) {
    try {
      json request;
      request["command"] = "kill_process";
      request["pid"] = pid;
      std::string request_str = request.dump() + "\n";
      write(request_str);
    } catch (std::exception &e) {
      std::cerr << "Exception in kill_process: " << e.what() << std::endl;
    }
  }

private:
  void connect(tcp::resolver::iterator endpoint_iterator) {
    async_connect(
        socket_, endpoint_iterator,
        std::bind(&TCPClient::handle_connect, this, std::placeholders::_1));
  }

  void handle_connect(const boost::system::error_code &ec) {
    if (!ec) {
      std::cout << "Connected" << std::endl;
    } else {
      std::cerr << "Connect failed: " << ec.message() << std::endl;
    }
  }

  void do_read() {
    boost::asio::async_read_until(
        socket_, buffer_, "\n",
        std::bind(&TCPClient::handle_read, this, std::placeholders::_1));
  }

  void handle_read(const boost::system::error_code &ec) {
    if (!ec) {
      std::istream is(&buffer_);
      std::string response;
      std::getline(is, response);
      handle_response(response);
      do_read();
    } else {
      std::cerr << "Read failed: " << ec.message() << std::endl;
    }
  }

  void handle_response(const std::string &response) {
    json j = json::parse(response);
    if (j.contains("processes")) {
      print_processes(j["processes"]);
    } else if (j.contains("success")) {
      if (j["success"]) {
        std::cout << "Process killed successfully." << std::endl;
      } else {
        std::cout << "Failed to kill process." << std::endl;
      }
    }
  }

  void print_processes(const std::vector<json> &processes) {
    std::cout << "Processes:" << std::endl;
    for (const auto &process : processes) {
      std::cout << "PID: " << process["Pid"] << ", Name: " << process["Name"]
                << std::endl;
    }
  }

  void do_write(const std::string &message) {
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << message;
    boost::asio::async_write(
        socket_, request,
        std::bind(&TCPClient::handle_write, this, std::placeholders::_1));
  }

  void handle_write(const boost::system::error_code &ec) {
    if (ec)
      std::cerr << "Write failed: " << ec.what() << std::endl;
  }

  void do_close() { socket_.close(); }

  tcp::socket socket_;
  tcp::resolver resolver_;
  boost::asio::streambuf buffer_;
  std::atomic<bool> running_;
};

#endif // TCPCLIENT_HPP
