/*! \file  auto_traits.cppm
 *! \brief
 *!
 */

export module fabric.match:auto_traits;
export import :types;

export import :traits.arithmetic;

import std;
import fabric.logging;
import fabric.exception;

namespace fabric::detail {
  template <typename T>
  concept less_comparable = requires(const T& t1, const T& t2) {
    { t1 < t2 } -> std::convertible_to<bool>;
    { t2 < t1 } -> std::convertible_to<bool>;
  };
  template <typename T>
  concept less_eq_comparable = requires(const T& t1, const T& t2) {
    { t1 <= t2 } -> std::convertible_to<bool>;
    { t2 <= t1 } -> std::convertible_to<bool>;
  };
  template <typename T>
  concept greater_comparable = requires(const T& t1, const T& t2) {
    { t1 > t2 } -> std::convertible_to<bool>;
    { t2 > t1 } -> std::convertible_to<bool>;
  };
  template <typename T>
  concept greater_eq_comparable = requires(const T& t1, const T& t2) {
    { t1 >= t2 } -> std::convertible_to<bool>;
    { t2 >= t1 } -> std::convertible_to<bool>;
  };

  template <typename T, typename R>
  struct auto_traits: auto_trait<std::equality_comparable<T>, match_traits::eq_match<T, R>>,
                      auto_trait<less_comparable<T>, match_traits::less_match<T, R>>,
                      auto_trait<less_eq_comparable<T>, match_traits::at_most_match<T, R>>,
                      auto_trait<greater_comparable<T>, match_traits::greater_match<T, R>>,
                      auto_trait<greater_eq_comparable<T>, match_traits::at_least_match<T, R>>,
                      auto_trait<std::totally_ordered<T>, match_traits::range_compare_match<T, R>>,
                      auto_trait<std::same_as<T, bool>, match_traits::bool_match<R>> {};
} // namespace fabric::detail
