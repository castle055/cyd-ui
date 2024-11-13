/*! \file  _types.cppm
 *! \brief 
 *!
 */

module;
#include <quantify/unit_macros.h>

export module cydui.dimensions:types;

export import quantify;
using namespace quantify;


constexpr double DPI = 92.0; // TODO - find a runtime way of getting this value

export namespace cyd::ui::dimensions {
 SCALE(screen) {
  UNIT(pixel, "px", 1, 1)
 }

 using screen_measure = quantity_t<screen::pixel, double>;
}
SCALE_CONVERSION(cyd::ui::dimensions::screen::scale, quantify::distance::scale)
  SCALE_FORWARD_CONVERSION((it / ((100.0 / 2.54) * DPI))) SCALE_BACKWARD_CONVERSION(it * ((100.0 / 2.54) * DPI))

export namespace cyd::ui::dimensions {
 template<typename T>
 class context;
 template<typename T>
 class dimension;
 template<typename T>
 class expression;

 template <typename T>
 struct parameter {
  std::string name{};

  bool operator==(const parameter& other) const {
    return name == other.name;
  }
 };

 template <typename T>
 struct cycle_t;

 template<typename T>
 struct compute_result_t;

 template <typename ...>
 struct is_dimension: std::false_type {};

 template <typename... Args>
 struct is_dimension<dimension<Args...>>: std::true_type {};

 template <typename... Args>
 constexpr bool is_dimension_v = is_dimension<Args...>::value;

 template <typename ...>
 struct is_expression: std::false_type {};

 template <typename... Args>
 struct is_expression<expression<Args...>>: std::true_type {};

 template <typename... Args>
 constexpr bool is_expression_v = is_expression<Args...>::value;
}

namespace cyd::ui::dimensions {
 template<typename T>
 class dimension_impl;

 struct dimensional_operators;
}

export constexpr cyd::ui::dimensions::screen_measure
operator""_px(long double value) noexcept {
  return {static_cast<double>(value)};
}

export constexpr cyd::ui::dimensions::screen_measure
operator""_px(unsigned long long value) noexcept {
  return {static_cast<double>(value)};
}

export constexpr cyd::ui::dimensions::parameter<cyd::ui::dimensions::screen_measure>
operator""_param(const char* str, unsigned long len) noexcept {
  return {std::string{str, len}};
}

export {
#define OPERATOR +
#include "operators_decl.inc"
#undef OPERATOR

#define OPERATOR -
#include "operators_decl.inc"
#undef OPERATOR

#define OPERATOR *
#include "operators_decl.inc"
#undef OPERATOR

#define OPERATOR /
#include "operators_decl.inc"
#undef OPERATOR
}