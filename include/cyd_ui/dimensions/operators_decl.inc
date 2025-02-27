template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(V, const cydui::dimensions::dimension<S>&);

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::dimension<S>&, const cydui::dimensions::dimension<S>&);

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::dimension<S>&, V);

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(V, const cydui::dimensions::expression<S>&);

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(V, const cydui::dimensions::expression<S>&&);

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::expression<S>&, const cydui::dimensions::expression<S>&);

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::expression<S>&&, const cydui::dimensions::expression<S>&);

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::expression<S>&, V);

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::dimension<S>&, const cydui::dimensions::expression<S>&);

template <typename S>
cydui::dimensions::expression<S>
operator OPERATOR(const cydui::dimensions::expression<S>&, const cydui::dimensions::dimension<S>&);


// Parameters
template <typename S>
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::dimension<S>&, const cydui::dimensions::parameter<S>&);

template <typename S>
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::parameter<S>&, const cydui::dimensions::dimension<S>&);

template <typename S>
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::expression<S>&, const cydui::dimensions::parameter<S>&);

template <typename S>
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::parameter<S>&, const cydui::dimensions::expression<S>&);

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(V, const cydui::dimensions::parameter<S>&);

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::parameter<S>&, V);

template <typename S>
cydui::dimensions::expression<S> operator OPERATOR(const cydui::dimensions::parameter<S>&, const cydui::dimensions::parameter<S>&);