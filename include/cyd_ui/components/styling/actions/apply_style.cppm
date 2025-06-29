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

namespace cydui::styling {
  // bool apply_rule_property(
  //   components::mounted_component_t& component,
  //   void*                            style_obj,
  //   const refl::field_info*          field,
  //   const refl::any&                 value
  // ) {
  //   ZoneScopedN("Apply Property");
  //   if (value.is(field->type())) {
  //     field->type().assign_copy_of(value.data(), field->get_ptr(style_obj));
  //     return true;
  //   }
  //
  //   if (value.is(refl::type_info::from<std::string>())
  //       and field->has_metadata<custom_style_parser>()) {
  //     std::string str = value.as<std::string>();
  //
  //     refl::any rule_field_parsed = field->get_metadata<custom_style_parser>().parser_function(str);
  //     if (rule_field_parsed.is(field->type())) {
  //       field->type().assign_copy_of(rule_field_parsed.data(), field->get_ptr(style_obj));
  //       return true;
  //     } else {
  //       LOG::print{WARN} //
  //       ("Custom parser type mismatch '{}::{}', expected: '{}', found: '{}'",
  //        component.get_name(),
  //        field->name,
  //        field->type().name(),
  //        rule_field_parsed.type().name());
  //     }
  //   }
  //
  //   if (field->type().is_type<vg::paint::type>()) {
  //     if (value.is<color::Color>()) {
  //       vg::paint::type&    paint       = field->get_ref<vg::paint::type>(style_obj);
  //       const color::Color& paint_color = value.as<color::Color>();
  //       paint                           = vg::paint::type::make(vg::paint::solid{paint_color});
  //       return true;
  //     }
  //   }
  //
  //   LOG::print{WARN} //
  //   ("Type mismatch '{}::{}', expected: '{}', found: '{}'",
  //    component.get_name(),
  //    field->name,
  //    field->type().name(),
  //    value.type().name());
  //
  //   return false;
  // }
  //
  // bool apply_rule(
  //   components::mounted_component_t&                       component,
  //   void*                                                  style_obj,
  //   std::unordered_set<const refl::field_info*>::iterator& field_it,
  //   std::unordered_set<const refl::field_info*>&           pending_fields,
  //   const StyleRule::sptr&                                 rule
  // ) {
  //   ZoneScopedN("Apply Rule");
  //   const auto* field = *field_it;
  //   if (rule->properties_.contains(field->name)) {
  //     const auto& rule_field = rule->properties_.at(field->name);
  //
  //     if (apply_rule_property(component, style_obj, *field_it, rule_field)) {
  //       field_it = pending_fields.erase(field_it);
  //       return true;
  //     }
  //   }
  //   ++field_it;
  //   return false;
  // }
  //
  // void apply_style_rules(components::mounted_component_t& component) {
  //   ZoneScopedN("Apply Rules");
  //   auto&       style_data  = component.get_style_data();
  //   auto&       style_rules = style_data.rules;
  //   const auto& bti         = refl::type_info::from<components::style_base_t>();
  //   const auto& ti          = component.get_blueprint()->get_style_type_info();
  //
  //   std::unordered_set<const refl::field_info*> pending_base_fields{};
  //   for (const auto& bfti: bti.fields()) {
  //     pending_base_fields.insert(&bfti);
  //   }
  //   std::unordered_set<const refl::field_info*> pending_fields{};
  //   for (const auto& fti: ti.fields()) {
  //     pending_fields.insert(&fti);
  //   }
  //
  //   // keep track of which fields where just activated from rules being deactivated
  //   std::unordered_set<const refl::field_info*> activated_base_fields{};
  //   std::unordered_set<const refl::field_info*> activated_fields{};
  //
  //   // keep track of which fields where just deactivated from rules being deactivated
  //   std::unordered_set<const refl::field_info*> deactivated_base_fields{};
  //   std::unordered_set<const refl::field_info*> deactivated_fields{};
  //
  //   components::style_base_t& base_s = component.get_style();
  //   for (auto& rule_instance: style_rules) {
  //     auto& [specificity, rule, is_active, _, __] = rule_instance;
  //     const bool should_be_active =
  //       check_style_comb_selector_vector(component, rule->selectors_, true, true);
  //     if (not is_active and should_be_active) {
  //       // Activate rule
  //       ZoneScopedN("Activate Rule");
  //       //! Base fields
  //       for (auto field_it = pending_base_fields.begin(); field_it != pending_base_fields.end();) {
  //         const refl::field_info* field = *field_it;
  //         if (apply_rule(component, &base_s, field_it, pending_base_fields, rule)) {
  //           rule_instance.active_base_properties.insert(field);
  //           activated_base_fields.insert(field);
  //         }
  //       }
  //       //! Custom fields
  //       for (auto field_it = pending_fields.begin(); field_it != pending_fields.end();) {
  //         const refl::field_info* field = *field_it;
  //         if (apply_rule(component, style_data.as_ptr(), field_it, pending_fields, rule)) {
  //           rule_instance.active_properties.insert(field);
  //           activated_fields.insert(field);
  //         }
  //       }
  //
  //       rule_instance.active = true;
  //     } else if (is_active and not should_be_active) {
  //       ZoneScopedN("Deactivate Rule");
  //       // Deactivate rule
  //       for (const auto& prop: rule_instance.active_base_properties) {
  //         deactivated_base_fields.insert(prop);
  //       }
  //       for (const auto& prop: rule_instance.active_properties) {
  //         deactivated_fields.insert(prop);
  //       }
  //       rule_instance.active_base_properties.clear();
  //       rule_instance.active_properties.clear();
  //       rule_instance.active = false;
  //     } else if (is_active and should_be_active) {
  //       // Activate rule
  //       ZoneScopedN("Update Rule");
  //       //! Remove fields that have been overriden by a more specific rule
  //       //! Base fields
  //       for (auto field_it = activated_base_fields.begin(); field_it != activated_base_fields.end();
  //            ++field_it) {
  //         rule_instance.active_base_properties.erase(*field_it);
  //       }
  //       //! Custom fields
  //       for (auto field_it = activated_fields.begin(); field_it != activated_fields.end();
  //            ++field_it) {
  //         rule_instance.active_properties.erase(*field_it);
  //       }
  //
  //       //! Check-apply fields that are no longer being overriden by a more specific rule
  //       //! Base fields
  //       for (auto field_it = deactivated_base_fields.begin();
  //            field_it != deactivated_base_fields.end();) {
  //         const refl::field_info* field = *field_it;
  //         if (apply_rule(component, &base_s, field_it, deactivated_base_fields, rule)) {
  //           rule_instance.active_base_properties.insert(field);
  //         }
  //       }
  //       //! Custom fields
  //       for (auto field_it = deactivated_fields.begin(); field_it != deactivated_fields.end();) {
  //         const refl::field_info* field = *field_it;
  //         if (apply_rule(component, style_data.as_ptr(), field_it, deactivated_fields, rule)) {
  //           rule_instance.active_properties.insert(field);
  //         }
  //       }
  //
  //       //! Remove pending fields that are active in this rule
  //       //! Base fields
  //       for (auto field_it = pending_base_fields.begin(); field_it != pending_base_fields.end();) {
  //         if (rule_instance.active_base_properties.contains(*field_it)) {
  //           field_it = pending_base_fields.erase(field_it);
  //         } else {
  //           ++field_it;
  //         }
  //       }
  //       //! Custom fields
  //       for (auto field_it = pending_fields.begin(); field_it != pending_fields.end();) {
  //         if (rule_instance.active_properties.contains(*field_it)) {
  //           field_it = pending_fields.erase(field_it);
  //         } else {
  //           ++field_it;
  //         }
  //       }
  //     }
  //   }
  //
  //   // Set deactivated fields to default values
  //   for (const auto& field: deactivated_base_fields) {
  //     style_data.reset_field(field, true);
  //   }
  //   for (const auto& field: deactivated_fields) {
  //     style_data.reset_field(field, false);
  //   }
  // }
} // namespace cydui::styling

export namespace cydui::styling {
  bool apply_style(components::mounted_component_t& component) {
    auto& style      = component.get_style_stack();

    style.manage_rules([&](const tss::StyleRuleInstance& rule) -> bool {
      return check_style_comb_selector_vector(component, rule.rule->selectors_, true, true);
    });
    bool style_changed = style.apply();

    return style_changed;
  }
} // namespace cydui::styling
