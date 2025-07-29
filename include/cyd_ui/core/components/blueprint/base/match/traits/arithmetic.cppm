/*! \file  arithmetic.cppm
 *! \brief
 *!
 */

export module fabric.match:traits.arithmetic;
export import :trait_base;

import std;
import fabric.logging;
import fabric.exception;

namespace fabric::match_traits {
  template <std::equality_comparable T, typename R>
  struct eq_match: trait_base<T, R, eq_match<T, R>> {
    auto eq(const T& t_) {
      return this->make_case(this->get_value() == t_);
    }

    auto eq(const std::equality_comparable_with<T> auto&... t_) {
      return this->make_case(((this->get_value() == t_) or ...));
    }

    auto neq(const std::equality_comparable_with<T> auto&... t_) {
      return this->make_case(((this->get_value() != t_) and ...));
    }
  };

  template <typename T, typename R>
  struct less_match: trait_base<T, R, less_match<T, R>> {
    auto less_than(const auto& t_) {
      return this->make_case(this->get_value() < t_);
    }
  };

  template <typename T, typename R>
  struct at_most_match: trait_base<T, R, at_most_match<T, R>> {
    auto at_most(const auto& t_) {
      return this->make_case(this->get_value() <= t_);
    }
  };

  template <typename T, typename R>
  struct greater_match: trait_base<T, R, greater_match<T, R>> {
    auto greater_than(const auto& t_) {
      return this->make_case(this->get_value() > t_);
    }
  };

  template <typename T, typename R>
  struct at_least_match: trait_base<T, R, at_least_match<T, R>> {
    auto at_least(const auto& t_) {
      return this->make_case(this->get_value() >= t_);
    }
  };

  template <typename T, typename R>
  struct range_compare_match: trait_base<T, R, range_compare_match<T, R>> {
    auto in_range(const T& t1, const T& t2) {
      return this->make_case((this->get_value() >= t1) and (this->get_value() <= t2));
    }

    auto not_in_range(const T& t1, const T& t2) {
      return this->make_case((this->get_value() < t1) or (this->get_value() > t2));
    }

    auto in_range(const std::totally_ordered_with<T> auto& t1, const std::totally_ordered_with<T> auto& t2) {
      return this->make_case((this->get_value() >= t1) and (this->get_value() <= t2));
    }

    auto not_in_range(const std::totally_ordered_with<T> auto& t1, const std::totally_ordered_with<T> auto& t2) {
      return this->make_case((this->get_value() < t1) or (this->get_value() > t2));
    }
  };

  template <typename R>
  struct bool_match: trait_base<bool, R, bool_match<R>> {
    auto if_true() {
      return this->make_case(this->get_value());
    }
    auto if_false() {
      return this->make_case(not this->get_value());
    }
  };
} // namespace fabric::match_traits
