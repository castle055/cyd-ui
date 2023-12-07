//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_HOLDER_H
#define CYD_UI_COMPONENT_HOLDER_H

#include "with_template.h"
#include "memory"

namespace cydui::components {
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
        components[""] = new C {comp};
        //components[""] = std::make_unique<C>(comp);
      }
      
      template<typename T>
      inline component_holder_t(with::with<T> _with) { // NOLINT(*-explicit-constructor)
        for (const auto &item: _with.get_selection()) {
          components[item.first] = item.second;
        }
      }
      
      const auto &get_components() const {
        return components;
      }
    private:
      std::unordered_map<std::string, component_base_t*> components {};
    };
}

#endif //CYD_UI_COMPONENT_HOLDER_H
