/*! \file  backend_base.cppm
 *! \brief 
 *!
 */

export module cydui.backends.backend_base;

import std;
export import reflect;

export import cydui.backends.frame_base;

namespace cydui::backends {
  export class backend_base {
  public:
    using sptr = std::shared_ptr<backend_base>;
    using frame_type = frame_base;

    virtual ~backend_base() = default;

    virtual frame_base::sptr make_frame(const std::string& title, int width, int height) = 0;
  };

  export template <typename T>
  concept BackendConcept = std::derived_from<T, backend_base>;
}