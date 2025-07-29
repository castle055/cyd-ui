/*! \file  WindowBase.cppm
 *! \brief
 *!
 */

export module cydui.platform.window.window_base;

export import std;
export import reflect;

export import fabric.async;
export import fabric.services;
export import cydui.platform.surface;
export import cydui.service_scopes;

namespace cydui::platform::window {
  export class WindowBase: public fabric::services::ServiceBase {
  public:
    using scope = services::WindowScope;
    using sptr    = std::shared_ptr<WindowBase>;
    using id_type = unsigned int;

    virtual fabric::task<> event_task(fabric::async::async_bus_t::sptr bus) = 0;

    virtual fabric::task<> present(const Surface& frame_buffer) = 0;

    virtual id_type get_id() const = 0;

    virtual fabric::task<> set_position(
      int x,
      int y
    ) = 0;

    virtual fabric::task<std::pair<
      int,
      int>>
    get_size() = 0;

    virtual fabric::task<> enable_text_input() = 0;

    virtual fabric::task<> disable_text_input() = 0;
  };


  export template <typename T>
  concept WindowConcept = std::derived_from<T, WindowBase>;

  export template <typename T>
  concept WindowSptrConcept = std::derived_from<typename T::element_type, WindowBase>;
} // namespace cydui::platform::frame
