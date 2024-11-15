#include <array>
#include <boost/asio.hpp>
#include <csignal>
#include <elf.h>
#include <fcntl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using boost::asio::ip::tcp;
using json = nlohmann::json;

class TaskManager {
public:
  std::string listProcesses() {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen("ps -e -o pid,comm,state", "r"), pclose);

    if (!pipe) {
      return "Error: Unable to fetch processes.\n";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
    }
    return result;
  }

  bool killProcess(int pid) { return kill(pid, SIGTERM) == 0; }

  std::string readElfHeader(const std::string &filePath) {
    int fd = open(filePath.c_str(), O_RDONLY);
    if (fd == -1) {
      return "Error: Unable to open ELF file.\n";
    }

    // Отображаем файл в память
    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
      close(fd);
      return "Error: Unable to get file size.\n";
    }

    void *map = mmap(nullptr, fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
      close(fd);
      return "Error: Unable to map ELF file to memory.\n";
    }

    Elf64_Ehdr *elfHeader = reinterpret_cast<Elf64_Ehdr *>(map);
    std::stringstream ss;

    // Проверка на ELF заголовок
    if (elfHeader->e_ident[EI_MAG0] != ELFMAG0 ||
        elfHeader->e_ident[EI_MAG1] != ELFMAG1 ||
        elfHeader->e_ident[EI_MAG2] != ELFMAG2 ||
        elfHeader->e_ident[EI_MAG3] != ELFMAG3) {
      munmap(map, fileStat.st_size);
      close(fd);
      return "Error: Not a valid ELF file.\n";
    }

    ss << "ELF Header:\n";
    ss << "  Magic:   ";
    for (int i = 0; i < EI_NIDENT; ++i) {
      ss << std::hex << static_cast<int>(elfHeader->e_ident[i]) << " ";
    }
    ss << "\n";
    ss << "  Class:                             "
       << (elfHeader->e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64" : "ELF32")
       << "\n";
    ss << "  Data:                              "
       << (elfHeader->e_ident[EI_DATA] == ELFDATA2LSB
               ? "2's complement, little endian"
               : "Unknown")
       << "\n";
    ss << "  Version:                           "
       << static_cast<int>(elfHeader->e_ident[EI_VERSION]) << "\n";
    ss << "  OS/ABI:                            "
       << static_cast<int>(elfHeader->e_ident[EI_OSABI]) << "\n";
    ss << "  Type:                              " << elfHeader->e_type << "\n";
    ss << "  Machine:                           " << elfHeader->e_machine
       << "\n";
    ss << "  Version:                           " << elfHeader->e_version
       << "\n";
    ss << "  Entry point address:              " << std::hex
       << elfHeader->e_entry << "\n";
    ss << "  Start of program headers:         " << elfHeader->e_phoff << "\n";
    ss << "  Start of section headers:         " << elfHeader->e_shoff << "\n";
    ss << "  Flags:                             " << elfHeader->e_flags << "\n";
    ss << "  Size of this header:              " << elfHeader->e_ehsize << "\n";

    munmap(map, fileStat.st_size);
    close(fd);
    std::cout << ss.str() << '\n';
    return ss.str();
  }
};

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(tcp::socket socket, TaskManager &taskManager)
      : socket_(std::move(socket)), taskManager_(taskManager) {}

  void start() { doRead(); }

private:
  void doRead() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_, maxLength),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            std::cout << "Received request: " << data_ << '\n';
            processCommand(std::string(data_, length));
            doRead();
          }
        });
  }

  void processCommand(const std::string &commandStr) {
    json request;
    json response;
    std::cout << "Processing command: " << commandStr << '\n';
    try {
      request = json::parse(commandStr);
    } catch (...) {
      response["status"] = "error";
      response["message"] = "Invalid JSON format";
      sendResponse(response);
      return;
    }

    std::string command = request["command"];
    if (command == "LIST") {
      response["status"] = "success";
      response["data"] = taskManager_.listProcesses();
    } else if (command == "KILL" && request.contains("pid")) {
      int pid = request["pid"];
      bool result = taskManager_.killProcess(pid);
      if (result) {
        response["status"] = "success";
        response["data"] = taskManager_.listProcesses();
      } else {
        response["status"] = "error";
        response["message"] = "Unknown command";
      }
    } else if (command == "READ_ELF" && request.contains("path")) {
      std::string filePath = request["path"];
      std::string elfInfo = taskManager_.readElfHeader(filePath);
      response["status"] = "success";
      response["elf"] = elfInfo;
    } else {
      response["status"] = "error";
      response["message"] = "Unknown command";
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
            std::cerr << "Error sending response: " << ec.message()
                      << std::endl;
            socket_.close();
          }
        });
  }

  tcp::socket socket_;
  TaskManager &taskManager_;
  enum { maxLength = 4096 };
  char data_[maxLength];
};

class Server {
public:
  Server(boost::asio::io_context &ioContext, short port)
      : acceptor_(ioContext, tcp::endpoint(tcp::v4(), port)), taskManager_() {
    doAccept();
  }

private:
  void doAccept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<Session>(std::move(socket), taskManager_)->start();
          }
          doAccept();
        });
  }

  tcp::acceptor acceptor_;
  TaskManager taskManager_;
};

int main() {
  try {
    boost::asio::io_context ioContext;
    Server server(ioContext, 8080);
    std::cout << "Server listening on port 8080" << std::endl;
    ioContext.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
