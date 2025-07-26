// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.style_stack;

import std;
export import reflect;

export import cydui.styling.sparse_style_map;
export import cydui.styling.style_rule_map;

export namespace cydui::style {
  class style_stack {
    const refl::type_info& style_ti_;

    sparse_style_map override_style_map_;
    sparse_style_map internal_override_style_map_;
    sparse_style_map animated_fields_;

    style_rule_map style_rule_map_;

    style_object_t style_object_;
    bool           style_object_dirty_ = true;

  public:
    explicit style_stack(
      const refl::type_info& style_ti,
      const style_object_t&  style_object
    );

    void mark_dirty();

    void update_override(const sparse_style_map& other);

    const style_rule_map& get_rule_map() const;

    void update_rule_list(const std::vector<tss::StyleRuleInstance>& new_rules);

    void manage_rules(auto&& fun) {
      bool rule_projection_changed = style_rule_map_.manage_rules(fun);
      if (rule_projection_changed) {
        style_object_dirty_ = true;
      }
    }

    bool apply();

    void apply_animations();

    sparse_style_map& get_style_override();

    sparse_style_map& get_internal_style_override();

    sparse_style_map& get_animated_style();

    const style_base_t& get_style_object() const;
  };
} // namespace cydui::style
