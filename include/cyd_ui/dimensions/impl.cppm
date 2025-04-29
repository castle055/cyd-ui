/*! \file  impl.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

export module cydui.dimensions:impl;

import std;

import fabric.logging;

export import :types;
export import :expression;
export import :context;

namespace cydui::dimensions {
  export template <typename S>
  const S& get_value(const dimension<S>& dimension);

  template <typename T>
  class dimension_impl {
  public:
    using value_type = T;
    using wptr       = std::weak_ptr<dimension_impl>;
    using sptr       = std::shared_ptr<dimension_impl>;

    ~dimension_impl() {
      clear_dependencies();
    }

    friend dimension<T>;
    friend expression<T>;
    friend typename expression<T>::node_t;

    template <typename S>
    friend const S& get_value(const dimension<S>& dimension);

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
    friend bool find_cycle(
      cycle_t<S>&                                          cycle,
      typename dimension_impl<S>::sptr                     start,
      typename dimension_impl<S>::sptr                     head,
      const std::unordered_map<std::string, dimension<S>>& global_parameters
    );

    bool is_set() const {
      return not expr_.empty();
    }

    void clear() {
      clear_dependencies();

      expr_.clear();

      mark_unknown();
    }

  private:
    dimension_impl()
        : context_(new context<T>{}) {}

    explicit dimension_impl(const std::shared_ptr<context<T>> ctx)
        : context_(ctx) {}

    void clear_dependencies() {
      for (const auto& param: expr_.parameters_) {
        if (context_->contains(param.name)) {
          auto& param_dim = context_->operator[](param.name);
          param_dim.impl_->dependents_.erase(this);
        }
      }
      for (auto dependency: expr_.dependencies_) {
        dependency->dependents_.erase(this);
      }
    }
    void set_dependencies() {
      for (const auto& param: expr_.parameters_) {
        if (context_->contains(param.name)) {
          auto& param_dim = context_->operator[](param.name);
          param_dim.impl_->dependents_.insert(this);
        }
      }
      for (auto dependency: expr_.dependencies_) {
        dependency->dependents_.insert(this);
      }
    }

    void set_expression(const expression<T>& expression) {
      clear_dependencies();
      expr_ = expression;
      set_dependencies();
    }

    void set_context(const std::shared_ptr<context<T>>& ctx, const std::string& name = "") {
      std::shared_ptr<context<T>> new_ctx = ctx;
      clear_dependencies();
      context_.swap(new_ctx);
      name_ = name;
      set_dependencies();
    }

    void mark_unknown() {
      ZoneScopedN("mark_unknown");
      if (unknown_) {
        return;
      }
      unknown_ = true;

      // TODO - unroll this recursion
      for (const auto& dependent: dependents_) {
        dependent->mark_unknown();
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

  private:
    expression<T> expr_{};
    T             value_{};
    bool          unknown_ = true;
    [[refl::ignore]]
    std::unordered_set<dimension_impl*> dependents_{};
    std::shared_ptr<context<T>>         context_;
    std::string                         name_{};

    [[refl::ignore]]
    wptr self;
  };

  template <typename T>
  typename dimension_impl<T>::sptr make_dimension_impl() {
    return std::make_shared<dimension_impl<T>>();
  }
} // namespace cydui::dimensions
