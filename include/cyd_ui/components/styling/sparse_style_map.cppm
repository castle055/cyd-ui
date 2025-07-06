// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.sparse_style_map;

import std;
export import reflect;
import fabric.logging;

export import cydui.styling.style_base;
export import cydui.styling.sparse_field_map;

export namespace cydui::style {
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
              if (cc.type_name == value.type().name()) {
                return set_field_as_is(path, cc.converter(value));
              }
            }
          }
        }

        // Try type specific conversion
        const std::string from_t        = value.type().name();
        const std::string to_t          = path.back()->type().name();
        auto                  rt_conversion = style::get_custom_type_conversion(from_t, to_t);
        if (rt_conversion.has_value()) {
          return set_field_as_is(path, rt_conversion.value().converter(value));
        }

        // Try reflecting into type
        if (from_t == refl::type_name<std::vector<refl::any>>) {
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
        } else if (from_t == refl::type_name<refl::archive>) {
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

    bool empty() const {
      return base_fields_.empty() and impl_fields_.empty();
    }

    std::size_t size() const {
      return base_fields_.size() + impl_fields_.size();
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
} // namespace cydui::components
