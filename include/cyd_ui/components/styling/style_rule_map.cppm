// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.style_rule_map;

import std;
export import reflect;

export import cydui.styling.lang;
export import cydui.styling.sparse_style_map;

export namespace cydui::style {
  class style_rule_map {
    std::vector<tss::StyleRuleInstance> applicable_rules_;
    sparse_style_map                    style_map_;

  public:
    explicit style_rule_map(const refl::type_info& style_ti)
        : style_map_(style_ti) {}

    bool update_rule_list(const std::vector<tss::StyleRuleInstance>& new_rules) {
      if (new_rules != applicable_rules_) {
        applicable_rules_ = new_rules;
        return true;
      }
      return false;
    }

    bool manage_rules(auto&& fun) {
      bool must_update_projection = false;
      for (auto& rule: applicable_rules_) {
        const auto& rule_cref        = rule;
        bool        should_be_active = fun(rule_cref);

        if (should_be_active != rule.active) {
          rule.active            = should_be_active;
          must_update_projection = true;
        }
      }

      bool projection_changed = false;
      if (must_update_projection) {
        projection_changed = update_projection();
      }

      return projection_changed;
    }

    const sparse_style_map& get_style_map() const {
      return style_map_;
    }

    void apply_to_object(style_object_t& obj) {
      style_map_.apply_to_object(obj);
    }

    std::size_t count_applicable_rules() const {
      return applicable_rules_.size();
    }

    std::size_t count_active_rules() const {
      std::size_t count = 0;
      for (auto& rule: applicable_rules_) {
        if (rule.active) {
          count++;
        }
      }
      return count;
    }

  private:
    bool update_projection() {
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
  };
} // namespace cydui::style
