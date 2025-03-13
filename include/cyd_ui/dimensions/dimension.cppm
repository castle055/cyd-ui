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
  using context    = context<value_type>;

  dimension()
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->self = impl_;
  }

  explicit dimension(expression&& expr)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->self = impl_;
    impl_->set_expression(expr);
    impl_->mark_unknown();
  }

  explicit dimension(const expression& expr)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->self = impl_;
    impl_->set_expression(expr);
    impl_->mark_unknown();
  }

  explicit dimension(const std::shared_ptr<context>& ctx)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>{ctx}}) {
    impl_->self = impl_;
    impl_->mark_unknown();
  }

  explicit dimension(const std::shared_ptr<context>& ctx, expression&& expr)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>{ctx}}) {
    impl_->self = impl_;
    impl_->set_expression(expr);
    impl_->mark_unknown();
  }

  dimension(const dimension& other)
      : impl_(std::shared_ptr<dimension_impl<value_type>>{new dimension_impl<value_type>}) {
    impl_->self = impl_;
    impl_->set_expression(other.impl_->expr_);
    impl_->value_ = other.impl_->value_;
    set_context(other.get_context(), other.impl_->name_);
    impl_->unknown_ = false;
    if (other.impl_->unknown_) {
      impl_->mark_unknown();
    }
  }

  dimension& operator=(const dimension& other) {
    impl_->set_expression(other.impl_->expr_);
    impl_->value_   = other.impl_->value_;
    impl_->unknown_ = false;
    if (other.impl_->unknown_) {
      impl_->mark_unknown();
    }
    return *this;
  }

  dimension& operator=(expression&& expr) {
    impl_->set_expression(expr);
    impl_->mark_unknown();
    return *this;
  }

  dimension& operator=(const expression& expr) {
    impl_->set_expression(expr);
    impl_->mark_unknown();
    return *this;
  }

  dimension& operator=(value_type&& expr) {
    impl_->value_ = std::move(expr);
    impl_->set_expression(std::move(expr));
    impl_->unknown_ = false;
    return *this;
  }

  dimension& operator=(const value_type& expr) {
    impl_->value_ = expr;
    impl_->set_expression(expr);
    impl_->unknown_ = false;
    return *this;
  }

  bool operator==(const dimension& rhl) const {
    return get_expression() == rhl.get_expression();
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
  void set_context(const std::shared_ptr<context>& ctx, const std::string& name = "") {
    impl_->set_context(ctx, name);
  }

  const expression& get_expression() const {
    return impl_->expr_;
  }

  friend class dimension_impl<Type>;

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
  template <typename S>
  friend bool find_cycle(
    cycle_t<S>&                                          cycle,
    typename dimension_impl<S>::sptr                     start,
    typename dimension_impl<S>::sptr                     head,
    const std::unordered_map<std::string, dimension<S>>& global_parameters
  );

private:
  const value_type& value() const {
    return impl_->value_;
  }

  typename dimension_impl<value_type>::sptr impl() const {
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
