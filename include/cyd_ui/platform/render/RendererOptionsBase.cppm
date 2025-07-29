/*! \file  RendererOptionsBase.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.renderer_options_base;

import std;
import reflect;


namespace cydui::platform::render {
  export class RendererOptionsBase {
    const refl::type_id_t type_id_;

  public:
    using sptr    = std::shared_ptr<RendererOptionsBase>;

    virtual ~RendererOptionsBase() = default;

    explicit RendererOptionsBase(const refl::type_id_t type_id)
      : type_id_(type_id) {
    }

    RendererOptionsBase(const RendererOptionsBase&) = default;
    RendererOptionsBase(RendererOptionsBase&&) = default;

    refl::type_id_t get_type_id() const {
      return type_id_;
    }

    bool is_type(const refl::type_id_t type_id) const {
      return type_id == type_id_;
    }

    template<typename T>
    bool is_type() const {
      return is_type(refl::type_id<T>);
    }

    template<typename T>
    T& as() {
      if (is_type<T>()) {
        return *dynamic_cast<T*>(this);
      }
      throw std::bad_cast();
    }
  };


  export template <typename T>
  concept RendererOptionsConcept = std::derived_from<T, RendererOptionsBase>;

  export template <typename T>
  concept RendererOptionsSptrConcept = std::derived_from<typename T::element_type, RendererOptionsBase>;
}
