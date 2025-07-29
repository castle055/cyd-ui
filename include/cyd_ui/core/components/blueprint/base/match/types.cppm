/*! \file  types.cppm
 *! \brief
 *!
 */

export module fabric.match:types;

import std;
import fabric.logging;
import fabric.exception;

namespace fabric {
  namespace detail {
    template <typename, typename, template <typename...> typename>
    struct match_trait_list_item;

    template <typename T, typename R, template <typename> typename Trait>
    struct match_trait_list_item<T, R, Trait>: Trait<R> {};

    template <typename T, typename R, template <typename, typename> typename Trait>
    struct match_trait_list_item<T, R, Trait>: Trait<T, R> {};

    export template <typename T, typename R>
    struct match_case_handler;

    template <bool Cond, typename Trait>
    struct auto_trait;

    template <typename Trait>
    struct auto_trait<true, Trait>: Trait {};

    template <typename Trait>
    struct auto_trait<false, Trait> {};
  } // namespace detail

  export template <typename T, typename R>
  struct match;

  template <typename T, typename R, template <typename...> typename... Traits>
  struct match_trait_list: detail::match_trait_list_item<T, R, Traits>... {};

  template <typename, typename>
  struct match_type {};
} // namespace fabric
