/*! \file  match.cppm
 *! \brief
 *!
 */

export module fabric.match;
export import :storage;
export import :case_handler;

export import :trait_base;
export import :auto_traits;

import std;
import fabric.logging;
import fabric.exception;

namespace fabric {
  export template <typename T, typename R>
  struct match: detail::match_storage<T, R>, detail::auto_traits<T, R>, match_type<T, R> {
    using value_type  = T;
    using result_type = R;

    explicit match(const T& condition)
        : detail::match_storage<
            T,
            R> {condition} {}

    template <typename U>
    explicit match(
      const T& t,
      U        default_)
        : detail::match_storage<
            T,
            R> {
            t,
            std::forward<R>(default_)} {}

    explicit match(
      const T&                        t,
      detail::match_result_storage<R> result_,
      bool                            shorted_)
        : detail::match_storage<
            T,
            R> {
            t,
            result_,
            shorted_} {}
  };

  export template <
    typename T,
    typename R>
  match(
    T,
    R)
    -> match<
      T,
      R>;

  export template <typename T>
  match(T) -> match<
    T,
    void>;

  export template <typename T>
  struct is_match_t: std::false_type {};

  export template <typename T, typename R>
  struct is_match_t<match<T, R>>: std::true_type {};

  export template <typename T>
  concept is_match = is_match_t<T>::value;
} // namespace fabric

struct some_obj {
  int a {1};

  bool operator==(const some_obj& other) const {
    return a == other.a;
  }

  bool operator<(const some_obj& other) const {
    return a < other.a;
  }
  bool operator>(const some_obj& other) const {
    return a > other.a;
  }
  bool operator>=(const some_obj& other) const {
    return a >= other.a;
  }
  bool operator<=(const some_obj& other) const {
    return a <= other.a;
  }
};

export void testststs() {
  int    a {1};
  double b {2};

  using asdf = fabric::match<bool, void>;

  auto r1 = *fabric::match {some_obj {123}, 0} //
               .in_range({1}, {100})(a)
               .not_in_range({1}, {100})(a)
               .eq({12})(a)
               .eq(some_obj {1})(a)
               .eq(some_obj {3})(b);
  LOG::print {WARN}("Result 1: {}", r1);

  auto r2 = *fabric::match {false, 0} //
               .eq(true)(a)
               .neq(true)(a)
               .eq(false)(b);
  LOG::print {WARN}("Result 2: {}", r2);

  auto r3 = *fabric::match {false, 123} //
               .eq(true)(a);
  LOG::print {WARN}("Result 3: {}", r3);

  auto r4 = *fabric::match {true, 123} //
               .eq(true)(a);
  LOG::print {WARN}("Result 4: {}", r4);

  auto& r5 = *fabric::match {false} //
                .eq(true)(a);
  a = 3;
  LOG::print {WARN}("Result 5: {}", r5);
}
