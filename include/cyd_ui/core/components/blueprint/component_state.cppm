// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.state;

import std;
import fabric.logging;
import fabric.async;

export import cydui.event_types;


namespace cydui::detail {
  export class component_state_delegate_t;
  export class ComponentState;

  class ComponentState {
  public:
    using sptr = std::shared_ptr<ComponentState>;

  public:
    std::string                           component_name_;
    ComponentState*                    parent_ = nullptr;
    std::unordered_map<std::string, sptr> children_states{};

  public:
    virtual ~ComponentState() = default;

    ComponentState() = default;
    explicit ComponentState(void* props) {}


    std::string component_name() const {
      return component_name_;
    }

    ComponentState* parent() {
      return parent_;
    }

    void add_children_state(
      const std::string& id,
      const sptr&        child
    ) {
      children_states[id] = child;
      child->parent_      = this;
    }

  private:
    friend class component_state_delegate_t;
  };

  class component_state_delegate_t {
  public:
    static void set_name(
      ComponentState* it,
      const std::string& name
    ) {
      it->component_name_ = name;
    }
  };
} // namespace cydui::components
