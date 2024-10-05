
//* for dimension-T

template <typename T, typename V>
requires (not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<T> operator OPERATOR (V value, const cyd::ui::dimensions::dimension<T>& dim) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(value, dim);
}

template <typename T>
cyd::ui::dimensions::expression<T> operator OPERATOR (const cyd::ui::dimensions::dimension<T>& dim1, const cyd::ui::dimensions::dimension<T>& dim2) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(dim1, dim2);
}

template <typename T, typename V>
requires (not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<T> operator OPERATOR (const cyd::ui::dimensions::dimension<T>& dim, V value) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(dim, value);
}

//* for expression-T
template <typename T, typename V>
requires (not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<T> operator OPERATOR (V value, const cyd::ui::dimensions::expression<T>& expr) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(value, expr);
}

template <typename T, typename V>
requires (not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<T> operator OPERATOR (V value, cyd::ui::dimensions::expression<T>&& expr) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(value, std::forward<cyd::ui::dimensions::expression<T>&&>(expr));
}

template <typename T>
cyd::ui::dimensions::expression<T> operator OPERATOR (const cyd::ui::dimensions::expression<T>& expr1, const cyd::ui::dimensions::expression<T>& expr2) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(expr1, expr2);
}

template <typename T>
cyd::ui::dimensions::expression<T> operator OPERATOR (cyd::ui::dimensions::expression<T>&& expr1, const cyd::ui::dimensions::expression<T>& expr2) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(std::forward<cyd::ui::dimensions::expression<T>&&>(expr1), expr2);
}

template <typename T, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<T>
operator OPERATOR(const cyd::ui::dimensions::expression<T>& expr, V value) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(expr, value);
}

//* for expression-dimension
template <typename T>
cyd::ui::dimensions::expression<T> operator OPERATOR(
  const cyd::ui::dimensions::dimension<T>& dim, const cyd::ui::dimensions::expression<T>& expr
) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(dim, expr);
}

template <typename T>
cyd::ui::dimensions::expression<T> operator OPERATOR(
  const cyd::ui::dimensions::expression<T>& expr, const cyd::ui::dimensions::dimension<T>& dim
) {
 return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(expr, dim);
}


// Parameters
template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::dimension<S>& lhs, const cyd::ui::dimensions::parameter<S>& rhs) {
  return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::parameter<S>& lhs, const cyd::ui::dimensions::dimension<S>& rhs) {
  return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::expression<S>& lhs, const cyd::ui::dimensions::parameter<S>& rhs) {
  return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::parameter<S>& lhs, const cyd::ui::dimensions::expression<S>& rhs) {
  return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(V lhs, const cyd::ui::dimensions::parameter<S>& rhs) {
  return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::parameter<S>& lhs, V rhs) {
  return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::parameter<S>& lhs, const cyd::ui::dimensions::parameter<S>& rhs) {
  return cyd::ui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}
