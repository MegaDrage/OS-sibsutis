#include <iostream>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <string>

using boost::asio::ip::tcp;
using json = nlohmann::json;

void sendCommand(tcp::socket &socket, const json &command) {
    boost::system::error_code error;
    std::string commandStr = command.dump() + "\n";
    boost::asio::write(socket, boost::asio::buffer(commandStr), error);
    if (error) {
        std::cerr << "Error sending command: " << error.message() << std::endl;
    }
}

json readResponse(tcp::socket &socket) {
    boost::system::error_code error;
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\n", error);

    if (error && error != boost::asio::error::eof) {
        std::cerr << "Error reading response: " << error.message() << std::endl;
        return json();
    }

    std::istream responseStream(&response);
    std::string responseLine;
    std::getline(responseStream, responseLine);
    return json::parse(responseLine);
}

int main() {
    try {
        boost::asio::io_context ioContext;
        tcp::socket socket(ioContext);

        // Подключение к серверу
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));

        std::cout << "Connected to server. Use commands:\n";
        std::cout << "- LIST: Show list of processes\n";
        std::cout << "- KILL <pid>: Terminate process with specified PID\n";
        std::cout << "Enter command: ";

        std::string command;
        while (std::getline(std::cin, command)) {
            json request;
            if (command == "LIST") {
                request["command"] = "LIST";
            } else if (command.rfind("KILL", 0) == 0) {
                request["command"] = "KILL";
                int pid = std::stoi(command.substr(5));
                request["pid"] = pid;
            } else {
                std::cout << "Unknown command\n";
                continue;
            }

            sendCommand(socket, request);
            json response = readResponse(socket);

            if (response.contains("status")) {
                std::cout << "Status: " << response["status"] << "\n";
                if (response.contains("data")) {
                    std::cout << "Data:\n" << response["data"] << "\n";
                }
                if (response.contains("message")) {
                    std::cout << "Message: " << response["message"] << "\n";
                }
            } else {
                std::cout << "Invalid response from server\n";
            }

            std::cout << "Enter command: ";
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
