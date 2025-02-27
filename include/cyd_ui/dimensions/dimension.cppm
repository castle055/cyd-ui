/*! \file  dimension.cppm
 *! \brief 
 *!
 */

export module cydui.dimensions:dimension;

import std;
import reflect;

import fabric.logging;

export import :types;
export import :expression;
export import :impl;

export template <typename Type>
class cydui::dimensions::dimension {
public:
  using value_type = Type;
  using expression = expression<value_type>;
  using context = context<value_type>;

  dimension()
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->self = impl_;
  }

  explicit dimension(expression&& expr)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->self = impl_;
    impl_->set_expression(expr);
  }

  explicit dimension(const expression& expr)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->self = impl_;
    impl_->set_expression(expr);
  }

  explicit dimension(const std::shared_ptr<context>& ctx)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>{ctx}}) {
    impl_->self = impl_;
  }

  explicit dimension(const std::shared_ptr<context>& ctx, expression&& expr)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>{ctx}}) {
    impl_->self = impl_;
    impl_->set_expression(expr);
  }

  dimension(const dimension& other)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->self = impl_;
    impl_->set_expression(other.impl_->expr_);
    impl_->value_ = other.impl_->value_;
    impl_->unknown_ = other.impl_->unknown_;
    set_context(other.get_context());
  }

  dimension& operator=(const dimension& other) {
    impl_->set_expression(other.impl_->expr_);
    impl_->value_ = other.impl_->value_;
    impl_->unknown_ = other.impl_->unknown_;
    return *this;
  }

  dimension& operator=(expression&& expr) {
    impl_->set_expression(expr);
    return *this;
  }

  dimension& operator=(const expression& expr) {
    impl_->set_expression(expr);
    return *this;
  }

  dimension& operator=(value_type&& expr) {
    impl_->value_ = std::move(expr);
    impl_->set_expression(std::move(expr));
    return *this;
  }

  dimension& operator=(const value_type& expr) {
    impl_->value_ = expr;
    impl_->set_expression(expr);
    return *this;
  }

  bool operator==(const dimension& rhl) const {
    // TODO - I don't think this is right, what if we call this on a yet not computed dimension
    return value() == rhl.value();
  }

  bool is_set() const {
    return impl_->is_set();
  }

  void clear() {
    for (const auto& dep: impl_->expr_.dependencies_) {
      dep->dependents_.erase({impl_});
    }
    impl_->clear();
  }

  const std::shared_ptr<context>& get_context() const {
    return impl_->context_;
  }
  void set_context(const std::shared_ptr<context>& ctx) {
    std::shared_ptr<context> new_ctx = ctx;
    impl_->context_.swap(new_ctx);
  }

  const expression& get_expression() const {
    return impl_->expr_;
  }

  friend struct dimensional_operators;

  template <typename S>
  friend compute_result_t<S> compute_dimension(
    dimension<S>& dim_, const std::unordered_map<std::string, dimension<S>>& parameters
  );
  template <typename S>
  friend bool evaluate_expression(
    typename dimension_impl<S>::sptr                     dim,
    const std::unordered_map<std::string, dimension<S>>& parameters
  );
  template <typename S>
  friend const S& get_value(dimension<S>& dim);
  template <typename S>
  friend const S& get_value(const dimension<S>& dim);

private:
  const value_type& value() const {
    return impl_->value_;
  }

  dimension_impl<value_type>::sptr impl() const {
    return impl_;
  }

  [[meta(refl::eq_policy::shallow)]]
  typename dimension_impl<value_type>::sptr impl_;
};


struct cydui::dimensions::dimensional_operators {
  dimensional_operators() = delete;

#define OPERATOR add
#define ENUM     ADDITION
#include "operators_impl.inc"
#undef OPERATOR
#undef ENUM

#define OPERATOR subtract
#define ENUM     SUBTRACTION
#include "operators_impl.inc"
#undef OPERATOR
#undef ENUM

#define OPERATOR multiply
#define ENUM     MULTIPLICATION
#include "operators_impl.inc"
#undef OPERATOR
#undef ENUM

#define OPERATOR divide
#define ENUM     DIVISION
#include "operators_impl.inc"
#undef OPERATOR
#undef ENUM
};

export {
#define OPERATOR    +
#define OP_FUNCTION add
#include "operators.inc"
#undef OPERATOR
#undef OP_FUNCTION

#define OPERATOR    -
#define OP_FUNCTION subtract
#include "operators.inc"
#undef OPERATOR
#undef OP_FUNCTION

#define OPERATOR    *
#define OP_FUNCTION multiply
#include "operators.inc"
#undef OPERATOR
#undef OP_FUNCTION

#define OPERATOR    /
#define OP_FUNCTION divide
#include "operators.inc"
#undef OPERATOR
#undef OP_FUNCTION
}
