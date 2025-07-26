/*! \file  WindowBase.cppm
 *! \brief
 *!
 */

export module cydui.platform.window.window_base;

export import std;
export import reflect;

export import fabric.async;
export import cydui.platform.surface;

namespace cydui::platform::window {
  export class WindowBase {
  public:
    using sptr    = std::shared_ptr<WindowBase>;
    using id_type = unsigned int;

    virtual ~WindowBase() = default;

    virtual fabric::task<> event_task() = 0;

    virtual void present(const Surface& frame_buffer) = 0;

    virtual id_type get_id() const = 0;

    virtual void set_position(
      int x,
      int y
    ) = 0;

    virtual std::pair<
      int,
      int>
    get_size() = 0;

    virtual void enable_text_input() = 0;

    virtual void disable_text_input() = 0;
  };


  export template <typename T>
  concept WindowConcept = std::derived_from<T, WindowBase>;

  export template <typename T>
  concept WindowSptrConcept = std::derived_from<typename T::element_type, WindowBase>;
} // namespace cydui::platform::frame
