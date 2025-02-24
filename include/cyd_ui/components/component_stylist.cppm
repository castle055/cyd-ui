// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>
#include "cyd_fabric_modules/headers/macros/async_events.h"

export module cydui.components.stylist;

import std;
import reflect;
import fabric.logging;
import fabric.wiring.signals;

import cydui.application;
import cydui.graphics;
export import cydui.components.base;

namespace cyd::ui::components {
  export class component_stylist_t {
  public:
    using sptr = std::shared_ptr<component_stylist_t>;

    static sptr make() {
      return std::make_shared<component_stylist_t>();
    }

  public:
    void compile_style_rule_list(const component_base_t::sptr& component, StyleArchive& style_archive) {
      auto& style_rules = component->get_style_data().rules;
      style_rules.clear();

      style_archive.for_each_rule(component->name(), [&](const StyleRule::sptr &rule) {
        for (const auto & selector : rule->selectors_) {
          if (check_style_comb_selector(component, selector)) {
            style_rules.emplace_back(selector.specificity(), rule);
            break;
          }
        }
      });

      std::stable_sort(style_rules.begin(), style_rules.end(), [](const auto &lhs, const auto &rhs) {
        return lhs.first < rhs.first;
      });
    }

    void apply_style(const component_base_t::sptr& component) {
      ZoneScopedN("Apply Style");
      auto& style_data = component->get_style_data();
      style_data.reset();

      if (style_data.has_override()) {
        style_data.apply_override();
      } else {
        apply_style_rules(component);
      }

      style_data.apply_transform();
    }

  private:
    void apply_style_rules(const component_base_t::sptr& component) {
      ZoneScopedN("Apply Rules");
      auto& style_data = component->get_style_data();
      auto& style_rules = style_data.rules;
      const auto& bti = refl::type_info::from<style_base_t>();
      const auto& ti = component->get_style_type_info();

      std::unordered_set<const refl::field_info*> pending_fields{};
      for (const auto & fti : ti.fields()) {
        pending_fields.insert(&fti);
      }
      std::unordered_set<const refl::field_info*> pending_base_fields{};
      for (const auto & bfti : bti.fields()) {
        pending_fields.insert(&bfti);
      }

      style_base_t& base_s = component->get_style();
      for (auto& [specificity, rule]: style_rules | std::views::reverse) {
        if (check_style_comb_selector_vector(component, rule->selectors_, true, true)) {
          ZoneScopedN("Apply Rule");
          //! Base fields
          for (auto field_it = pending_base_fields.begin(); field_it != pending_base_fields.end();) {
            apply_style_property(component, &base_s, field_it, pending_base_fields, rule);
          }
          //! Custom fields
          for (auto field_it = pending_fields.begin(); field_it != pending_fields.end();) {
            apply_style_property(component, style_data.as_raw(), field_it, pending_fields, rule);
          }
        }
      }
    }

    void apply_style_property(const component_base_t::sptr &component, void *style_obj,
                              std::unordered_set<const refl::field_info *>::iterator &field_it,
                              std::unordered_set<const refl::field_info *> &pending_fields,
                              const StyleRule::sptr &rule) {
      ZoneScopedN("Apply Property");
      const auto *field = *field_it;
      if (rule->properties_.contains(field->name)) {
        const auto &rule_field = rule->properties_.at(field->name);
        if (rule_field.is(field->type())) {
          field->type().assign_copy_of(rule_field.data(), field->get_ptr(style_obj));
          field_it = pending_fields.erase(field_it);
          return;
        } else if (field->type().is_type<vg::paint::type>()) {
          if (rule_field.is<color::Color>()) {
            vg::paint::type &paint = field->get_ref<vg::paint::type>(style_obj);
            const color::Color &paint_color = rule_field.as<color::Color>();
            paint = vg::paint::type::make(vg::paint::solid{paint_color});
            field_it = pending_fields.erase(field_it);
            return;
          }
        }
        LOG::print{WARN}("Property '{}::{}', expected type: '{}', found: '{}'",
                         component->name(),
                         field->name,
                         field->type().name(),
                         rule_field.type().name());
      }
      ++field_it;
    }

    bool check_style_comb_selector_vector(const component_base_t::sptr& component, const std::vector<StyleRuleCombinedSelector> &selectors,
                                                 bool check_tags = false, bool check_pseudo_states = false) {
      ZoneScopedN("Check Selectors");
      for (const auto &selector: selectors) {
        if (check_style_comb_selector(component, selector, check_tags, check_pseudo_states)) {
          return true;
        }
      }
      return false;
    }

    bool check_style_comb_selector(const component_base_t::sptr& component, const StyleRuleCombinedSelector& selector, bool check_tags = false, bool check_pseudo_states = false) {
      auto it = selector.selectors.rbegin();
      if (not check_style_selector(component.get(), it->second, check_tags, check_pseudo_states)) {
        return false;
      }
      StyleRuleCombinedSelector::kind_e kind = it->first;
      ++it;

      component_base_t* current = component.get();
      while (it != selector.selectors.rend()) {
        if (kind == StyleRuleCombinedSelector::CHILD_COMBINATOR) {
          if (current->parent.has_value() and check_style_selector(current->parent.value(), it->second, check_tags, check_pseudo_states)) {
            current = current->parent.value();
          } else {
            return false;
          }
        } else if (kind == StyleRuleCombinedSelector::DESCENDENT_COMBINATOR) {
          bool found = false;
          while (current->parent.has_value()) {
            if (check_style_selector(current->parent.value(), it->second, check_tags, check_pseudo_states)) {
              found = true;
              break;
            }
            current = current->parent.value();
          }

          if (not found) {
            return false;
          }
        }

        kind = it->first;
        ++it;
      }

      return true;
    }

    bool check_style_selector(component_base_t* component, const StyleRuleSelector& selector, bool check_tags, bool check_pseudo_states) {
      ZoneScopedN("Check Selector");
      auto& style_data = component->get_style_data();
      if (component->name() != selector.component) {
        return false;
      }

      if (check_tags) {
        for (const auto & tag : selector.tags) {
          if (not style_data.tags.contains(tag)) {
            return false;
          }
        }
      }

      if (check_pseudo_states) {
        if (selector.pseudo_states.contains("hover") and not component->state()->hovering) {
          return false;
        }
        if (selector.pseudo_states.contains("focus") and not component->state()->focused) {
          return false;
        }
      }

      return true;
    }
  };
}