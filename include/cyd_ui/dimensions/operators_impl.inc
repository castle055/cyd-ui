
//* for dimension-T

template <typename T, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
static cyd::ui::dimensions::expression<T>
OPERATOR(V value, const cyd::ui::dimensions::dimension<T>& dim) {
  using namespace cyd::ui::dimensions;
  expression<T> expr{};
  expr.add_dependency(dim.impl_);

  auto const_value         = expr.make_node(expression<T>::node_t::CONSTANT);
  const_value->const_value = value;

  auto dim_       = expr.make_node(expression<T>::node_t::DIMENSION);
  dim_->dimension = dim.impl_;

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(const_value);
  node->children.push_back(dim_);

  expr.tree_ = node;
  return expr;
}

template <typename T>
static cyd::ui::dimensions::expression<T> OPERATOR(
  const cyd::ui::dimensions::dimension<T>& dim1, const cyd::ui::dimensions::dimension<T>& dim2
) {
  using namespace cyd::ui::dimensions;
  expression<T> expr{};
  expr.add_dependency(dim1.impl_);
  expr.add_dependency(dim2.impl_);

  auto dim1_       = expr.make_node(expression<T>::node_t::DIMENSION);
  dim1_->dimension = dim1.impl_;

  auto dim2_       = expr.make_node(expression<T>::node_t::DIMENSION);
  dim2_->dimension = dim2.impl_;

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(dim1_);
  node->children.push_back(dim2_);

  expr.tree_ = node;
  return expr;
}

template <typename T, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
static cyd::ui::dimensions::expression<T>
OPERATOR(const cyd::ui::dimensions::dimension<T>& dim, V value) {
  using namespace cyd::ui::dimensions;
  expression<T> expr{};
  expr.add_dependency(dim.impl_);

  auto dim_       = expr.make_node(expression<T>::node_t::DIMENSION);
  dim_->dimension = dim.impl_;

  auto const_value         = expr.make_node(expression<T>::node_t::CONSTANT);
  const_value->const_value = value;

  auto node      = expr.make_node(expression<T>::node_t::ENUM);
  node->children = {};
  node->children.push_back(dim_);
  node->children.push_back(const_value);

  expr.tree_ = node;
  return expr;
}

//* for expression-T
template <typename T, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
static cyd::ui::dimensions::expression<T>
OPERATOR(V value, const cyd::ui::dimensions::expression<T>& expr) {
  using namespace cyd::ui::dimensions;
  expression<T> expr_{};

  auto const_value         = expr_.make_node(expression<T>::node_t::CONSTANT);
  const_value->const_value = value;

  auto node = expr_.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(const_value);
  node->children.push_back(expr_.tree_);

  expr_.tree_ = node;
  expr_.dependencies_.insert_range(expr.dependencies_.begin(), expr.dependencies_.end());
  return expr_;
}

template <typename T, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
static cyd::ui::dimensions::expression<T>
OPERATOR(V value, cyd::ui::dimensions::expression<T>&& expr) {
  using namespace cyd::ui::dimensions;
  auto const_value         = expr.make_node(expression<T>::node_t::CONSTANT);
  const_value->const_value = value;

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(const_value);
  node->children.push_back(expr.tree_);

  expr.tree_ = node;
  return expr;
}

template <typename T>
static cyd::ui::dimensions::expression<T> OPERATOR(
  const cyd::ui::dimensions::expression<T>& expr1, const cyd::ui::dimensions::expression<T>& expr2
) {
  using namespace cyd::ui::dimensions;
  expression<T> expr{};

  auto node      = expr.make_node(expression<T>::node_t::ENUM);
  node->children = {};
  node->children.push_back(expr1.tree_);
  node->children.push_back(expr2.tree_);

  expr.tree_ = node;
  expr.dependencies_.insert_range(expr1.dependencies_.begin(), expr1.dependencies_.end());
  expr.dependencies_.insert_range(expr2.dependencies_.begin(), expr2.dependencies_.end());
  return expr;
}

template <typename T>
static cyd::ui::dimensions::expression<T> OPERATOR(
  cyd::ui::dimensions::expression<T>&& expr1, const cyd::ui::dimensions::expression<T>& expr2
) {
  using namespace cyd::ui::dimensions;
  auto node = expr1.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(expr1.tree_);
  node->children.push_back(expr2.tree_);

  expr1.tree_ = node;
  expr1.dependencies_.insert(expr2.dependencies_.begin(), expr2.dependencies_.end());
  return expr1;
}

template <typename T, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
static cyd::ui::dimensions::expression<T>
OPERATOR(const cyd::ui::dimensions::expression<T>& expr1, V value) {
  using namespace cyd::ui::dimensions;
  expression<T> expr{};

  auto const_value         = expr.make_node(expression<T>::node_t::CONSTANT);
  const_value->const_value = value;

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(expr1.tree_);
  node->children.push_back(const_value);

  expr.tree_ = node;
  return expr;
}

//* for expression-dimension
template <typename T>
static cyd::ui::dimensions::expression<T> OPERATOR(
  const cyd::ui::dimensions::dimension<T>& dim, const cyd::ui::dimensions::expression<T>& expr1
) {
  using namespace cyd::ui::dimensions;
  expression<T> expr{};

  auto dim_       = expr.make_node(expression<T>::node_t::DIMENSION);
  dim_->dimension = dim.impl_;
  expr.add_dependency(dim.impl_);

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(dim_);
  node->children.push_back(expr1.tree_);

  expr.tree_ = node;
  expr.dependencies_.insert(expr1.dependencies_.begin(), expr1.dependencies_.end());
  return expr;
}

template <typename T>
static cyd::ui::dimensions::expression<T> OPERATOR(
  const cyd::ui::dimensions::expression<T>& expr1, const cyd::ui::dimensions::dimension<T>& dim
) {
  using namespace cyd::ui::dimensions;
  expression<T> expr{};

  auto dim_       = expr.make_node(expression<T>::node_t::DIMENSION);
  dim_->dimension = dim.impl_;
  expr.add_dependency(dim.impl_);

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(expr1.tree_);
  node->children.push_back(dim_);

  expr.tree_ = node;
  expr.dependencies_.insert(expr1.dependencies_.begin(), expr1.dependencies_.end());
  return expr;
}


// Parameters
template <typename S>
static cyd::ui::dimensions::expression<S> OPERATOR(
  const cyd::ui::dimensions::dimension<S>& lhs, const cyd::ui::dimensions::parameter<S>& rhs
) {
  using namespace cyd::ui::dimensions;
  expression<S> expr{};

  expr.add_dependency(lhs.impl_);

  auto dim_node       = expr.make_node(expression<S>::node_t::DIMENSION);
  dim_node->dimension = lhs.impl_;

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(dim_node);
  node->children.push_back(param_node);

  expr.tree_ = node;
  return expr;
}

template <typename S>
static cyd::ui::dimensions::expression<S> OPERATOR(
  const cyd::ui::dimensions::parameter<S>& lhs, const cyd::ui::dimensions::dimension<S>& rhs
) {
  using namespace cyd::ui::dimensions;
  expression<S> expr{};

  expr.add_dependency(rhs.impl_);

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = lhs;

  auto dim_node       = expr.make_node(expression<S>::node_t::DIMENSION);
  dim_node->dimension = rhs.impl_;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(param_node);
  node->children.push_back(dim_node);

  expr.tree_ = node;
  return expr;
}

template <typename S>
static cyd::ui::dimensions::expression<S> OPERATOR(
  const cyd::ui::dimensions::expression<S>& lhs, const cyd::ui::dimensions::parameter<S>& rhs
) {
  using namespace cyd::ui::dimensions;
  expression<S> expr{};

  expr.dependencies_.insert(lhs.dependencies_.begin(), lhs.dependencies_.end());

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(lhs.tree_);
  node->children.push_back(param_node);

  expr.tree_ = node;
  return expr;
}

template <typename S>
static cyd::ui::dimensions::expression<S> OPERATOR(
  const cyd::ui::dimensions::parameter<S>& lhs, const cyd::ui::dimensions::expression<S>& rhs
) {
  using namespace cyd::ui::dimensions;
  expression<S> expr{};

  expr.dependencies_.insert_range(rhs.dependencies_.begin(), rhs.dependencies_.end());

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = lhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(param_node);
  node->children.push_back(rhs.tree_);

  expr.tree_ = node;
  return expr;
}

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
static cyd::ui::dimensions::expression<S>
OPERATOR(V lhs, const cyd::ui::dimensions::parameter<S>& rhs) {
  using namespace cyd::ui::dimensions;
  expression<S> expr{};

  auto const_value         = expr.make_node(expression<S>::node_t::CONSTANT);
  const_value->const_value = lhs;

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(const_value);
  node->children.push_back(param_node);

  expr.tree_ = node;
  return expr;
}

template <typename S, typename V>
  requires(not cyd::ui::dimensions::is_dimension_v<V> && not cyd::ui::dimensions::is_expression_v<V>)
static cyd::ui::dimensions::expression<S>
OPERATOR(const cyd::ui::dimensions::parameter<S>& lhs, V rhs) {
  using namespace cyd::ui::dimensions;
  expression<S> expr{};

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = lhs;

  auto const_value         = expr.make_node(expression<S>::node_t::CONSTANT);
  const_value->const_value = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(param_node);
  node->children.push_back(const_value);

  expr.tree_ = node;
  return expr;
}

template <typename S>
static cyd::ui::dimensions::expression<S> OPERATOR(
  const cyd::ui::dimensions::parameter<S>& lhs, const cyd::ui::dimensions::parameter<S>& rhs
) {
  using namespace cyd::ui::dimensions;
  expression<S> expr{};

  auto param1_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param1_node->parameter = lhs;

  auto param2_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param2_node->parameter = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(param1_node);
  node->children.push_back(param2_node);

  expr.tree_ = node;
  return expr;
}
