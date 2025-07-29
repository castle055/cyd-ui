/*! \file  trait_base.cppm
 *! \brief
 *!
 */

export module fabric.match:trait_base;
export import :types;

import std;
import fabric.logging;
import fabric.exception;

namespace fabric::match_traits {
  template <typename T, typename R, typename Trait>
  struct trait_base {
  protected:
    const T& get_value() {
      auto* m = static_cast<match<T, R>*>(static_cast<Trait*>(this));
      return m->t;
    }
    detail::
      match_case_handler<
        T,
        R>
      make_case(bool matches) {
      // auto* m = static_cast<match<T, R>*>(this);
      auto* m = static_cast<match<T, R>*>(static_cast<Trait*>(this));
      return detail::match_case_handler {*m, matches};
    }
  };
} // namespace fabric::match_traits
