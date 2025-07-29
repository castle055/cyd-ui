// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling;

import std;
import reflect;

export import cydui.core.Component.impl;
export import cydui.styling.lang;


export namespace cydui::style {
  std::vector<tss::StyleRuleInstance> instantiate_rules(
    detail::ComponentImpl& component,
    tss::StyleArchive&   style_archive
  );

  void compile_style_rule_list(
    detail::ComponentImpl& component,
    tss::StyleArchive&   style_archive
  );

  bool check_style_comb_selector_vector(
    detail::ComponentImpl&                               component,
    const std::vector<tss::StyleRuleCombinedSelector>& selectors,
    bool                                               check_tags = false,
    bool check_pseudo_states                                      = false
  );
} // namespace cydui::style

export namespace cydui::style {
  bool apply_style(detail::ComponentImpl& component);
} // namespace cydui::style
