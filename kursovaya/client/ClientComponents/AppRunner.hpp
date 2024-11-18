#ifndef _APP_RUNNER_HPP
#define _APP_RUNNER_HPP
#include "../UI/HomeScreen.hpp"
#include "../logger.hpp"
#include <boost/asio.hpp>
#include <thread>

namespace tmelfv {
namespace client {

class AppRunner {
public:
  AppRunner(boost::asio::io_context &ioContext, std::atomic<bool> &exitFlag,
            Logger &logger)
      : ioContext_(ioContext), exitFlag_(exitFlag), logger_(logger) {}

  void run() {
    ioThread_ = std::thread([this]() { startIoContext(ioContext_); });

    uiThread_ = std::thread([this]() { homeScreen_->MakeLoop(); });

    while (!exitFlag_.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ioContext_.stop();
    ioThread_.join();
    uiThread_.join();
  }

  void setHomeScreen(std::shared_ptr<HomeScreen> homeScreen) {
    homeScreen_ = homeScreen;
  }

private:
  boost::asio::io_context &ioContext_;
  std::atomic<bool> &exitFlag_;
  std::thread ioThread_;
  std::thread uiThread_;
  std::shared_ptr<HomeScreen> homeScreen_;
  Logger &logger_;

  void startIoContext(boost::asio::io_context &ioContext) {
    try {
      ioContext.run();
    } catch (const std::exception &e) {
      logger_.log("Exception in io_context: " + std::string(e.what()));
    }
  }
};
} // namespace client
} // namespace tmelfv
#endif // _APP_RUNNER_HPP
