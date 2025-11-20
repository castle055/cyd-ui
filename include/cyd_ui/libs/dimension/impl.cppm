/*! \file  impl.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

export module cydui.dimensions:impl;

import std;

import fabric.logging;
export import cydui.digraph;

export import :types;
export import :expression;
export import :context;

namespace cydui::dimensions {
  export template <typename S>
  const S& get_value(const dimension<S>& dimension);

  template <typename T>
  class dimension_impl: public digraph_node<dimension_impl<T>> {
    expression<T>               expr_ {};
    T                           value_ {};
    bool                        unknown_ = true;
    std::shared_ptr<context<T>> context_;
    std::string                 name_ {};

  public:
    using value_type = T;
    using wptr       = std::weak_ptr<dimension_impl>;
    using sptr       = std::shared_ptr<dimension_impl>;

    ~dimension_impl() {
      this->clear_inputs();
    }

    friend dimension<T>;
    friend expression<T>;
    friend typename expression<T>::node_t;

    template <typename S>
    friend const S& get_value(const dimension<S>& dimension);

    template <typename S>
    friend std::string to_string(const dimension_impl<S>* dimension);

    template <typename S>
    friend compute_result_t<S> compute_dimension(
      dimension<S>& dim_,
      const std::unordered_map<
        std::string,
        dimension<S>>& parameters);
    template <typename S>
    friend bool evaluate_expression(
      typename dimension_impl<S>::sptr dim,
      const std::unordered_map<
        std::string,
        dimension<S>>& parameters);
    template <typename S>
    friend bool find_cycle(
      cycle_t<S>&                      cycle,
      typename dimension_impl<S>::sptr start,
      typename dimension_impl<S>::sptr head,
      const std::unordered_map<
        std::string,
        dimension<S>>& global_parameters);

    bool is_set() const {
      return not expr_.empty();
    }

    void clear() {
      this->clear_inputs();

      expr_.clear();

      mark_unknown();
    }

    dimension_impl()
        : context_(new context<T> {}) {}

  private:
    void set_inputs() {
      for (const auto& param: expr_.parameters_) {
        if (context_->contains(param.name)) {
          auto& param_dim = context_->operator[](param.name);
          this->add_input(param_dim.impl_);
        }
      }
      for (auto dependency: expr_.dependencies_) {
        this->add_input(dependency);
      }
    }

    void set_expression(const expression<T>& expression) {
      this->clear_inputs();
      expr_ = expression;
      set_inputs();
    }

    void set_context(
      const std::shared_ptr<context<T>>& ctx,
      const std::string&                 name = "") {
      std::shared_ptr<context<T>> new_ctx = ctx;
      this->clear_inputs();
      context_.swap(new_ctx);
      name_ = name;
      set_inputs();
    }

    void mark_unknown() {
      ZoneScopedN("mark_unknown");
      if (unknown_) {
        return;
      }
      unknown_ = true;

      // TODO - unroll this recursion
      for (const auto& dependent: this->get_outputs()) {
        dependent->data().mark_unknown();
      }
    }

  private:
    expression<T>& expr() {
      return expr_;
    }
    const expression<T>& expr() const {
      return expr_;
    }

    bool is_unknown() const {
      return unknown_;
    }
  };

  export template <typename T>
  typename dimension_impl<T>::sptr make_dimension_impl() {
    return digraph_node<dimension_impl<T>>::make();
  }

  template <typename T>
  std::string to_string(const dimension_impl<T>* dim) {
    std::stringstream ss{};
    if (not dim->context_->get_name().empty() and not dim->name_.empty()) {
      return std::format("{}.{}", dim->context_->get_name(), dim->name_);
    } else {
      return "<unknown>";
    }
  }
} // namespace cydui::dimensions
