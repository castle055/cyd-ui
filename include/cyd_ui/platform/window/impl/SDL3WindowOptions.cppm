/*! \file  SDL3WindowOptions.cppm
 *! \brief
 *!
 */

export module cydui.platform.window.sdl3;

import std;
import reflect;

export import cydui.platform.window.window_options_base;

namespace cydui::platform::window {
  export class SDL3WindowOptions final: public WindowOptionsBase {
  public:
    std::string title;
    int         x {}, y {};
    int         width, height;
    bool        x11_override_redirect {false};

    SDL3WindowOptions(
      std::string title,
      int         width,
      int         height)
        : WindowOptionsBase(refl::type_id<SDL3WindowOptions>),
          title(std::move(title)),
          width(width),
          height(height) {}

    std::string get_title() const override {
      return title;
    }
  };
} // namespace cydui::platform::window
