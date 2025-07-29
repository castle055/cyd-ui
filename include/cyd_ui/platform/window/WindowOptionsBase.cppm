/*! \file  WindowOptionsBase.cppm
 *! \brief
 *!
 */

export module cydui.platform.window.window_options_base;

import std;
import reflect;


namespace cydui::platform::window {
  export class WindowOptionsBase {
    const refl::type_id_t type_id_;

  public:
    using sptr    = std::shared_ptr<WindowOptionsBase>;

    explicit WindowOptionsBase(const refl::type_id_t type_id)
      : type_id_(type_id) {
    }

    WindowOptionsBase(const WindowOptionsBase& rhs) = default;
    WindowOptionsBase(WindowOptionsBase&& rhs) = default;

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

    virtual std::string get_title() const = 0;
  };


  export template <typename T>
  concept WindowOptionsConcept = std::derived_from<T, WindowOptionsBase>;

  export template <typename T>
  concept WindowOptionsSptrConcept = std::derived_from<typename T::element_type, WindowOptionsBase>;
}
