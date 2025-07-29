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
    RGBA32,
    ARGB32,
  };

  export enum class SurfaceType { CPU, GPU };

  export struct Surface {
    SurfaceType type{SurfaceType::CPU};
    void*       handle{nullptr};
    SurfaceSize size{};
    int         pitch{};
    PixelFormat pixel_format{PixelFormat::RGBA32};
  };
} // namespace cydui::platform
