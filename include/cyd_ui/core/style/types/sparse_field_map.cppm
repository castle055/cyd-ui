// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.sparse_field_map;

import std;
export import reflect;

export namespace cydui::style {
  class sparse_field_map {
    [[refl::ignore]]
    const refl::type_info&                          type_;
    std::unordered_map<refl::field_path, refl::any> fields_{};

  public:
    explicit sparse_field_map(const refl::type_info& type);
    sparse_field_map(const sparse_field_map& other);

    sparse_field_map& operator=(const sparse_field_map& other);
    sparse_field_map& operator=(sparse_field_map&& other);

    bool operator==(const sparse_field_map& other) const;

    bool set(
      const refl::field_path& field_path,
      const refl::any&        value
    );
    std::optional<refl::any_ref> get(const refl::field_path& field_path);
    std::optional<refl::any>     get(const refl::field_path& field_path) const;
    bool                         erase(const refl::field_path& field_path);
    void                         clear();

    bool contains_value(const refl::field_path& field_path) const;
    bool contains_specific_value(const refl::field_path& field_path) const;

    std::unordered_map<
      refl::field_path,
      refl::any>::iterator
    begin();
    std::unordered_map<
      refl::field_path,
      refl::any>::iterator
    end();

    bool        empty() const;
    std::size_t size() const;

    std::optional<refl::field_path> find_path(const std::string& field_path) const;
    const refl::type_info&          get_type() const;

  private:
    bool assert_path_root_type(const refl::field_path& path) const;
  };
} // namespace cydui::style
