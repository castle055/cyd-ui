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

namespace cydui::dimensions {
  export template <typename S>
  compute_result_t<S> compute_dimension(
    dimension<S>& dim_,
    const std::unordered_map<
      std::string,
      dimension<S>>& parameters
  );
  export template <typename S>
  bool evaluate_expression(
    typename dimension_impl<S>::sptr dim,
    const std::unordered_map<
      std::string,
      dimension<S>>& parameters
  );
  export template <typename S>
  const S& get_value(const dimension<S>& dimension);
  export template <typename S>
  bool find_cycle(
    cycle_t<S>&                      cycle,
    typename dimension_impl<S>::sptr start,
    typename dimension_impl<S>::sptr head,
    const std::unordered_map<
      std::string,
      dimension<S>>& global_parameters
  );

  export template <typename Type>
  class dimension {
  public:
    using value_type = Type;
    using expression = expression<value_type>;
    using context    = context<value_type>;

    dimension()
        : impl_(make_dimension_impl<value_type>()) {}

    explicit dimension(expression&& expr)
        : impl_(make_dimension_impl<value_type>()) {
      impl_->set_expression(expr);
      impl_->mark_unknown();
    }

    explicit dimension(const expression& expr)
        : impl_(make_dimension_impl<value_type>()) {
      impl_->set_expression(expr);
      impl_->mark_unknown();
    }

    explicit dimension(const std::shared_ptr<context>& ctx)
        : impl_(make_dimension_impl<value_type>()) {
      impl_->set_context(ctx);
      impl_->mark_unknown();
    }

    explicit dimension(
      const std::shared_ptr<context>& ctx,
      expression&&                    expr
    )
        : impl_(make_dimension_impl<value_type>()) {
      impl_->set_context(ctx);
      impl_->set_expression(expr);
      impl_->mark_unknown();
    }

    dimension(const dimension& other)
        : impl_(other.impl_) {
    }

    dimension& operator=(const dimension& other) {
      return this->operator=(expression{other.impl_});
    }

    dimension& operator=(expression&& expr) {
      if (impl_->expr_ != expr) {
        impl_->set_expression(expr);
        impl_->mark_unknown();
      }
      return *this;
    }

    dimension& operator=(const expression& expr) {
      if (impl_->expr_ != expr) {
        impl_->set_expression(expr);
        impl_->mark_unknown();
      }
      return *this;
    }

    dimension& operator=(const value_type& expr) {
      impl_->value_ = expr;
      impl_->set_expression(expr);
      impl_->mark_unknown();
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
      impl_->clear();
    }

    const std::shared_ptr<context>& get_context() const {
      return impl_->context_;
    }
    void set_context(
      const std::shared_ptr<context>& ctx,
      const std::string&              name = ""
    ) {
      impl_->set_context(ctx, name);
    }

    const expression& get_expression() const {
      return impl_->expr_;
    }

    typename expression::dep_t as_dependency() const {
      return impl_;
    }

    friend class dimension_impl<Type>;

    friend struct dimensional_operators;

    template <typename S>
    friend compute_result_t<S> compute_dimension(
      dimension<S>& dim_,
      const std::unordered_map<
        std::string,
        dimension<S>>& parameters
    );
    template <typename S>
    friend bool evaluate_expression(
      typename dimension_impl<S>::sptr dim,
      const std::unordered_map<
        std::string,
        dimension<S>>& parameters
    );
    template <typename S>
    friend const S& get_value(const dimension<S>& dimension);
    template <typename S>
    friend bool find_cycle(
      cycle_t<S>&                      cycle,
      typename dimension_impl<S>::sptr start,
      typename dimension_impl<S>::sptr head,
      const std::unordered_map<
        std::string,
        dimension<S>>& global_parameters
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
} // namespace cydui::dimensions


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
