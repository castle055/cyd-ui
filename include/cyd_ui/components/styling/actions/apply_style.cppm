// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>
#include "cyd_fabric_modules/headers/macros/async_events.h"

export module cydui.styling.actions.apply_style;

import std;
import reflect;
import fabric.logging;
import fabric.wiring.signals;

import cydui.application;
export import cydui.components.mounted;
export import cydui.styling.lang;
import cydui.styling.actions.compile_style_rule_list;


export namespace cydui::style {
  bool apply_style(components::mounted_component_t& component) {
    auto& style      = component.get_style_stack();

    style.manage_rules([&](const tss::StyleRuleInstance& rule) -> bool {
      return check_style_comb_selector_vector(component, rule.rule->selectors_, true, true);
    });
    bool style_changed = style.apply();

    return style_changed;
  }
} // namespace cydui::styling
