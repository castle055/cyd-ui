/*! \file  RendererOptionsBase.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.renderer_options_base;

import std;
import reflect;


namespace cydui::platform::render {
  export class RendererOptionsBase {
  public:
    using sptr    = std::shared_ptr<RendererOptionsBase>;

  };


  export template <typename T>
  concept RendererOptionsConcept = std::derived_from<T, RendererOptionsBase>;

  export template <typename T>
  concept RendererOptionsSptrConcept = std::derived_from<typename T::element_type, RendererOptionsBase>;
}
