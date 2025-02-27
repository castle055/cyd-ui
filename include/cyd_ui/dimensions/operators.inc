
//* for dimension-T

template <typename T, typename V>
requires (not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<T> operator OPERATOR (V value, const cydui::dimensions::dimension<T>& dim) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(value, dim);
}

template <typename T>
cydui::dimensions::expression<T> operator OPERATOR (const cydui::dimensions::dimension<T>& dim1, const cydui::dimensions::dimension<T>& dim2) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(dim1, dim2);
}

template <typename T, typename V>
requires (not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<T> operator OPERATOR (const cydui::dimensions::dimension<T>& dim, V value) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(dim, value);
}

//* for expression-T
template <typename T, typename V>
requires (not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<T> operator OPERATOR (V value, const cydui::dimensions::expression<T>& expr) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(value, expr);
}

template <typename T, typename V>
requires (not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<T> operator OPERATOR (V value, cydui::dimensions::expression<T>&& expr) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(value, std::forward<cydui::dimensions::expression<T>&&>(expr));
}

template <typename T>
cydui::dimensions::expression<T> operator OPERATOR (const cydui::dimensions::expression<T>& expr1, const cydui::dimensions::expression<T>& expr2) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(expr1, expr2);
}

template <typename T>
cydui::dimensions::expression<T> operator OPERATOR (cydui::dimensions::expression<T>&& expr1, const cydui::dimensions::expression<T>& expr2) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(std::forward<cydui::dimensions::expression<T>&&>(expr1), expr2);
}

template <typename T, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<T>
operator OPERATOR(const cydui::dimensions::expression<T>& expr, V value) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(expr, value);
}

//* for expression-dimension
template <typename T>
cydui::dimensions::expression<T> operator OPERATOR(
  const cydui::dimensions::dimension<T>& dim, const cydui::dimensions::expression<T>& expr
) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(dim, expr);
}

template <typename T>
cydui::dimensions::expression<T> operator OPERATOR(
  const cydui::dimensions::expression<T>& expr, const cydui::dimensions::dimension<T>& dim
) {
 return cydui::dimensions::dimensional_operators::OP_FUNCTION(expr, dim);
}


// Parameters
template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::dimension<S>& lhs, const cydui::dimensions::parameter<S>& rhs) {
  return cydui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::parameter<S>& lhs, const cydui::dimensions::dimension<S>& rhs) {
  return cydui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::expression<S>& lhs, const cydui::dimensions::parameter<S>& rhs) {
  return cydui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::parameter<S>& lhs, const cydui::dimensions::expression<S>& rhs) {
  return cydui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(V lhs, const cydui::dimensions::parameter<S>& rhs) {
  return cydui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::parameter<S>& lhs, V rhs) {
  return cydui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::parameter<S>& lhs, const cydui::dimensions::parameter<S>& rhs) {
  return cydui::dimensions::dimensional_operators::OP_FUNCTION(lhs, rhs);
}
