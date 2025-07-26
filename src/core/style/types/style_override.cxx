// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module cydui.styling.style_override;

import std;
import reflect;
import fabric.logging;
import cydui.styling.sparse_style_map;

using namespace cydui::style;


void style_override::set_base_field_override(
  const refl::field_path& field_info,
  const refl::any&        value
) {
  style_override_data.base_fields[field_info] = value;
}

void style_override::set_field_override(
  const refl::field_path& field_info,
  const refl::any&        value
) {
  style_override_data.fields[field_info] = value;
}

bool style_override::has_base_field_override(const refl::field_path& field_info) {
  return style_override_data.base_fields.contains(field_info);
}

bool style_override::has_field_override(const refl::field_path& field_info) {
  return style_override_data.fields.contains(field_info);
}

void style_override::clear_style_override() {
  style_override_data.clear();
}

bool style_override::update_override_with(const style_override& other) {
  const auto& override_data     = other.style_override_data;
  bool        something_changed = false;
  for (const auto& base_field: override_data.base_fields) {
    if ((not style_override_data.base_fields.contains(base_field.first))//
        or (style_override_data.base_fields.contains(base_field.first)
          and base_field.second != style_override_data.base_fields.at(base_field.first))) {
      something_changed = true;
    }
    style_override_data.base_fields.insert(base_field);
  }
  for (const auto& field: override_data.fields) {
    if ((not style_override_data.fields.contains(field.first))//
        or (style_override_data.fields.contains(field.first)
          and field.second != style_override_data.fields.at(field.first))) {
      something_changed = true;
    }
    style_override_data.fields.insert(field);
  }
  return something_changed;
}

const style_override_data_t& style_override::get_style_override() const {
  return style_override_data;
}
