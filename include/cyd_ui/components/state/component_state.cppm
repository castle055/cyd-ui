// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.state;

import std;

import fabric.logging;
import fabric.async;

export import cydui.events;
export import cydui.backends.frame_base;


namespace cydui::components {
  export class component_state_delegate_t;
  export class component_state_t;

  class component_state_t {
  public:
    using sptr = std::shared_ptr<component_state_t>;

  public:
    std::string                           component_name_;
    component_state_t*                    parent_ = nullptr;
    std::unordered_map<std::string, sptr> children_states{};

  public:
    virtual ~component_state_t() = default;

    component_state_t() = default;
    explicit component_state_t(void* props) {}


    std::string component_name() const {
      return component_name_;
    }

    component_state_t* parent() {
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
      component_state_t* it,
      const std::string& name
    ) {
      it->component_name_ = name;
    }
  };
} // namespace cydui::components
