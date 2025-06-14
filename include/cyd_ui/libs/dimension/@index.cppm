

export module cydui.dimensions;

export import fabric.linalg;

export import :types;
export import :compute;
export import :api;
export import :impl;
export import :dimension;
export import :expression;

export namespace cydui {
  using dimension_t = dimensions::dimension<dimensions::screen_measure>;
  using expression_t = dimensions::expression<dimensions::screen_measure>;
  using dimension_parameter_t = dimensions::parameter<dimensions::screen_measure>;
  using dimension_ctx_t = dimensions::context<dimensions::screen_measure>;

  using la = with_precision<float>;
  using angle_type = quantify::Q<quantify::angle::degrees, la::scalar>;
}
