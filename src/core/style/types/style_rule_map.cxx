// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module cydui.styling.style_rule_map;

import std;
import reflect;

import cydui.styling.lang;
import cydui.styling.sparse_style_map;

using namespace cydui::style;


style_rule_map::style_rule_map(const refl::type_info& style_ti)
    : style_map_(style_ti) {}

bool style_rule_map::update_rule_list(const std::vector<tss::StyleRuleInstance>& new_rules) {
  if (new_rules != applicable_rules_) {
    applicable_rules_ = new_rules;
    return true;
  }
  return false;
}

const sparse_style_map& style_rule_map::get_style_map() const {
  return style_map_;
}

void style_rule_map::apply_to_object(style_object_t& obj) {
  style_map_.apply_to_object(obj);
}

std::size_t style_rule_map::count_applicable_rules() const {
  return applicable_rules_.size();
}

std::size_t style_rule_map::count_active_rules() const {
  std::size_t count = 0;
  for (auto& rule: applicable_rules_) {
    if (rule.active) {
      count++;
    }
  }
  return count;
}

bool style_rule_map::update_projection() {
  sparse_style_map new_map{style_map_.get_type()};
  for (const auto& rule: applicable_rules_) {
    if (rule.active) {
      for (const auto& [path, prop]: rule.rule->properties_) {
        new_map.set_field(path, prop);
      }
    }
  }
  // if (style_map_ != new_map) {
  style_map_ = std::move(new_map);
  return true;
  // } else {
  // return false;
  // }
}
