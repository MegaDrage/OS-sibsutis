#include <boost/asio.hpp>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;
using std::string;

class TcpClient {
public:
  TcpClient(io_context &io_context)
      : socket_{io_context}, resolver_{io_context} {}

  void write(const std::string &message) {
    boost::asio::post(socket_.get_executor(),
                      std::bind(&TcpClient::do_write, this, message));
  }

  void connectToServer(const std::string &host, const std::string &port) {
    boost::asio::post(socket_.get_executor(),
                      std::bind(&TcpClient::do_connectToServer, this,
                                resolver_.resolve(host, port)));
  }
  void close() {
    boost::asio::post(socket_.get_executor(),
                      std::bind(&TcpClient::do_close, this));
  }

private:
  void do_connectToServer(tcp::resolver::iterator endpoint_iterator) {
    async_connect(
        socket_, endpoint_iterator,
        std::bind(&TcpClient::handle_connect, this, std::placeholders::_1));
  }

  void handle_connect(const boost::system::error_code &ec) {
    if (!ec) {
      std::cout << "Connected" << std::endl;
      std::string data{"some client data ..."};
      do_write(data);
      read();
    } else {
      std::cerr << "Connect failed: " << ec.message() << std::endl;
    }
  }

  void do_write(const std::string &message) {
    boost::asio::async_write(
        socket_, boost::asio::buffer(message),
        std::bind(&TcpClient::handle_write, this, std::placeholders::_1));
  }

  void handle_write(const boost::system::error_code &ec) {
    std::cerr << "Client Write: " << ec.message() << std::endl;
  }

  void handle_read(boost::system::error_code ec, size_t bytes_transferred) {
    std::cout << "Read " << ec.message() << " (" << bytes_transferred << ")"
              << std::endl;
    if (!ec) {
      std::cout << "Received: " << bytes_transferred << std::endl;
      read();
    } else {
      // Handle the error
    }
  }
  void read() {
    socket_.async_read_some(
        buffer(data_), std::bind(&TcpClient::handle_read, this,
                                 std::placeholders::_1, std::placeholders::_2));
    std::cout << "Have read: " << data_.data() << '\n';
  }
  void do_close() { socket_.close(); }

  tcp::socket socket_;
  std::array<char, 1024> data_{0};
  tcp::resolver resolver_;
};

int main() {
  using boost::asio::ip::tcp;
  boost::asio::io_context io_context;
  TcpClient client(io_context);
  client.connectToServer("127.0.0.1", "25000");
  io_context.run();
  return 0;
}
