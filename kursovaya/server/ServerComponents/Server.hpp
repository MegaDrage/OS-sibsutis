#ifndef _SERVER_COMPONENTS_SERVER_HPP
#define _SERVER_COMPONENTS_SERVER_HPP

#include "../Commands/ICommandRegistrer.hpp"
#include "Session.hpp"
#include <boost/asio.hpp>

namespace tmelfv {
namespace server {
using boost::asio::ip::tcp;
class Server {
public:
  Server(boost::asio::io_context &ioContext, short port,
         std::shared_ptr<ICommandRegistrer> registrer)
      : acceptor_(ioContext, tcp::endpoint(tcp::v4(), port)),
        registrer_(registrer) {
    registrer_->registerCommands(commandManager_, taskManager_);
    doAccept();
  }

private:
  void doAccept() {
    acceptor_.async_accept([this](boost::system::error_code ec,
                                  tcp::socket socket) {
      if (!ec) {
        std::make_shared<Session>(std::move(socket), commandManager_)->start();
      }
      doAccept();
    });
  }

  tcp::acceptor acceptor_;
  TaskManager taskManager_;
  CommandManager commandManager_;
  std::shared_ptr<ICommandRegistrer> registrer_;
};
} // namespace server
} // namespace tmelfv
#endif // _SERVER_COMPONENTS_SESSION_HPP
