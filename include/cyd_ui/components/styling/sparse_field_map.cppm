// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.sparse_field_map;

import std;
export import reflect;
import fabric.logging;

export namespace cydui::style {
  class sparse_field_map {
    [[refl::ignore]]
    const refl::type_info&                          type_;
    std::unordered_map<refl::field_path, refl::any> fields_{};

  public:
    explicit sparse_field_map(const refl::type_info& type)
        : type_(type) {}

    sparse_field_map(const sparse_field_map& other)
        : type_(other.type_),
          fields_(other.fields_) {}

    sparse_field_map& operator=(const sparse_field_map& other) {
      this->fields_ = other.fields_;
      return *this;
    }

    sparse_field_map& operator=(sparse_field_map&& other) {
      this->fields_.swap(other.fields_);
      return *this;
    }

    bool erase(const refl::field_path& field_path) {
      if (not assert_path_root_type(field_path))
        return false;
      const bool changed = fields_.contains(field_path);
      fields_.erase(field_path);
      return changed;
    }

    bool set(
      const refl::field_path& field_path,
      const refl::any&        value
    ) {
      if (not assert_path_root_type(field_path))
        return false;
      const bool changed  = (not fields_.contains(field_path)) or (fields_[field_path] != value);
      fields_[field_path] = value;
      return changed;
    }

    std::optional<refl::any_ref> get(const refl::field_path& field_path) {
      if (not assert_path_root_type(field_path))
        return std::nullopt;
      refl::field_path path = field_path;
      while (path.depth() > 0) {
        if (fields_.contains(path)) {
          if (path == field_path) {
            return fields_.at(path);
          } else {
            refl::field_path rel_path{path.type()};
            for (auto it = std::next(field_path.begin(), path.depth()); it != field_path.end();
                 ++it) {
              rel_path = rel_path.append(*it);
            }
            refl::any&             p_val = fields_.at(path);
            void*                  ptr   = rel_path.get_ptr(p_val.data());
            const refl::type_info& ti    = field_path.type();
            return refl::any_ref{ti, ptr};
          }
        }
        path = path.parent();
      }

      return std::nullopt;
    }

    std::optional<refl::any> get(const refl::field_path& field_path) const {
      if (not assert_path_root_type(field_path))
        return std::nullopt;
      refl::field_path path = field_path;
      while (path.depth() > 0) {
        if (fields_.contains(path)) {
          if (path == field_path) {
            return fields_.at(path);
          } else {
            refl::field_path rel_path{path.type()};
            for (auto it = std::next(field_path.begin(), path.depth()); it != field_path.end();
                 ++it) {
              rel_path = rel_path.append(*it);
            }
            const refl::any& p_val = fields_.at(path);
            const void*      ptr   = rel_path.get_ptr(p_val.data());

            const refl::type_info& ti      = field_path.type();
            void*                  cpy_ptr = ti.make_copy_of(ptr);
            return refl::any::make(ti, cpy_ptr);
          }
        }
        path = path.parent();
      }

      return std::nullopt;
    }

    bool contains_value(const refl::field_path& field_path) const {
      if (not assert_path_root_type(field_path))
        return false;
      refl::field_path path = field_path;
      while (path.depth() > 0) {
        if (fields_.contains(path)) {
          return true;
        }
        path = path.parent();
      }
      return false;
    }

    bool contains_specific_value(const refl::field_path& field_path) const {
      if (not assert_path_root_type(field_path))
        return false;
      return fields_.contains(field_path);
    }

    auto begin() {
      return fields_.begin();
    }

    auto end() {
      return fields_.end();
    }

    bool operator==(const sparse_field_map& other) const {
      return other.type_ == type_ and other.fields_ == fields_;
    }

    void clear() {
      fields_.clear();
    }

    const refl::type_info& get_type() const {
      return type_;
    }

    std::optional<refl::field_path> find_path(const std::string& field_path) const {
      return refl::field_path::from_string(type_, field_path);
    }

    bool empty() const {
      return fields_.empty();
    }

    std::size_t size() const {
      return fields_.size();
    }

  private:
    bool assert_path_root_type(const refl::field_path& path) const {
      if (path.root_type() != type_) {
        LOG::print{ERROR
        }("Field belongs to type '{}', expected '{}'", path.root_type().name(), type_.name());
        return false;
      }
      return true;
    }
  };
} // namespace cydui::style
