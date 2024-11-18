#ifndef SCREEN_HANDLER_HPP
#define SCREEN_HANDLER_HPP

namespace tmelfv {
namespace client {

class ScreenHandler {
public:
  virtual void Show() = 0;
  virtual ~ScreenHandler() = default;
};

} // namespace client
} // namespace tmelfv

#endif // SCREEN_HANDLER_HPP
