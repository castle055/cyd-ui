//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_HOLDER_H
#define CYD_UI_COMPONENT_HOLDER_H

#include "with_template.h"
#include "memory"

namespace cydui::components {
    struct component_builder_t {
      template<ComponentConcept C>
      inline component_builder_t(C comp) { // NOLINT(*-explicit-constructor)
        components.emplace_back("", [=] {
          return new C {comp};
        });
        
        //components[""] = std::make_unique<C>(comp);
      }
      
      template<typename T>
      inline component_builder_t(with::with<T> _with) { // NOLINT(*-explicit-constructor)
        for (const auto &item: _with.get_selection()) {
          components.emplace_back(item);
        }
      }
      
      std::vector<std::pair<std::string, component_base_t*>> build_components() const {
        std::vector<std::pair<std::string, component_base_t*>> _components {};
        for (const auto &item: components) {
          const auto &[id, builder] = item;
          _components.emplace_back(id, builder());
        }
        return _components;
      }
      
      const auto &get_components() const {
        return components;
      }
    
    private:
      std::vector<std::pair<std::string, std::function<component_base_t*()>>> components {};
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
      
      template<ComponentConcept C>
      inline component_holder_t(C comp) { // NOLINT(*-explicit-constructor)
        components.emplace_back("", new C {comp});
        //components[""] = std::make_unique<C>(comp);
      }
      
      template<typename T>
      inline component_holder_t(with::with<T> _with) { // NOLINT(*-explicit-constructor)
        for (const auto &item: _with.get_selection()) {
          components.emplace_back(item.first, item.second());
        }
      }
      
      inline component_holder_t(const component_builder_t &builder) { // NOLINT(*-explicit-constructor)
        for (const auto &item: builder.build_components()) {
          components.emplace_back(item);
        }
      }
      
      [[nodiscard]] const auto &get_components() const {
        return components;
      }
      
      // Cannot work, since we would need to recursively copy any `component_holder_t` in the props
      // of each component.
      //component_holder_t clone() const {
      //
      //}
    private:
      std::vector<std::pair<std::string, component_base_t*>> components {};
    };
  
}

#endif //CYD_UI_COMPONENT_HOLDER_H
