// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.contexts.store;

import std;
export import reflect;

import fabric.logging;

export namespace cydui::detail {
  class context_store_t {
    context_store_t* parent_context_;
    [[refl::ignore]]
    std::unordered_map<refl::type_id_t, std::shared_ptr<void>> context_map_{};

  public:
    explicit context_store_t(context_store_t* parent_context)
        : parent_context_(parent_context) {}

    template <typename T>
    void add_context(const std::shared_ptr<T>& ptr) {
      static constexpr refl::type_id_t type_id = refl::type_id<T>;
      std::shared_ptr<void>            ptr_    = std::static_pointer_cast<void>(ptr);

      context_map_[type_id] = ptr_;
    }

    template <typename ContextType>
    std::optional<std::shared_ptr<ContextType>> find_context() {
      static constexpr refl::type_id_t type_id = refl::type_id<ContextType>;
      if (not empty()) {
        auto context = context_map_.contains(type_id)
                         ? std::static_pointer_cast<ContextType>(context_map_[type_id])
                         : nullptr;
        if (nullptr != context) {
          return context;
        }
      }

      if (nullptr != parent_context_) {
        return parent_context_->find_context<ContextType>();
      }

      return std::nullopt;
    }


    bool empty() const {
      return context_map_.empty();
    }
  };
} // namespace cydui::core
