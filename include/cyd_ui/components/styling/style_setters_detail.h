// Parameters:
//  - STYLE_SETTER_RETURN_TYPE
//  - STYLE_SETTER_RETURN_EXPR
//  - STYLE_SETTER_REF_CONSTRAINT
//  - STYLE_MAP_GETTER

#define STR(X) #X

#define STYLE_BASE_FIELD(NAME)                                                                     \
  refl::type_info::from<style::style_base_t>()                                                     \
    .field_by_offset(__builtin_offsetof(cydui::style::style_base_t, NAME))                         \
    .value()

#define FIELD_PATH(NAME)                                                                           \
  static const refl::field_path field_path = {                                                     \
    refl::type_info::from<cydui::style::style_base_t>(), STYLE_BASE_FIELD(NAME)                    \
  }

#define FIELD_PATH_STR(PATH_STR)                                                                   \
  static const refl::field_path field_path =                                                       \
    refl::field_path::from_string(refl::type_info::from<cydui::style::style_base_t>(), PATH_STR)   \
      .value()

#define STYLE_SETTER(NAME, TYPE, ...)                                                              \
  STYLE_SETTER_RETURN_TYPE NAME(const TYPE& value) STYLE_SETTER_REF_CONSTRAINT {                   \
    FIELD_PATH(NAME);                                                                              \
    auto val = [&] -> auto __VA_ARGS__();                                                          \
    STYLE_MAP_GETTER.set_field(field_path, refl::any::make(val));                                  \
    STYLE_SETTER_RETURN_EXPR;                                                                      \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE NAME(TYPE&& value) STYLE_SETTER_REF_CONSTRAINT {                        \
    FIELD_PATH(NAME);                                                                              \
    auto val = [&] -> auto __VA_ARGS__();                                                          \
    STYLE_MAP_GETTER.set_field(field_path, refl::any::make(val));                                  \
    STYLE_SETTER_RETURN_EXPR;                                                                      \
  }

#define INNER_STYLE_SETTER(NAME, TYPE, INNER_NAME, INNER_TYPE, ...)                                \
  STYLE_SETTER_RETURN_TYPE NAME##_##INNER_NAME(const INNER_TYPE& value)                            \
    STYLE_SETTER_REF_CONSTRAINT {                                                                  \
    FIELD_PATH_STR(#NAME "." #INNER_NAME);                                                         \
    auto val = [&] -> auto __VA_ARGS__();                                                          \
    STYLE_MAP_GETTER.set_field(field_path, refl::any::make(val));                                  \
    STYLE_SETTER_RETURN_EXPR;                                                                      \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE NAME##_##INNER_NAME(INNER_TYPE&& value) STYLE_SETTER_REF_CONSTRAINT {   \
    FIELD_PATH_STR(#NAME "." #INNER_NAME);                                                         \
    auto val = [&] -> auto __VA_ARGS__();                                                          \
    STYLE_MAP_GETTER.set_field(field_path, refl::any::make(val));                                  \
    STYLE_SETTER_RETURN_EXPR;                                                                      \
  }

#define FOUR_SIDED_SETTER(NAME, TYPE, ...)                                                         \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, top, TYPE, __VA_ARGS__)                      \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, right, TYPE, __VA_ARGS__)                    \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, bottom, TYPE, __VA_ARGS__)                   \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, left, TYPE, __VA_ARGS__)                     \
  STYLE_SETTER_RETURN_TYPE NAME(const TYPE& value) STYLE_SETTER_REF_CONSTRAINT {                   \
    NAME##_top(value);                                                                             \
    NAME##_right(value);                                                                           \
    NAME##_bottom(value);                                                                          \
    NAME##_left(value);                                                                            \
    STYLE_SETTER_RETURN_EXPR;                                                                      \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE NAME(TYPE&& value) STYLE_SETTER_REF_CONSTRAINT {                        \
    NAME##_top(value);                                                                             \
    NAME##_right(value);                                                                           \
    NAME##_bottom(value);                                                                          \
    NAME##_left(value);                                                                            \
    STYLE_SETTER_RETURN_EXPR;                                                                      \
  }
#define FOUR_SIDED_SETTER_POLY(NAME, TYPE, DTYPE, ...)                                             \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, top, DTYPE, __VA_ARGS__)                     \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, right, DTYPE, __VA_ARGS__)                   \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, bottom, DTYPE, __VA_ARGS__)                  \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, left, DTYPE, __VA_ARGS__)                    \
  STYLE_SETTER_RETURN_TYPE NAME(const DTYPE& value) STYLE_SETTER_REF_CONSTRAINT {                  \
    NAME##_top(value);                                                                             \
    NAME##_right(value);                                                                           \
    NAME##_bottom(value);                                                                          \
    NAME##_left(value);                                                                            \
    STYLE_SETTER_RETURN_EXPR;                                                                      \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE NAME(DTYPE&& value) STYLE_SETTER_REF_CONSTRAINT {                       \
    NAME##_top(value);                                                                             \
    NAME##_right(value);                                                                           \
    NAME##_bottom(value);                                                                          \
    NAME##_left(value);                                                                            \
    STYLE_SETTER_RETURN_EXPR;                                                                      \
  }
#define FOUR_SIDED_DIMENSIONAL_OVERLOADS(NAME)                                                     \
  DIMENSIONAL_OVERLOADS(NAME##_top)                                                                \
  DIMENSIONAL_OVERLOADS(NAME##_right)                                                              \
  DIMENSIONAL_OVERLOADS(NAME##_bottom)                                                             \
  DIMENSIONAL_OVERLOADS(NAME##_left)                                                               \
  DIMENSIONAL_OVERLOADS(NAME)

#define FOUR_CORNERED_SETTER(NAME, TYPE, ...)                                                      \
  INNER_STYLE_SETTER(NAME, four_cornered_property<TYPE>, top_left, TYPE, __VA_ARGS__)              \
  INNER_STYLE_SETTER(NAME, four_cornered_property<TYPE>, top_right, TYPE, __VA_ARGS__)             \
  INNER_STYLE_SETTER(NAME, four_cornered_property<TYPE>, bottom_right, TYPE, __VA_ARGS__)          \
  INNER_STYLE_SETTER(NAME, four_cornered_property<TYPE>, bottom_left, TYPE, __VA_ARGS__)           \
  STYLE_SETTER_RETURN_TYPE NAME(const TYPE& value) STYLE_SETTER_REF_CONSTRAINT {                   \
    NAME##_top_left(value);                                                                        \
    NAME##_top_right(value);                                                                       \
    NAME##_bottom_right(value);                                                                    \
    NAME##_bottom_left(value);                                                                     \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE NAME(TYPE&& value) STYLE_SETTER_REF_CONSTRAINT {                        \
    NAME##_top_left(value);                                                                        \
    NAME##_top_right(value);                                                                       \
    NAME##_bottom_right(value);                                                                    \
    NAME##_bottom_left(value);                                                                     \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }

#define DIMENSIONAL_OVERLOADS(DIMENSION_NAME)                                                      \
  STYLE_SETTER_RETURN_TYPE DIMENSION_NAME(const dimension_parameter_t& value)                      \
    STYLE_SETTER_REF_CONSTRAINT {                                                                  \
    DIMENSION_NAME(expression_t{dimensions::screen_measure{0} + value});                           \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE DIMENSION_NAME(float value) STYLE_SETTER_REF_CONSTRAINT {               \
    DIMENSION_NAME(expression_t{dimensions::screen_measure{value}});                               \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE DIMENSION_NAME(double value) STYLE_SETTER_REF_CONSTRAINT {              \
    DIMENSION_NAME(expression_t{dimensions::screen_measure{value}});                               \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE DIMENSION_NAME(int value) STYLE_SETTER_REF_CONSTRAINT {                 \
    DIMENSION_NAME(expression_t{dimensions::screen_measure{static_cast<double>(value)}});          \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }                                                                                                \
  STYLE_SETTER_RETURN_TYPE DIMENSION_NAME(dimension_t& value) STYLE_SETTER_REF_CONSTRAINT {        \
    DIMENSION_NAME(expression_t{dimensions::screen_measure{0} + value});                           \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }                                                                                                \
  template <typename DimType>                                                                      \
  STYLE_SETTER_RETURN_TYPE DIMENSION_NAME(const DimType& value) STYLE_SETTER_REF_CONSTRAINT {      \
    DIMENSION_NAME(expression_t{value});                                                           \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }                                                                                                \
  template <typename DimType>                                                                      \
  STYLE_SETTER_RETURN_TYPE DIMENSION_NAME(DimType&& value) STYLE_SETTER_REF_CONSTRAINT {           \
    DIMENSION_NAME(expression_t{std::forward<DimType>(value)});                                    \
    STYLE_SETTER_RETURN_EXPR                                                                       \
  }

#define FOUR_CORNERED_DIMENSIONAL_OVERLOADS(NAME)                                                  \
  DIMENSIONAL_OVERLOADS(NAME##_top_left)                                                           \
  DIMENSIONAL_OVERLOADS(NAME##_top_right)                                                          \
  DIMENSIONAL_OVERLOADS(NAME##_bottom_right)                                                       \
  DIMENSIONAL_OVERLOADS(NAME##_bottom_left)                                                        \
  DIMENSIONAL_OVERLOADS(NAME)

#define DIMENSION_STYLE_SETTER(NAME)                                                               \
  STYLE_SETTER(NAME, expression_t, { return value; })                                              \
  DIMENSIONAL_OVERLOADS(NAME)
\
