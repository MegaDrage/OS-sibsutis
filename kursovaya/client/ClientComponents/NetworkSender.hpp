#ifndef NETWORK_SENDER_HPP
#define NETWORK_SENDER_HPP

#include "../logger.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace tmelfv {

class NetworkSender {
public:
  NetworkSender(boost::asio::ip::tcp::socket &socket, Logger &log)
      : socket_(socket), log_(log) {}

  void sendAsync(const std::string &command) {
    boost::asio::async_write(
        socket_, boost::asio::buffer(command),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (ec) {
            log_.log("Error sending command: " + ec.message());
          } else {
            log_.log("Command sent successfully.");
          }
        });
  }

private:
  tcp::socket &socket_;
  Logger &log_;
};

} // namespace tmelfv

#endif // NETWORK_SENDER_HPP
