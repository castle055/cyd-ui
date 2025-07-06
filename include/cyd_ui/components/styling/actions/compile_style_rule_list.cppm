// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.styling.actions.compile_style_rule_list;

import std;
import reflect;
import fabric.logging;
import fabric.wiring.signals;

import cydui.application;
export import cydui.components.mounted;
export import cydui.styling.lang;


namespace cydui::style {
  bool check_style_selector(
    components::mounted_component_t& component,
    const tss::StyleRuleSelector&    selector,
    bool                             check_tags,
    bool                             check_pseudo_states
  ) {
    ZoneScopedN("Check Selector");
    if (component.get_name() != selector.component) {
      return false;
    }

    if (check_tags) {
      for (const auto& tag: selector.tags) {
        if (not component.get_blueprint()->has_tag(tag)) {
          return false;
        }
      }
    }

    if (check_pseudo_states) {
      if (selector.pseudo_states.contains("hover") and not component.is_hovered()) {
        return false;
      }
      if (selector.pseudo_states.contains("focus") and not component.is_focused()) {
        return false;
      }
    }

    return true;
  }

  bool check_style_comb_selector(
    components::mounted_component_t&      component,
    const tss::StyleRuleCombinedSelector& selector,
    bool                                  check_tags = false,
    bool check_pseudo_states                         = false
  ) {
    auto it = selector.selectors.rbegin();
    if (not check_style_selector(component, it->second, check_tags, check_pseudo_states)) {
      return false;
    }
    tss::StyleRuleCombinedSelector::kind_e kind = it->first;
    ++it;

    components::mounted_component_t* current = &component;
    while (it != selector.selectors.rend()) {
      if (kind == tss::StyleRuleCombinedSelector::CHILD_COMBINATOR) {
        auto& parent = *current->get_parent();
        if (not current->is_root()
            and check_style_selector(parent, it->second, check_tags, check_pseudo_states)) {
          current = &parent;
        } else {
          return false;
        }
      } else if (kind == tss::StyleRuleCombinedSelector::DESCENDENT_COMBINATOR) {
        bool found = false;
        while (not current->is_root()) {
          auto& parent = *current->get_parent();
          if (check_style_selector(parent, it->second, check_tags, check_pseudo_states)) {
            found = true;
            break;
          }
          current = &parent;
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
} // namespace cydui::style

export namespace cydui::style {
  std::vector<tss::StyleRuleInstance> instantiate_rules(
    components::mounted_component_t& component,
    tss::StyleArchive&               style_archive
  ) {
    std::vector<tss::StyleRuleInstance> style_rules{};

    style_archive.for_each_rule(component.get_name(), [&](const tss::StyleRule::sptr& rule) {
      for (const auto& selector: rule->selectors_) {
        if (check_style_comb_selector(component, selector)) {
          style_rules.emplace_back(selector.specificity(), rule);
          break;
        }
      }
    });

    std::stable_sort(
      style_rules.begin(),
      style_rules.end(),
      [](const tss::StyleRuleInstance& lhs, const tss::StyleRuleInstance& rhs) {
        return lhs.specificity < rhs.specificity;
      }
    );

    return style_rules;
  }

  void compile_style_rule_list(
    components::mounted_component_t& component,
    tss::StyleArchive&               style_archive
  ) {
    const std::vector<tss::StyleRuleInstance> style_rules{
      instantiate_rules(component, style_archive)
    };
    component.get_style_stack().update_rule_list(style_rules);
  }

  bool check_style_comb_selector_vector(
    components::mounted_component_t&                   component,
    const std::vector<tss::StyleRuleCombinedSelector>& selectors,
    bool                                               check_tags = false,
    bool check_pseudo_states                                      = false
  ) {
    ZoneScopedN("Check Selectors");
    for (const auto& selector: selectors) {
      if (check_style_comb_selector(component, selector, check_tags, check_pseudo_states)) {
        return true;
      }
    }
    return false;
  }
} // namespace cydui::style
