/*! \file  dimension.cppm
 *! \brief 
 *!
 */

export module cydui.dimensions:dimension;

import std;

import fabric.logging;

import :types;
import :expression;
import :impl;

template <typename Type>
class cyd::ui::dimensions::dimension {
public:
  using value_type = Type;
  using expression = expression<value_type>;
  using context = context<value_type>;

  dimension()
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {}

  explicit dimension(expression&& expr)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->set_expression(expr);
  }

  explicit dimension(const std::shared_ptr<context>& ctx)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>{ctx}}) {}

  explicit dimension(const std::shared_ptr<context>& ctx, expression&& expr)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>{ctx}}) {
    impl_->set_expression(expr);
  }

  dimension(const dimension& other)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->set_expression(other.impl_->expr_);
    impl_->value_ = other.impl_->value_;
    impl_->unknown_ = other.impl_->unknown_;
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

  bool operator==(const dimension& rhl) const {
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

  typename dimension_impl<value_type>::sptr impl_;
};


struct cyd::ui::dimensions::dimensional_operators {
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
