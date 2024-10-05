template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(V, const cyd::ui::dimensions::dimension<S>&);

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::dimension<S>&, const cyd::ui::dimensions::dimension<S>&);

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::dimension<S>&, V);

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(V, const cyd::ui::dimensions::expression<S>&);

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(V, const cyd::ui::dimensions::expression<S>&&);

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::expression<S>&, const cyd::ui::dimensions::expression<S>&);

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::expression<S>&&, const cyd::ui::dimensions::expression<S>&);

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::expression<S>&, V);

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::dimension<S>&, const cyd::ui::dimensions::expression<S>&);

template <typename S>
cyd::ui::dimensions::expression<S>
operator OPERATOR(const cyd::ui::dimensions::expression<S>&, const cyd::ui::dimensions::dimension<S>&);


// Parameters
template <typename S>
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::dimension<S>&, const cyd::ui::dimensions::parameter<S>&);

template <typename S>
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::parameter<S>&, const cyd::ui::dimensions::dimension<S>&);

template <typename S>
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::expression<S>&, const cyd::ui::dimensions::parameter<S>&);

template <typename S>
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::parameter<S>&, const cyd::ui::dimensions::expression<S>&);

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(V, const cyd::ui::dimensions::parameter<S>&);

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::parameter<S>&, V);

template <typename S>
cyd::ui::dimensions::expression<S> operator OPERATOR(const cyd::ui::dimensions::parameter<S>&, const cyd::ui::dimensions::parameter<S>&);