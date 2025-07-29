/*! \file  RendererBase.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.renderer_base;

export import std;
export import reflect;

export import fabric.async;
export import fabric.services;
export import cydui.core.Component.impl;
export import cydui.platform.surface;
export import cydui.service_scopes;

namespace cydui::platform::render {
  export class FrameRendererBase {
  public:
    using uptr = std::unique_ptr<FrameRendererBase>;

    virtual ~FrameRendererBase() = default;

    virtual fabric::task<> render(detail::ComponentImpl& root_component) = 0;

    virtual fabric::task<Surface> finish() = 0;
  };

  export class RendererBase: public fabric::services::ServiceBase {
  public:
    using scope = services::WindowScope;
    using sptr  = std::shared_ptr<RendererBase>;

    virtual fabric::task<FrameRendererBase::uptr> begin_frame(
      int width,
      int height) = 0;

    virtual std::optional<refl::any> mount_component(detail::ComponentImpl& component) {
      return std::nullopt;
    }

    virtual void unmount_component(detail::ComponentImpl& component) {
      return;
    }
  };


  export template <typename T>
  concept RendererConcept = std::derived_from<T, RendererBase>;

  export template <typename T>
  concept RendererSptrConcept = std::derived_from<typename T::element_type, RendererBase>;
} // namespace cydui::platform::render
