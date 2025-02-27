
#define STYLE_BASE_FIELD(NAME)                                                                     \
  refl::type_info::from<style_base_t>().field_by_offset(offsetof(style_base_t, NAME)).value()

#define END_SETTER(VALUE)                                                                          \
  this->get_style_data().set_base_field_override(field, refl::any::make(VALUE));                   \
  return *static_cast<T*>(this)

#define STYLE_SETTER(NAME, TYPE, ...)                                                              \
  T& NAME(const TYPE& value) {                                                                     \
    static const auto* field = STYLE_BASE_FIELD(NAME);                                             \
    auto               val   = [&] -> auto __VA_ARGS__();                                          \
    this->get_style_data().set_base_field_override(field, refl::any::make(val));                   \
    return *static_cast<T*>(this);                                                                 \
  }                                                                                                \
  T& NAME(TYPE&& value) {                                                                          \
    static const auto* field = STYLE_BASE_FIELD(NAME);                                             \
    auto               val   = [&] -> auto __VA_ARGS__();                                          \
    this->get_style_data().set_base_field_override(field, refl::any::make(val));                   \
    return *static_cast<T*>(this);                                                                 \
  }

#define INNER_STYLE_SETTER(NAME, TYPE, INNER_NAME, INNER_TYPE, ...)                                \
  T& NAME##_##INNER_NAME(const INNER_TYPE& value) {                                                \
    static const auto* field =                                                                     \
      refl::type_info::from<style_base_t>().field_by_offset(offsetof(style_base_t, NAME)).value(); \
    static const auto* inner_field =                                                               \
      field->type().field_by_offset(offsetof(TYPE, INNER_NAME)).value();                           \
    auto val = [&] -> auto __VA_ARGS__();                                                          \
    this->get_style_data().set_base_field_override({field, inner_field}, refl::any::make(val));    \
    return *static_cast<T*>(this);                                                                 \
  }                                                                                                \
  T& NAME##_##INNER_NAME(INNER_TYPE&& value) {                                                     \
    static const auto* field =                                                                     \
      refl::type_info::from<style_base_t>().field_by_offset(offsetof(style_base_t, NAME)).value(); \
    static const auto* inner_field =                                                               \
      field->type().field_by_offset(offsetof(TYPE, INNER_NAME)).value();                           \
    auto val = [&] -> auto __VA_ARGS__();                                                          \
    this->get_style_data().set_base_field_override({field, inner_field}, refl::any::make(val));    \
    return *static_cast<T*>(this);                                                                 \
  }

#define FOUR_SIDED_SETTER(NAME, TYPE, ...)                                                         \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, top, TYPE, __VA_ARGS__)                      \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, right, TYPE, __VA_ARGS__)                    \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, bottom, TYPE, __VA_ARGS__)                   \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, left, TYPE, __VA_ARGS__)                     \
  T& NAME(const TYPE& value) {                                                                     \
    NAME##_top(value);                                                                             \
    NAME##_right(value);                                                                           \
    NAME##_bottom(value);                                                                          \
    NAME##_left(value);                                                                            \
    return *static_cast<T*>(this);                                                                 \
  }                                                                                                \
  T& NAME(TYPE&& value) {                                                                          \
    NAME##_top(value);                                                                             \
    NAME##_right(value);                                                                           \
    NAME##_bottom(value);                                                                          \
    NAME##_left(value);                                                                            \
    return *static_cast<T*>(this);                                                                 \
  }
#define FOUR_SIDED_SETTER_POLY(NAME, TYPE, DTYPE, ...)                                             \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, top, DTYPE, __VA_ARGS__)                     \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, right, DTYPE, __VA_ARGS__)                   \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, bottom, DTYPE, __VA_ARGS__)                  \
  INNER_STYLE_SETTER(NAME, four_sided_property<TYPE>, left, DTYPE, __VA_ARGS__)                    \
  T& NAME(const DTYPE& value) {                                                                    \
    NAME##_top(value);                                                                             \
    NAME##_right(value);                                                                           \
    NAME##_bottom(value);                                                                          \
    NAME##_left(value);                                                                            \
    return *static_cast<T*>(this);                                                                 \
  }                                                                                                \
  T& NAME(DTYPE&& value) {                                                                         \
    NAME##_top(value);                                                                             \
    NAME##_right(value);                                                                           \
    NAME##_bottom(value);                                                                          \
    NAME##_left(value);                                                                            \
    return *static_cast<T*>(this);                                                                 \
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
  T& NAME(const TYPE& value) {                                                                     \
    NAME##_top_left(value);                                                                        \
    NAME##_top_right(value);                                                                       \
    NAME##_bottom_right(value);                                                                    \
    NAME##_bottom_left(value);                                                                     \
    return *static_cast<T*>(this);                                                                 \
  }                                                                                                \
  T& NAME(TYPE&& value) {                                                                          \
    NAME##_top_left(value);                                                                        \
    NAME##_top_right(value);                                                                       \
    NAME##_bottom_right(value);                                                                    \
    NAME##_bottom_left(value);                                                                     \
    return *static_cast<T*>(this);                                                                 \
  }

#define DIMENSIONAL_OVERLOADS(DIMENSION_NAME)                                                      \
  T& DIMENSION_NAME(const anchors::dimension_parameter_t value) {                                  \
    return DIMENSION_NAME(dimension_t{dimensions::screen_measure{0} + value});                     \
  }                                                                                                \
  T& DIMENSION_NAME(float value) {                                                                 \
    return DIMENSION_NAME(dimension_t{dimensions::screen_measure{value}});                         \
  }                                                                                                \
  T& DIMENSION_NAME(double value) {                                                                \
    return DIMENSION_NAME(dimension_t{dimensions::screen_measure{value}});                         \
  }                                                                                                \
  T& DIMENSION_NAME(int value) {                                                                   \
    return DIMENSION_NAME(dimension_t{dimensions::screen_measure{static_cast<double>(value)}});    \
  }                                                                                                \
  T& DIMENSION_NAME(dimension_t& value) {                                                          \
    return DIMENSION_NAME(dimension_t{dimensions::screen_measure{0} + value});                     \
  }                                                                                                \
  template <typename DimType>                                                                      \
  T& DIMENSION_NAME(const DimType& value) {                                                        \
    return DIMENSION_NAME(dimension_t{value});                                                     \
  }                                                                                                \
  template <typename DimType>                                                                      \
  T& DIMENSION_NAME(DimType&& value) {                                                             \
    return DIMENSION_NAME(dimension_t{std::forward<DimType>(value)});                              \
  }

#define FOUR_CORNERED_DIMENSIONAL_OVERLOADS(NAME)                                                  \
  DIMENSIONAL_OVERLOADS(NAME##_top_left)                                                           \
  DIMENSIONAL_OVERLOADS(NAME##_top_right)                                                          \
  DIMENSIONAL_OVERLOADS(NAME##_bottom_right)                                                       \
  DIMENSIONAL_OVERLOADS(NAME##_bottom_left)                                                        \
  DIMENSIONAL_OVERLOADS(NAME)

#define DIMENSION_STYLE_SETTER(NAME)                                                               \
  STYLE_SETTER(NAME, dimension_t, { return value; })                                               \
  DIMENSIONAL_OVERLOADS(NAME) \
