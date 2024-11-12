#include <boost/asio.hpp>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using boost::asio::ip::tcp;
using json = nlohmann::json;

class TCPServer {
public:
  TCPServer(boost::asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    start_accept();
  }

private:
  void start_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::cout << "creating session on: "
                      << socket.remote_endpoint().address().to_string() << ":"
                      << socket.remote_endpoint().port() << '\n';
            std::make_shared<Session>(std::move(socket))->start();
          } else {
            std::cout << "error: " << ec.message() << std::endl;
          }
          start_accept();
        });
  }

  class Session : public std::enable_shared_from_this<Session> {
  public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() { do_read(); }

  private:
    void do_read() {
      auto self(shared_from_this());
      boost::asio::async_read_until(
          socket_, buffer_, "\n",
          [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
              std::istream is(&buffer_);
              std::string request;
              std::getline(is, request);
              std::cout << "Request is: " << request << '\n';
              handle_request(request);
              do_read();
            } else {
              std::cout << "Read error: " << ec.message() << std::endl;
              if (ec == boost::asio::error::eof ||
                  ec == boost::asio::error::connection_reset) {
                std::cout << "Client disconnected." << std::endl;
                socket_.close();
              }
            }
          });
    }

    void handle_request(const std::string &request) {
      try {
        json j = json::parse(request);
        json response;

        if (j["command"] == "get_processes") {
          std::vector<json> processes = get_processes();
          response["processes"] = processes;
        } else if (j["command"] == "kill_process") {
          bool success = kill_process(j["pid"]);
          response["success"] = success;
        }

        std::string response_str = response.dump() + "\n";

        if (!socket_.is_open()) {
          std::cerr << "Socket is closed, cannot write response." << std::endl;
          return;
        }
        boost::asio::streambuf write_buffer;
        std::ostream output(&write_buffer);
        output << response_str;
        boost::asio::write(socket_, write_buffer.data());
        // boost::asio::async_write(
        //     socket_, write_buffer.data(),
        //     [this, self = shared_from_this()](boost::system::error_code ec,
        //                                       std::size_t length) {
        //       if (ec) {
        //         std::cerr << "Write error: " << ec.what() << std::endl;
        //       } else {
        //         std::cout << "Write successful, length: " << length
        //                   << std::endl;
        //       }
        //     });
      } catch (const json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
      }
    }

    std::vector<json> get_processes() {
      std::vector<json> processes;
      DIR *dir = opendir("/proc");
      if (dir) {
        struct dirent *ent;
        while ((ent = readdir(dir)) != nullptr) {
          if (ent->d_type == DT_DIR) {
            std::string dir_name = ent->d_name;
            if (dir_name.find_first_not_of("0123456789") == std::string::npos) {
              json process_info = get_process_info(dir_name);
              processes.push_back(process_info);
            }
          }
        }
        closedir(dir);
      }
      return processes;
    }

    json get_process_info(const std::string &pid) {
      json process_info;
      std::ifstream status_file("/proc/" + pid + "/status");
      if (status_file.is_open()) {
        std::string line;
        while (std::getline(status_file, line)) {
          size_t pos = line.find(':');
          if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            value.erase(std::remove(value.begin(), value.end(), '\t'),
                        value.end());
            process_info[key] = value;
          }
        }
        status_file.close();
      }
      return process_info;
    }

    bool kill_process(const std::string &pid) {
      pid_t pid_num = std::stoi(pid);
      return kill(pid_num, SIGKILL) == 0;
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
  };

  tcp::acceptor acceptor_;
};

int main() {
  try {
    boost::asio::io_context io_context;
    TCPServer server(io_context, 12345);
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
