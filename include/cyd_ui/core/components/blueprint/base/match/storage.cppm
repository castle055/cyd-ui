/*! \file  storage.cppm
 *! \brief
 *!
 */

export module fabric.match:storage;
export import :types;

import std;
import fabric.logging;
import fabric.exception;

namespace fabric::detail {
  template <typename R>
  struct match_result_storage {
    R    value;
    bool match {false};
  };

  template <>
  struct match_result_storage<void> {};

  template <typename T, typename R = void>
  class match_storage {
    match_result_storage<R> res;
    bool                    shorted {false};

  public:
    const T& t;

    explicit match_storage(
      const T&                t,
      match_result_storage<R> result_,
      bool                    shorted_)
        : res(result_),
          shorted(shorted_),
          t(t) {}

    explicit match_storage(const T& t)
        : res {},
          t(t) {}

    template <typename U>
    explicit match_storage(
      const T& t,
      U        default_)
        : res(
            std::forward<R>(default_),
            true),
          t(t) {}

    template <typename S>
    match<T, S> force_type() {
      return match<T, S> {t};
    }

    R operator*() const {
      if constexpr (not std::is_void_v<R>) {
        if (res.match) {
          return res.value;
        } else {
          throw fabric::exception {std::format("Missing default case")};
        }
      }
    }

    // operator R() {
    //   if constexpr (not std::is_void_v<R>) {
    //     if (res.match) {
    //       return res.value;
    //     } else {
    //       throw fabric::exception {std::format("Missing default case")};
    //     }
    //   }
    // }

    template <typename, typename>
    friend struct match_case_handler;
  };

  template <
    typename T,
    typename R>
  match_storage(
    T,
    R)
    -> match_storage<
      T,
      R>;
} // namespace fabric
