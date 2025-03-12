
//* for dimension-T

template <typename T, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
static cydui::dimensions::expression<T>
OPERATOR(V value, const cydui::dimensions::dimension<T>& dim) {
  using namespace cydui::dimensions;
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
static cydui::dimensions::expression<T> OPERATOR(
  const cydui::dimensions::dimension<T>& dim1, const cydui::dimensions::dimension<T>& dim2
) {
  using namespace cydui::dimensions;
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
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
static cydui::dimensions::expression<T>
OPERATOR(const cydui::dimensions::dimension<T>& dim, V value) {
  using namespace cydui::dimensions;
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
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
static cydui::dimensions::expression<T>
OPERATOR(V value, const cydui::dimensions::expression<T>& expr) {
  using namespace cydui::dimensions;
  expression<T> expr_{};

  auto const_value         = expr_.make_node(expression<T>::node_t::CONSTANT);
  const_value->const_value = value;

  auto node = expr_.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(const_value);
  node->children.push_back(expr_.tree_);

  expr_.tree_ = node;
  expr_.dependencies_.insert(expr.dependencies_.begin(), expr.dependencies_.end());
  expr_.parameters_.insert(expr.parameters_.begin(), expr.parameters_.end());
  return expr_;
}

template <typename T, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
static cydui::dimensions::expression<T>
OPERATOR(V value, cydui::dimensions::expression<T>&& expr) {
  using namespace cydui::dimensions;
  auto const_value         = expr.make_node(expression<T>::node_t::CONSTANT);
  const_value->const_value = value;

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(const_value);
  node->children.push_back(expr.tree_);

  expr.tree_ = node;
  return expr;
}

template <typename T>
static cydui::dimensions::expression<T> OPERATOR(
  const cydui::dimensions::expression<T>& expr1, const cydui::dimensions::expression<T>& expr2
) {
  using namespace cydui::dimensions;
  expression<T> expr{};

  auto node      = expr.make_node(expression<T>::node_t::ENUM);
  node->children = {};
  node->children.push_back(expr1.tree_);
  node->children.push_back(expr2.tree_);

  expr.tree_ = node;
  expr.dependencies_.insert(expr1.dependencies_.begin(), expr1.dependencies_.end());
  expr.dependencies_.insert(expr2.dependencies_.begin(), expr2.dependencies_.end());
  expr.parameters_.insert(expr1.parameters_.begin(), expr1.parameters_.end());
  expr.parameters_.insert(expr2.parameters_.begin(), expr2.parameters_.end());
  return expr;
}

template <typename T>
static cydui::dimensions::expression<T> OPERATOR(
  cydui::dimensions::expression<T>&& expr1, const cydui::dimensions::expression<T>& expr2
) {
  using namespace cydui::dimensions;
  auto node = expr1.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(expr1.tree_);
  node->children.push_back(expr2.tree_);

  expr1.tree_ = node;
  expr1.dependencies_.insert(expr2.dependencies_.begin(), expr2.dependencies_.end());
  expr1.parameters_.insert(expr2.parameters_.begin(), expr2.parameters_.end());
  return expr1;
}

template <typename T, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
static cydui::dimensions::expression<T>
OPERATOR(const cydui::dimensions::expression<T>& expr1, V value) {
  using namespace cydui::dimensions;
  expression<T> expr{};

  auto const_value         = expr.make_node(expression<T>::node_t::CONSTANT);
  const_value->const_value = value;

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(expr1.tree_);
  node->children.push_back(const_value);

  expr.tree_ = node;
  expr.dependencies_.insert(expr1.dependencies_.begin(), expr1.dependencies_.end());
  expr.parameters_.insert(expr1.parameters_.begin(), expr1.parameters_.end());
  return expr;
}

//* for expression-dimension
template <typename T>
static cydui::dimensions::expression<T> OPERATOR(
  const cydui::dimensions::dimension<T>& dim, const cydui::dimensions::expression<T>& expr1
) {
  using namespace cydui::dimensions;
  expression<T> expr{};

  auto dim_       = expr.make_node(expression<T>::node_t::DIMENSION);
  dim_->dimension = dim.impl_;
  expr.add_dependency(dim.impl_);

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(dim_);
  node->children.push_back(expr1.tree_);

  expr.tree_ = node;
  expr.dependencies_.insert(expr1.dependencies_.begin(), expr1.dependencies_.end());
  expr.parameters_.insert(expr1.parameters_.begin(), expr1.parameters_.end());
  return expr;
}

template <typename T>
static cydui::dimensions::expression<T> OPERATOR(
  const cydui::dimensions::expression<T>& expr1, const cydui::dimensions::dimension<T>& dim
) {
  using namespace cydui::dimensions;
  expression<T> expr{};

  auto dim_       = expr.make_node(expression<T>::node_t::DIMENSION);
  dim_->dimension = dim.impl_;
  expr.add_dependency(dim.impl_);

  auto node = expr.make_node(expression<T>::node_t::ENUM);
  node->children.push_back(expr1.tree_);
  node->children.push_back(dim_);

  expr.tree_ = node;
  expr.dependencies_.insert(expr1.dependencies_.begin(), expr1.dependencies_.end());
  expr.parameters_.insert(expr1.parameters_.begin(), expr1.parameters_.end());
  return expr;
}


// Parameters
template <typename S>
static cydui::dimensions::expression<S> OPERATOR(
  const cydui::dimensions::dimension<S>& lhs, const cydui::dimensions::parameter<S>& rhs
) {
  using namespace cydui::dimensions;
  expression<S> expr{};

  expr.add_dependency(lhs.impl_);

  auto dim_node       = expr.make_node(expression<S>::node_t::DIMENSION);
  dim_node->dimension = lhs.impl_;

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(dim_node);
  node->children.push_back(param_node);

  expr.parameters_.insert(rhs);
  expr.tree_ = node;
  return expr;
}

template <typename S>
static cydui::dimensions::expression<S> OPERATOR(
  const cydui::dimensions::parameter<S>& lhs, const cydui::dimensions::dimension<S>& rhs
) {
  using namespace cydui::dimensions;
  expression<S> expr{};

  expr.add_dependency(rhs.impl_);

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = lhs;

  auto dim_node       = expr.make_node(expression<S>::node_t::DIMENSION);
  dim_node->dimension = rhs.impl_;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(param_node);
  node->children.push_back(dim_node);

  expr.parameters_.insert(lhs);
  expr.tree_ = node;
  return expr;
}

template <typename S>
static cydui::dimensions::expression<S> OPERATOR(
  const cydui::dimensions::expression<S>& lhs, const cydui::dimensions::parameter<S>& rhs
) {
  using namespace cydui::dimensions;
  expression<S> expr{};

  expr.dependencies_.insert(lhs.dependencies_.begin(), lhs.dependencies_.end());
  expr.parameters_.insert(lhs.parameters_.begin(), lhs.parameters_.end());

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(lhs.tree_);
  node->children.push_back(param_node);

  expr.parameters_.insert(rhs);
  expr.tree_ = node;
  return expr;
}

template <typename S>
static cydui::dimensions::expression<S> OPERATOR(
  const cydui::dimensions::parameter<S>& lhs, const cydui::dimensions::expression<S>& rhs
) {
  using namespace cydui::dimensions;
  expression<S> expr{};

  expr.dependencies_.insert(rhs.dependencies_.begin(), rhs.dependencies_.end());
  expr.parameters_.insert(rhs.parameters_.begin(), rhs.parameters_.end());

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = lhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(param_node);
  node->children.push_back(rhs.tree_);

  expr.parameters_.insert(lhs);
  expr.tree_ = node;
  return expr;
}

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
static cydui::dimensions::expression<S>
OPERATOR(V lhs, const cydui::dimensions::parameter<S>& rhs) {
  using namespace cydui::dimensions;
  expression<S> expr{};

  auto const_value         = expr.make_node(expression<S>::node_t::CONSTANT);
  const_value->const_value = lhs;

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(const_value);
  node->children.push_back(param_node);

  expr.parameters_.insert(rhs);
  expr.tree_ = node;
  return expr;
}

template <typename S, typename V>
  requires(not cydui::dimensions::is_dimension_v<V> && not cydui::dimensions::is_expression_v<V>)
static cydui::dimensions::expression<S>
OPERATOR(const cydui::dimensions::parameter<S>& lhs, V rhs) {
  using namespace cydui::dimensions;
  expression<S> expr{};

  auto param_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param_node->parameter = lhs;

  auto const_value         = expr.make_node(expression<S>::node_t::CONSTANT);
  const_value->const_value = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(param_node);
  node->children.push_back(const_value);

  expr.parameters_.insert(lhs);
  expr.tree_ = node;
  return expr;
}

template <typename S>
static cydui::dimensions::expression<S> OPERATOR(
  const cydui::dimensions::parameter<S>& lhs, const cydui::dimensions::parameter<S>& rhs
) {
  using namespace cydui::dimensions;
  expression<S> expr{};

  auto param1_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param1_node->parameter = lhs;

  auto param2_node       = expr.make_node(expression<S>::node_t::PARAMETER);
  param2_node->parameter = rhs;

  auto node = expr.make_node(expression<S>::node_t::ENUM);
  node->children.push_back(param1_node);
  node->children.push_back(param2_node);

  expr.parameters_.insert(lhs);
  expr.parameters_.insert(rhs);
  expr.tree_ = node;
  return expr;
}
