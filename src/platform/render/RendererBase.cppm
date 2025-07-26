/*! \file  RendererBase.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.renderer_base;

export import std;
export import reflect;

export import fabric.async;
export import cydui.platform.surface;

namespace cydui::platform::render {
  export class FrameRendererBase {
  public:
    using uptr = std::unique_ptr<FrameRendererBase>;

    virtual ~FrameRendererBase() = default;

    virtual Surface finish() = 0;
  };

  export class RendererBase {
  public:
    using sptr = std::shared_ptr<RendererBase>;

    virtual ~RendererBase() = default;

    virtual FrameRendererBase::uptr begin_frame() = 0;
  };


  export template <typename T>
  concept RendererConcept = std::derived_from<T, RendererBase>;

  export template <typename T>
  concept RendererSptrConcept = std::derived_from<typename T::element_type, RendererBase>;
} // namespace cydui::platform::render
