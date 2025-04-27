DIMENSION_STYLE_SETTER(x)
DIMENSION_STYLE_SETTER(y)
DIMENSION_STYLE_SETTER(width)
DIMENSION_STYLE_SETTER(height)
FOUR_SIDED_SETTER(margin, dimension_t, { return value; })
FOUR_SIDED_DIMENSIONAL_OVERLOADS(margin)
FOUR_SIDED_SETTER(padding, dimension_t, { return value; })
FOUR_SIDED_DIMENSIONAL_OVERLOADS(padding)

FOUR_SIDED_SETTER_POLY(border, vg::paint::type, vg::paint::solid, {
  return vg::paint::type::make<vg::paint::solid>(value);
})
FOUR_SIDED_SETTER_POLY(border, vg::paint::type, vg::paint::gradient::linear, {
  return vg::paint::type::make<vg::paint::gradient::linear>(value);
})
FOUR_SIDED_SETTER_POLY(border, vg::paint::type, vg::paint::gradient::radial, {
  return vg::paint::type::make<vg::paint::gradient::radial>(value);
})
FOUR_SIDED_SETTER(border_width, int, { return value; })
FOUR_CORNERED_SETTER(border_radius, int, { return value; })
FOUR_SIDED_SETTER(border_dasharray, std::valarray<double>, { return value; })

STYLE_SETTER(background, vg::paint::solid, {
  return vg::paint::type::make<vg::paint::solid>(value);
})
STYLE_SETTER(background, vg::paint::gradient::linear, {
  return vg::paint::type::make<vg::paint::gradient::linear>(value);
})
STYLE_SETTER(background, vg::paint::gradient::radial, {
  return vg::paint::type::make<vg::paint::gradient::radial>(value);
})

STYLE_SETTER(opacity, la::scalar, { return value; })
STYLE_SETTER(translate_x, la::scalar, { return value; })
STYLE_SETTER(translate_y, la::scalar, { return value; })
STYLE_SETTER_RETURN_TYPE translate(const la::vec<2>& value) {
  translate_x(value[0]);
  translate_y(value[1]);
  STYLE_SETTER_RETURN_EXPR
}
STYLE_SETTER_RETURN_TYPE translate(la::vec<2>&& value) {
  translate_x(value[0]);
  translate_y(value[1]);
  STYLE_SETTER_RETURN_EXPR
}
STYLE_SETTER(scale_x, la::scalar, { return value; })
STYLE_SETTER(scale_y, la::scalar, { return value; })
STYLE_SETTER_RETURN_TYPE scale(const la::vec<2>& value) {
  scale_x(value[0]);
  scale_y(value[1]);
  STYLE_SETTER_RETURN_EXPR
}
STYLE_SETTER_RETURN_TYPE scale(la::vec<2>&& value) {
  scale_x(value[0]);
  scale_y(value[1]);
  STYLE_SETTER_RETURN_EXPR
}
STYLE_SETTER(rotation, angle_type, { return value; })

STYLE_SETTER(overflow_x, cydui::overflow_e, { return value; })
STYLE_SETTER(overflow_y, cydui::overflow_e, { return value; })
