// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.style_override;

import std;
export import reflect;

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
    );

    void set_field_override(
      const refl::field_path& field_info,
      const refl::any&        value
    );

    bool has_base_field_override(const refl::field_path& field_info);

    bool has_field_override(const refl::field_path& field_info);

    void clear_style_override();

    bool update_override_with(const style_override& other);

    const style_override_data_t& get_style_override() const;
  };
} // namespace cydui::style
