// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.style_override;

import std;
export import reflect;
import fabric.logging;

export import cydui.styling.sparse_style_map;

export namespace cydui::style {
  struct style_override_data_t {
    void clear() {
      base_fields.clear();
      fields.clear();
    }

    bool operator==(const style_override_data_t& other) const {
      return base_fields == other.base_fields and fields == other.fields;
    }

    std::unordered_map<refl::field_path, refl::any> base_fields{};
    std::unordered_map<refl::field_path, refl::any> fields{};
  };

  class style_override {
    style_override_data_t style_override_data{};

  public:
    void set_base_field_override(
      const refl::field_path& field_info,
      const refl::any&        value
    ) {
      style_override_data.base_fields[field_info] = value;
    }

    void set_field_override(
      const refl::field_path& field_info,
      const refl::any&        value
    ) {
      style_override_data.fields[field_info] = value;
    }

    bool has_base_field_override(const refl::field_path& field_info) {
      return style_override_data.base_fields.contains(field_info);
    }

    bool has_field_override(const refl::field_path& field_info) {
      return style_override_data.fields.contains(field_info);
    }

    void clear_style_override() {
      style_override_data.clear();
    }

    bool update_override_with(const style_override& other) {
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

    const auto& get_style_override() const {
      return style_override_data;
    }
  };
} // namespace cydui::components
