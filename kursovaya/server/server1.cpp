#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <thread>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;
using std::string;

class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket socket) : m_socket(std::move(socket)) {}

  void run() { wait_for_request(); }

private:
  void wait_for_request() {
    auto self(shared_from_this());
    boost::asio::async_read_until(
        m_socket, m_buffer, "\0",
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            std::string data{std::istreambuf_iterator<char>(&m_buffer),
                             std::istreambuf_iterator<char>()};

            std::cout << "Request: " << data << std::endl;
            handle_request(data);
            wait_for_request();
          } else {
            std::cout << "error: " << ec << std::endl;
            ;
          }
        });
  }
  void handle_request(const std::string &request) {
    std::string data{"some server data ..." + request};
    boost::asio::async_write(
        m_socket, boost::asio::buffer(data),
        std::bind(&session::handle_write, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
    // json j = json::parse(request);
    // json response;
    //
    // if (j["command"] == "get_processes") {
    //   std::vector<json> processes = get_processes();
    //   response["processes"] = processes;
    // } else if (j["command"] == "kill_process") {
    //   bool success = kill_process(j["pid"]);
    //   response["success"] = success;
    // }
    //
    // std::string response_str = response.dump() + "\n";
    // boost::asio::async_write(
    //     socket_, boost::asio::buffer(response_str),
    //     [this, self = shared_from_this()](boost::system::error_code ec,
    //                                       std::size_t length) {
    //       if (ec) {
    //         std::cerr << "Write error: " << ec.message() << std::endl;
    //       }
    //     });
  }

private:
  void handle_write(const boost::system::error_code &err,
                    size_t /*bytes_transferred*/) {
    std::cerr << "Server Write:" << err.message() << '\n';
  }
  tcp::socket m_socket;
  boost::asio::streambuf m_buffer;
};

class server {
public:
  server(boost::asio::io_context &io_context, short port)
      : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }

private:
  void do_accept() {
    m_acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::cout << "creating session on: "
                      << socket.remote_endpoint().address().to_string() << ":"
                      << socket.remote_endpoint().port() << '\n';
            std::make_shared<session>(std::move(socket))->run();
          } else {
            std::cout << "error: " << ec.message() << std::endl;
          }
          do_accept();
        });
  }

private:
  tcp::acceptor m_acceptor;
};

int main() {
  boost::asio::io_context io_context;
  server s(io_context, 25000);
  io_context.run();

  return 0;
}
