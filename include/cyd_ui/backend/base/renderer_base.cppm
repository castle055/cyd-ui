/*! \file  renderer_base.cppm
 *! \brief
 *!
 */

export module cydui.backends.renderer_base;

import std;
export import reflect;

export import cydui.backends.texture_base;

namespace cydui::backends {
  export class renderer_base {
  public:
    using sptr = std::shared_ptr<renderer_base>;

    virtual ~renderer_base() = default;

    virtual void clear()                                           = 0;
    virtual void render_texture(const texture_base::sptr& texture) = 0;
    virtual void present()                                         = 0;

    virtual void resize_texture(
      const texture_base::sptr& texture,
      float                     new_width,
      float                     new_heigth,
      bool                      copy_old = false
    ) = 0;

    struct rect {
      float x;
      float y;
      float w;
      float h;
    };

    virtual void copy_texture(
      const texture_base::sptr& source,
      const texture_base::sptr& destination,
      rect* source_rect,
      rect* dest_rect,
      bool blend
    ) = 0;
  };
} // namespace cydui::backends
