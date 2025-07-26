// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.sparse_style_map;

import std;
export import reflect;

export import cydui.styling.style_base;
export import cydui.styling.sparse_field_map;

export namespace cydui::style {
  class sparse_style_map {
    sparse_field_map base_fields_;
    sparse_field_map impl_fields_;

  public:
    explicit sparse_style_map(const refl::type_info& type);

    sparse_style_map(const sparse_style_map& other);

    sparse_style_map& operator=(const sparse_style_map& other);

    sparse_style_map& operator=(sparse_style_map&& other);

    bool operator==(const sparse_style_map& other) const;

    const refl::type_info& get_type() const;

    void apply_to_object(style_object_t& obj);

    void clear();

    std::optional<refl::field_path> find_path(const std::string& path_str) const;

    bool erase_path(const refl::field_path& path);

    bool set_field(
      const refl::field_path& path,
      const refl::any&        value
    );

    bool set_field(
      const std::string& path_str,
      const refl::any&   value
    );

    std::optional<refl::any_ref> get_field(const refl::field_path& path);

    std::optional<refl::any> get_field(const refl::field_path& path) const;

    std::optional<refl::any_ref> get_field(const std::string& path_str);

    std::optional<refl::any> get_field(const std::string& path_str) const;

    bool empty() const;

    std::size_t size() const;

  private:
    void apply_map(
      sparse_field_map& map,
      void*             obj
    );

    bool set_field_as_is(
      const refl::field_path& path,
      const refl::any&        value
    );
  };
} // namespace cydui::style
