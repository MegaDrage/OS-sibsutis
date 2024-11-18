#include "ServerComponents/Server.hpp"
#include "Commands/DefaultCommandRegisterer.hpp"
#include <boost/asio.hpp>

using namespace tmelfv::server;

int main() {
  try {
    boost::asio::io_context ioContext;
    auto registrer = std::make_shared<DefaultCommandRegistrer>();
    Server server(ioContext, 8080, registrer);
    std::cout << "Server listening on port 8080" << std::endl;
    ioContext.run();
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << '\n';
  }

  return 0;
}
