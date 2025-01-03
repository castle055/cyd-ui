/*! \file  impl.cppm
 *! \brief 
 *!
 */

export module cydui.dimensions:impl;

import std;

import fabric.logging;

import :types;
import :expression;
import :context;

namespace cyd::ui::dimensions {
  export template<typename S>
  const S &get_value(dimension<S> &dimension);

  export template<typename S>
  const S &get_value(const dimension<S> &dimension);

  template <typename T>
  class dimension_impl {
  public:
    using value_type = T;
    using wptr       = std::weak_ptr<dimension_impl>;
    using sptr       = std::shared_ptr<dimension_impl>;

    ~dimension_impl() = default;

    friend dimension<T>;
    friend expression<T>;
    friend expression<T>::node_t;

    template <typename S>
    friend const S& get_value(dimension<S>& dim);

    template <typename S>
    friend const S& get_value(const dimension<S>& dim);

    template <typename S>
    friend compute_result_t<S> compute_dimension(dimension<S>& dim_, const std::unordered_map<std::string, dimension<S>>& parameters);
    template <typename S>
    friend bool evaluate_expression(typename dimension_impl<S>::sptr dim, const std::unordered_map<std::string, dimension<S>>& parameters);
    template <typename S>
    friend bool find_cycle(
      cycle_t<S>&                      cycle,
      typename dimension_impl<S>::sptr start,
      typename dimension_impl<S>::sptr head
    );

    bool is_set() const {
      return not expr_.empty();
    }

    void clear() {
      expr_.clear();
      mark_unknown();
    }

  private:
    dimension_impl()
        : context_(new context<T>{}) {}

    explicit dimension_impl(const std::shared_ptr<context<T>> ctx)
        : context_(ctx) {}

    void set_expression(const expression<T>& expression) {
      if (expression == expr_) {
        return;
      }
      expr_ = expression;
      mark_unknown();
    }

    void mark_unknown() {
      unknown_ = true;
      for (auto dependent: dependents_) {
        if (dependent.expired()) {
          dependents_.erase(dependent);
        } else {
          if (!dependent.lock()->unknown_) {
            dependent.lock()->mark_unknown();
          }
        }
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
    expression<T>               expr_{};
    T                           value_{};
    bool                        unknown_ = true;
    std::unordered_set<wptr>    dependents_{};
    std::shared_ptr<context<T>> context_;
  };

  template <typename T>
  typename dimension_impl<T>::sptr make_dimension_impl() {
    return std::make_shared<dimension_impl<T>>();
  }
}

template <typename T>
struct std::hash<std::weak_ptr<cyd::ui::dimensions::dimension_impl<T>>> {
    std::size_t operator()(const std::weak_ptr<cyd::ui::dimensions::dimension_impl<T>>& it) const {
        return reinterpret_cast<std::size_t>(it.lock().get());
    }
};

template <typename T>
struct std::equal_to<std::weak_ptr<cyd::ui::dimensions::dimension_impl<T>>> {
    bool operator()(const std::weak_ptr<cyd::ui::dimensions::dimension_impl<T>>& it1, const std::weak_ptr<cyd::ui::dimensions::dimension_impl<T>>& it2) const {
        return it1.lock() == it2.lock();
    }
};
