// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

module cydui.styling;

import std;
import reflect;
import fabric.logging;
import fabric.wiring.signals;

import cydui.styling.lang;

using namespace cydui::style;
using namespace cydui::detail;
using namespace cydui;

bool check_style_selector(
  ComponentImpl&                component,
  const tss::StyleRuleSelector& selector,
  bool                          check_tags,
  bool                          check_pseudo_states
) {
  ZoneScopedN("Check Selector");
  if (component.get_name() != selector.component) {
    return false;
  }

  if (check_tags) {
    for (const auto& tag: selector.tags) {
      if (not component.get_blueprint().has_tag(tag)) {
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
  ComponentImpl&                        component,
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

  ComponentImpl* current = &component;
  while (it != selector.selectors.rend()) {
    if (kind == tss::StyleRuleCombinedSelector::CHILD_COMBINATOR) {
      auto& parent = *current->get_parent_impl();
      if (not current->is_root()
          and check_style_selector(parent, it->second, check_tags, check_pseudo_states)) {
        current = &parent;
      } else {
        return false;
      }
    } else if (kind == tss::StyleRuleCombinedSelector::DESCENDENT_COMBINATOR) {
      bool found = false;
      while (not current->is_root()) {
        auto& parent = *current->get_parent_impl();
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

std::vector<tss::StyleRuleInstance> style::instantiate_rules(
  ComponentImpl&     component,
  tss::StyleArchive& style_archive
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

void style::compile_style_rule_list(
  ComponentImpl&     component,
  tss::StyleArchive& style_archive
) {
  const std::vector<tss::StyleRuleInstance> style_rules{instantiate_rules(component, style_archive)
  };
  component.get_style_stack().update_rule_list(style_rules);
}

bool style::check_style_comb_selector_vector(
  ComponentImpl&                                     component,
  const std::vector<tss::StyleRuleCombinedSelector>& selectors,
  bool                                               check_tags,
  bool                                               check_pseudo_states
) {
  ZoneScopedN("Check Selectors");
  for (const auto& selector: selectors) {
    if (check_style_comb_selector(component, selector, check_tags, check_pseudo_states)) {
      return true;
    }
  }
  return false;
}

bool style::apply_style(ComponentImpl& component) {
  auto& style = component.get_style_stack();

  style.manage_rules([&](const tss::StyleRuleInstance& rule) -> bool {
    return check_style_comb_selector_vector(component, rule.rule->selectors_, true, true);
  });
  bool style_changed = style.apply();

  return style_changed;
}
