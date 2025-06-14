/*! \file  backend_base.cppm
 *! \brief 
 *!
 */

export module cydui.backends;

import std;
export import reflect;

export import cydui.backends.backend_base;

namespace cydui::backends {
  export template <BackendConcept Backend, typename ...Args>
  typename std::shared_ptr<Backend> make_backend(Args&&... args) {
    auto backend = std::make_shared<Backend>(std::forward<Args>(args)...);
    return backend;
  }
}