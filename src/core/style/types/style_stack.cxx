// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module cydui.styling.style_stack;

import std;
import reflect;

import cydui.styling.style_override;
import cydui.styling.sparse_style_map;
import cydui.styling.style_rule_map;

using namespace cydui::style;


style_stack::style_stack(
  const refl::type_info& style_ti,
  const style_object_t&  style_object
)
    : style_ti_(style_ti),
      override_style_map_(style_ti_),
      internal_override_style_map_(style_ti_),
      animated_fields_(style_ti_),
      style_rule_map_(style_ti_),
      style_object_(style_object) {}

void style_stack::mark_dirty() {
  style_object_dirty_ = true;
}

void style_stack::update_override(const sparse_style_map& other) {
  if (override_style_map_ == other) {
    return;
  }
  override_style_map_ = other;
  style_object_dirty_ = true;
}

const style_rule_map& style_stack::get_rule_map() const {
  return style_rule_map_;
}

void style_stack::update_rule_list(const std::vector<tss::StyleRuleInstance>& new_rules) {
  bool rule_projection_changed = style_rule_map_.update_rule_list(new_rules);
  if (rule_projection_changed) {
    style_object_dirty_ = true;
  }
}

bool style_stack::apply() {
  if (style_object_dirty_) {
    style_object_dirty_ = false;

    style_object_.reset();

    style_rule_map_.apply_to_object(style_object_);
    override_style_map_.apply_to_object(style_object_);
    internal_override_style_map_.apply_to_object(style_object_);
    animated_fields_.apply_to_object(style_object_);
    return true;
  }
  return false;
}

void style_stack::apply_animations() {
  animated_fields_.apply_to_object(style_object_);
}

sparse_style_map& style_stack::get_style_override() {
  return override_style_map_;
}

sparse_style_map& style_stack::get_internal_style_override() {
  return internal_override_style_map_;
}

sparse_style_map& style_stack::get_animated_style() {
  return animated_fields_;
}

const style_base_t& style_stack::get_style_object() const {
  return style_object_.get_base();
}
