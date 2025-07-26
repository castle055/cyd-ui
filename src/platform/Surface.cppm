/*! \file  Surface.cppm
 *! \brief
 *!
 */

export module cydui.platform.surface;

import std;
import reflect;

import fabric.async;

namespace cydui::platform {
  export using SurfaceSize = std::pair<int, int>;

  export enum class PixelFormat {
    RGBA8,
  };

  enum class SurfaceType { CPU, GPU };

  export struct Surface {
    SurfaceType type{SurfaceType::CPU};
    void*       handle{nullptr};
    SurfaceSize size{};
    PixelFormat pixel_format{PixelFormat::RGBA8};
  };
} // namespace cydui::platform::window
