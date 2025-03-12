// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.base:holder;

import std;
export import :type;

export
{
  namespace cydui::components {
    struct component_builder_t {

      component_builder_t() = default;

      template<ComponentConcept C>
      component_builder_t(C comp) {
        components.emplace_back([=] {
          return std::shared_ptr<component_base_t> {new C {comp}};
        });
      }

      std::vector<std::shared_ptr<component_base_t>> get_components() const {
        std::vector<std::shared_ptr<component_base_t>> _components { };
        for (const auto &builder: components) {
          _components.emplace_back(builder());
        }
        return _components;
      }

      const auto &get_component_constructors() const {
        return components;
      }

      void append_component(const std::function<std::shared_ptr<component_base_t>()>& component) {
        components.emplace_back(component);
      }

      void transform(auto&& fun) {
        for (auto &builder: components) {
          auto original_builder = builder;
          builder = [=]() {
            return fun(original_builder());
          };
        }
      }

      bool empty() const {
        return components.empty();
      }

      std::size_t size() const {
        return components.size();
      }

    private:
      std::vector<std::function<std::shared_ptr<component_base_t>()>> components { };
    };

    struct component_holder_t {
      //template<ComponentConcept C>
      //inline component_holder_t(C &&comp) {
      //  components[""] = new C {comp};
      //  //components[""] = std::make_unique<C>(comp);
      //}
      //
      //template<ComponentConcept C>
      //inline component_holder_t(C &comp) {
      //  components[""] = new C {comp};
      //  //components[""] = std::make_unique<C>(comp);
      //}
      component_holder_t() = default;

      template<ComponentConcept ...C>
      component_holder_t(C... comps) noexcept {
        (components.emplace_back(std::shared_ptr<component_base_t>{new C{comps}}), ...);
      }

      template<ComponentConcept C>
      component_holder_t(std::vector<C> comps) noexcept {
        for (auto &comp: comps) {
          components.emplace_back(std::shared_ptr<component_base_t> {new C {comp}});
        }
      }

      template<ComponentConcept C>
      component_holder_t(C comp) {
        components.emplace_back(std::shared_ptr<component_base_t> {new C {comp}});
      }

      component_holder_t(const component_builder_t &builder) {
        for (const auto &item: builder.get_components()) {
          components.emplace_back(item);
        }
      }

      component_holder_t(const component_holder_t &other) {
        components.insert_range(components.end(), other.components);
      }

      component_holder_t(component_holder_t &&other) noexcept {
        components.insert_range(components.end(), std::move(other.components));
      }

      component_holder_t& operator=(const component_holder_t &other) {
        components = other.components;
        return *this;
      }

      component_holder_t& operator=(component_holder_t &&other) noexcept {
        components = std::move(other.components);
        return *this;
      }

      component_holder_t& append(const component_holder_t &other) {
        components.insert_range(components.end(), other.components);
        return *this;
      }

      component_holder_t(
        const std::vector<std::shared_ptr<component_base_t>> &components_
      ): components(components_) {
      }

      [[nodiscard]] auto &get_components() {
        return components;
      }

      [[nodiscard]] const auto &get_components() const {
        return components;
      }

      // Cannot work, since we would need to recursively copy any `component_holder_t` in the props
      // of each component.
      //component_holder_t clone() const {
      //
      //}

      void append_component(std::shared_ptr<component_base_t> component) {
        components.emplace_back(component);
      }

      auto begin() {
        return components.begin();
      }

      auto begin() const {
        return components.begin();
      }

      auto end() {
        return components.end();
      }

      auto end() const {
        return components.end();
      }

    private:
      std::vector<std::shared_ptr<component_base_t>> components { };
    };
  }
}
