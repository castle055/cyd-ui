// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.base:contexts;

import std;

import fabric.logging;
import fabric.async;

export import :holder;
export import :state;

export template<typename ContextType>
struct ContextUpdate {
  constexpr static const char* type = refl::type_name<ContextUpdate<ContextType>>.data();
  ContextType* ptr = nullptr;
};

export class with_context {
public:
  with_context(std::initializer_list<cydui::components::component_holder_t> &&components) {
    std::size_t i {0};
    for (auto &&holder: components) {
      for (auto &[id, c]: holder.get_components()) {
        components_.emplace_back(std::format(":{}{}", i, id), c);
      }
      ++i;
    }
  }

  cydui::components::component_holder_t build() const {
    return cydui::components::component_holder_t {components_};
  }

private:
  // std::vector<cydui::components::component_holder_t> components_;
  std::vector<std::pair<std::string, std::shared_ptr<cydui::components::component_base_t>>> components_ { };
};

export namespace cydui::components {
  class context_store_t {
  public:
    context_store_t() = default;

    template<typename T>
    void add_context(const std::shared_ptr<T> &ptr) {
      static constexpr refl::type_id_t type_id = refl::type_id<T>;
      std::shared_ptr<void> ptr_               = std::static_pointer_cast<void>(ptr);

      context_map_[type_id] = ptr_;
    }

    template<typename T>
    std::optional<T*> find_context() {
      static constexpr refl::type_id_t type_id = refl::type_id<T>;
      if (context_map_.contains(type_id)) {
        return static_cast<T*>(context_map_[type_id].get());
      }
      return std::nullopt;
    }

    bool empty() const {
      return context_map_.empty();
    }
  private:
    std::unordered_map<refl::type_id_t, std::shared_ptr<void>> context_map_ { };
  };
}

export struct use_context_delegate;

export template<typename ContextType>
struct use_context {
  using context_type = ContextType;
  friend struct use_context_delegate;

  use_context() = default;

  use_context(const use_context& other) = delete;

  use_context& operator=(const use_context& other) = delete;

  use_context(use_context&& other) noexcept: ctx(other.ctx), state(other.state) {
    other.stop_listening();
    if (other.owns_context) {
      this->owns_context = true;

      other.owns_context = false;
      other.ctx          = nullptr;
    }
  }

  use_context& operator=(use_context&& other) {
    other.stop_listening();
    stop_listening();
    this->ctx = other.ctx;
    this->state = other.state;
    if (other.owns_context) {
      this->owns_context = true;
      other.owns_context = false;
    }
    return *this;
  }

  ~use_context() {
    stop_listening();
    if (owns_context) {
      delete ctx;
    }
  }

  context_type* operator->() {
    return ctx;
  }

  context_type &operator*() {
    return *ctx;
  }

  void notify() {
    state->emit<ContextUpdate<context_type>>({ctx});
  }

  private:
    void start_listening() {
      stop_listening();

      listener = state->window->on_event([&](ContextUpdate<context_type> ev) {
        if (ev.ptr == ctx) {
          state->force_redraw();
        }
      });
    }
    void stop_listening() {
      if (listener.has_value()) {
        listener.value().remove();
        listener = std::nullopt;
      }
    }

private:
  // This object will own the context if it couldn't be found and thus a default one was created
  bool owns_context                                  = false;
  context_type* ctx                                  = nullptr;
  cydui::components::component_state_ref state     = nullptr;

  std::optional<fabric::async::listener<ContextUpdate<context_type>>> listener {std::nullopt};
};

export struct provide_context_delegate;

export template<typename ContextType>
struct provide_context {
  using context_type = ContextType;
  friend struct provide_context_delegate;

  provide_context() = default;

  provide_context(const provide_context &other) = default;

  provide_context &operator=(const provide_context &other) = default;

  provide_context(provide_context &&other) noexcept = default;

  provide_context &operator=(provide_context &&other) = default;

  context_type* operator->() {
    return context_.get();
  }

  context_type &operator*() {
    return *context_;
  }

  void notify() {
    state->emit<ContextUpdate<context_type>>({context_.get()});
  }

  cydui::components::component_holder_t operator >(with_context &&components);

  operator std::shared_ptr<context_type>() {
    return context_;
  }

private:
  // This object will own the context if it couldn't be found and thus a default one was created
  cydui::components::component_state_ref state = nullptr;
  std::shared_ptr<context_type> context_         = std::make_shared<context_type>();
};



struct use_context_delegate {
  template<typename ContextType>
  static void set_state(use_context<ContextType>* it, const cydui::components::component_state_ref& state) {
    it->state = state;
  }
  template<typename ContextType>
  static void set_context(use_context<ContextType>* it, ContextType* ctx) {
    it->ctx = ctx;
  }
  template<typename ContextType>
  static void set_owns_context(use_context<ContextType>* it, bool value) {
    it->owns_context = value;
  }
  template<typename ContextType>
  static void start_listening(use_context<ContextType>* it) {
    it->start_listening();
  }
};

struct provide_context_delegate {
  template<typename ContextType>
  static void set_state(provide_context<ContextType>* it, const cydui::components::component_state_ref& state) {
    it->state = state;
  }
  template<typename ContextType>
  static void set_context(provide_context<ContextType>* it, const std::shared_ptr<ContextType>& ctx) {
    it->context_ = ctx;
  }
};
