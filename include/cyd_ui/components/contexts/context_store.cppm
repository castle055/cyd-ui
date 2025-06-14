// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.contexts.store;

import std;
export import reflect;

import fabric.logging;

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
    std::optional<std::shared_ptr<T>> find_context() {
      static constexpr refl::type_id_t type_id = refl::type_id<T>;
      if (context_map_.contains(type_id)) {
        return std::static_pointer_cast<T>(context_map_[type_id]);
      }
      return std::nullopt;
    }

    bool empty() const {
      return context_map_.empty();
    }
  private:
    [[refl::ignore]]
    std::unordered_map<refl::type_id_t, std::shared_ptr<void>> context_map_ { };
  };
}
