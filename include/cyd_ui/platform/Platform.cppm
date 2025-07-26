/*! \file  Platform.cppm
 *! \brief
 *!
 */

export module cydui.platform;

import std;
import reflect;

import cydui.platform.window.window_options_base;
import cydui.platform.render.renderer_options_base;


namespace cydui::platform {
  export class PlatformImpl;

  export class Platform {
    std::unique_ptr<PlatformImpl> impl_;

  public:
    explicit Platform(
      const window::WindowOptionsBase::sptr&   window_opts,
      const render::RendererOptionsBase::sptr& renderer_opts
    );
  };
} // namespace cydui::platform
