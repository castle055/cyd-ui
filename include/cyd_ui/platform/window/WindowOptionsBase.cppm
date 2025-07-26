/*! \file  WindowOptionsBase.cppm
 *! \brief
 *!
 */

export module cydui.platform.window.window_options_base;

import std;
import reflect;


namespace cydui::platform::window {
  export class WindowOptionsBase {
  public:
    using sptr    = std::shared_ptr<WindowOptionsBase>;

  };


  export template <typename T>
  concept WindowOptionsConcept = std::derived_from<T, WindowOptionsBase>;

  export template <typename T>
  concept WindowOptionsSptrConcept = std::derived_from<typename T::element_type, WindowOptionsBase>;
}
