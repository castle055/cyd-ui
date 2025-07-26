// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.blueprint.base:content;

import std;
import fabric.logging;


export namespace cydui::core {
  class blueprint_base_t;

  class content_element final: public std::unique_ptr<blueprint_base_t> {
  public:
    //! @note We do NOT want this to be an explicit constructor.
    template <typename BlueprintType>
    content_element(std::unique_ptr<BlueprintType> blueprint)
        : std::unique_ptr<blueprint_base_t>(std::move(blueprint)) {}

    //! @note We do NOT want this to be an explicit constructor.
    template <typename BlueprintType>
    content_element(const BlueprintType& blueprint)
        : std::unique_ptr<blueprint_base_t>(new BlueprintType{blueprint}) {}
  };

  class content_type: public std::vector<content_element> {
  public:
    template <typename... Args>
    content_type(Args&&... args) {
      (add_element(std::forward<Args>(args)), ...);
    }

    ~content_type();

    content_type(const content_type& other);
    content_type& operator=(const content_type& other);

  private:
    template <typename Arg>
    void add_element(Arg&& arg) {
      if constexpr (std::is_same_v<Arg, content_type>) {
        for (const auto& blueprint: arg) {
          this->emplace_back(blueprint->clone());
        }
      } else {
        this->emplace_back(arg.clone());
      }
    }
  };
} // namespace cydui::components
