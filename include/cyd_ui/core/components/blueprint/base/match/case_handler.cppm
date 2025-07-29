/*! \file  case_handler.cppm
 *! \brief
 *!
 */

export module fabric.match:case_handler;
export import :storage;

import std;
import fabric.logging;
import fabric.exception;

namespace fabric::detail {
  template <typename T, typename R>
  struct match_case_handler {
    match<T, R> parent;
    bool        selected;

    match<T, R> operator()(R&& res) {
      if (selected and not parent.shorted) {
        return match<T, R> {
          parent.t, match_result_storage<R> {std::forward<R>(res), true},
            true
        };
      } else {
        return parent;
      }
    }

    match<T, R> operator()(const R& res) {
      if (selected and not parent.shorted) {
        return match<T, R> {
          parent.t, match_result_storage<R> {res, true},
            true
        };
      } else {
        return parent;
      }
    }

    template <typename S>
      requires (std::same_as<S, R>)
      match<T, S> operator()(S&& res) {
      if (selected and not parent.shorted) {
        return match<T, S> {
          parent.t, match_result_storage<S> {std::forward<S>(res), true},
            true
        };
      } else {
        return parent;
      }
    }

    template <typename S>
      requires (not std::same_as<S, R> and std::convertible_to<S, R>)
      match<T, R> operator()(S&& res) {
      if (selected and not parent.shorted) {
        return match<T, R> {
          parent.t, match_result_storage<R> {R(std::forward<S>(res)), true},
            true
        };
      } else {
        return parent;
      }
    }
  };

  template <typename T>
  struct match_case_handler<T, void> {
    match<T, void>& parent;
    bool            selected;

    template <typename S>
      match<T, S> operator()(S&& res) {
      if (selected) {
        return match<T, S> {
          parent.t, match_result_storage<S> {std::forward<S>(res), true},
            true
        };
      } else {
        if constexpr (std::is_default_constructible_v<S>) {
          return match<T, S> {parent.t};
        } else {
          return match<T, S> {
            parent.t, match_result_storage<S> {std::forward<S>(res), false},
              false
          };
        }
      }
    }
  };
} // namespace fabric::detail
