/*! \file  frame_base.cppm
 *! \brief 
 *!
 */

export module cydui.backends.frame_base;

import std;
export import reflect;

export import fabric.async;

export import cydui.backends.renderer_base;

namespace cydui::backends {
  export class frame_base: public fabric::async::async_bus_t {
  public:
    using sptr = std::shared_ptr<frame_base>;
    using id_type = unsigned int;

    virtual ~frame_base() = default;

    virtual id_type get_id() const = 0;

    virtual void set_position(int x, int y) = 0;

    virtual std::pair<int, int> get_size() = 0;

    virtual void enable_text_input() = 0;

    virtual void disable_text_input() = 0;

    virtual renderer_base::sptr get_renderer() = 0;

    virtual texture_base::sptr make_texture(bool is_target = false) = 0;
  };

  export template <typename T>
  concept FrameConcept = std::derived_from<T, frame_base>;

  export template <typename T>
  concept FrameSptrConcept = std::derived_from<typename T::element_type, frame_base>;
}