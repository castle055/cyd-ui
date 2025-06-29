// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.style_override;

import std;
export import reflect;
import fabric.logging;

export import cydui.styling.style_base;

export namespace cydui::components {
  class sparse_field_map {
    [[refl::ignore]]
    const refl::type_info&                type_;
    std::map<refl::field_path, refl::any> fields_{};

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
            auto                   rel_path = field_path.relative_to(path);
            refl::any&             p_val    = fields_.at(path);
            void*                  ptr      = rel_path.get_ptr(p_val.data());
            const refl::type_info& ti       = field_path.type();
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
            auto             rel_path = field_path.relative_to(path);
            const refl::any& p_val    = fields_.at(path);
            const void*      ptr      = rel_path.get_ptr(p_val.data());

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

  class sparse_style_map {
    sparse_field_map base_fields_;
    sparse_field_map impl_fields_;

  public:
    explicit sparse_style_map(const refl::type_info& type)
        : base_fields_(refl::type_info::from<style_base_t>()),
          impl_fields_(type) {}

    sparse_style_map(const sparse_style_map& other)
        : base_fields_(other.base_fields_),
          impl_fields_(other.impl_fields_) {}

    sparse_style_map& operator=(const sparse_style_map& other) {
      if (other.get_type().id() != this->get_type().id()) {
        LOG::print{ERROR
        }("Assigning style map of type {} to map of type {}",
          other.get_type().name(),
          this->get_type().name());
        return *this;
      }
      this->base_fields_ = other.base_fields_;
      this->impl_fields_ = other.impl_fields_;
      return *this;
    }

    sparse_style_map& operator=(sparse_style_map&& other) {
      if (other.get_type().id() != this->get_type().id()) {
        LOG::print{ERROR
        }("Assigning style map of type {} to map of type {}",
          other.get_type().name(),
          this->get_type().name());
        return *this;
      }
      this->base_fields_ = std::move(other.base_fields_);
      this->impl_fields_ = std::move(other.impl_fields_);
      return *this;
    }

    bool operator==(const sparse_style_map& other) const {
      return base_fields_ == other.base_fields_ and impl_fields_ == other.impl_fields_;
    }

    const refl::type_info& get_type() const {
      return impl_fields_.get_type();
    }

    void apply_to_object(style_object_t& obj) {
      apply_map(base_fields_, &obj.get_base());
      apply_map(impl_fields_, obj.get_impl());
    }

    void clear() {
      base_fields_.clear();
      impl_fields_.clear();
    }

    std::optional<refl::field_path> find_path(const std::string& path_str) const {
      return base_fields_.find_path(path_str).or_else([&] {
        return impl_fields_.find_path(path_str);
      });
    }

    bool erase_path(const refl::field_path& path) {
      bool changed = false;
      if (path.root_type().id() == refl::type_id<style_base_t>) {
        changed = base_fields_.erase(path);
      } else {
        changed = impl_fields_.erase(path);
      }

      return changed;
    }

    bool set_field(
      const refl::field_path& path,
      const refl::any&        value
    ) {
      if (not value.is(path.type())) {
        // Try field specific conversion
        if (path.back()->has_metadata<const CustomConversion>()) {
          for (const auto& [t_info, ptr]: path.back()->metadata) {
            if (t_info().id() == refl::type_id<const CustomConversion>) {
              const auto& cc = *static_cast<const CustomConversion*>(ptr);
              if (cc.type_id == value.type().id()) {
                return set_field_as_is(path, cc.converter(value));
              }
            }
          }
        }

        // Try type specific conversion
        const refl::type_id_t from_t        = value.type().id();
        const refl::type_id_t to_t          = path.back()->type().id();
        auto                  rt_conversion = style::get_custom_type_conversion(from_t, to_t);
        if (rt_conversion.has_value()) {
          return set_field_as_is(path, rt_conversion.value().converter(value));
        }

        // Try reflecting into type
        if (from_t == refl::type_id<std::vector<refl::any>>) {
          const auto& expr_vector = value.as<std::vector<refl::any>>();
          const auto& ti          = path.back()->type();
          if (not ti.fields().empty()) {
            if (ti.fields().size() >= expr_vector.size()) {
              auto it      = ti.fields().begin();
              bool changed = false;
              for (std::size_t i = 0; i < expr_vector.size(); ++i) {
                refl::field_path inner_path{path};
                inner_path  = inner_path.append(&(*it));
                changed    |= set_field(inner_path, expr_vector[i]);
                ++it;
              }
              return changed;
            }

            LOG::print{ERROR
            }("({}) expected type '{}', found too many expressions",
              path.to_string(),
              path.type().name());
          }
        } else if (from_t == refl::type_id<refl::archive>) {
          const auto& archive = value.as<refl::archive>();
          bool        changed = false;
          for (const auto& [item_name, item_value]: archive) {
            refl::field_path inner_path{path};
            auto             field = path.type().field_by_name(item_name);
            if (field.has_value()) {
              inner_path  = inner_path.append(field.value());
              changed    |= set_field(inner_path, item_value);
            } else {
              LOG::print{WARN
              }("({}) unknown field '{}' in '{}'", path.to_string(), item_name, path.type().name());
            }
          }
          return changed;
        }

        LOG::print{ERROR
        }("({}) expected type '{}', found '{}'",
          path.to_string(),
          path.type().name(),
          value.type().name());
        return false;
      } else {
        return set_field_as_is(path, value);
      }
    }

    bool set_field(
      const std::string& path_str,
      const refl::any&   value
    ) {
      std::optional<refl::field_path> path_opt = find_path(path_str);
      if (not path_opt.has_value()) {
        LOG::print{WARN
        }("Unknown style field path: ({}|{})::{}",
          base_fields_.get_type().name(),
          impl_fields_.get_type().name(),
          path_str);
        return false;
      }
      refl::field_path path = path_opt.value();
      return set_field(path, value);
    }

    std::optional<refl::any_ref> get_field(const refl::field_path& path) {
      if (path.root_type().id() == refl::type_id<style_base_t>) {
        return base_fields_.get(path);
      } else {
        return impl_fields_.get(path);
      }
    }

    std::optional<refl::any> get_field(const refl::field_path& path) const {
      if (path.root_type().id() == refl::type_id<style_base_t>) {
        return base_fields_.get(path);
      } else {
        return impl_fields_.get(path);
      }
    }

    std::optional<refl::any_ref> get_field(const std::string& path_str) {
      std::optional<refl::field_path> path_opt = find_path(path_str);
      if (not path_opt.has_value()) {
        LOG::print{WARN
        }("Unknown style field path: ({}|{})::{}",
          base_fields_.get_type().name(),
          impl_fields_.get_type().name(),
          path_str);
        return std::nullopt;
      }
      return get_field(path_opt.value());
    }

    std::optional<refl::any> get_field(const std::string& path_str) const {
      std::optional<refl::field_path> path_opt = find_path(path_str);
      if (not path_opt.has_value()) {
        LOG::print{WARN
        }("Unknown style field path: ({}|{})::{}",
          base_fields_.get_type().name(),
          impl_fields_.get_type().name(),
          path_str);
        return std::nullopt;
      }
      return get_field(path_opt.value());
    }

  private:
    void apply_map(
      sparse_field_map& map,
      void*             obj
    ) {
      for (const auto& [path, value]: map) {
        void*                  dst      = path.get_ptr(obj);
        const refl::type_info& field_ti = path.type();
        field_ti.assign_copy_of(value.data(), dst);
      }
    }

    bool set_field_as_is(
      const refl::field_path& path,
      const refl::any&        value
    ) {
      bool changed = false;
      if (path.root_type().id() == refl::type_id<style_base_t>) {
        changed = base_fields_.set(path, value);
      } else {
        changed = impl_fields_.set(path, value);
      }
      return changed;
    }
  };

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
