/*! \file  texture_base.cppm
 *! \brief 
 *!
 */

export module cydui.backends.texture_base;

import std;
export import reflect;

namespace cydui::backends {
  export class texture_base {
  public:
    using sptr = std::shared_ptr<texture_base>;

    virtual ~texture_base() = default;

    virtual float width() const = 0;
    virtual float height() const = 0;

    // Lock Texture
    virtual std::pair<void*, int> begin_render() = 0;

    // Unlock Texture
    virtual void end_render() = 0;
  };
}