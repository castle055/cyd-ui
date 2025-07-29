// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.blueprint.base:content;

import std;
import fabric.logging;
import fabric.match;

export namespace cydui {
  class Blueprint;
  class BlueprintList;
} // namespace cydui

export namespace cydui::detail {
  class BlueprintListElement final: public std::unique_ptr<Blueprint> {
  public:
    //! @note We do NOT want this to be an explicit constructor.
    template <typename BlueprintType>
    BlueprintListElement(std::unique_ptr<BlueprintType> blueprint)
        : std::unique_ptr<Blueprint>(std::move(blueprint)) {}

    //! @note We do NOT want this to be an explicit constructor.
    template <typename BlueprintType>
    BlueprintListElement(const BlueprintType& blueprint)
        : std::unique_ptr<Blueprint>(new BlueprintType {blueprint}) {}
  };

  template <typename T>
  concept ConstructsBlueprint =
    std::same_as<std::remove_cvref_t<T>, BlueprintList> //
    or std::derived_from<std::remove_cvref_t<T>, Blueprint>
    or (//
      fabric::is_match<std::remove_cvref_t<T>>//
      and (//
        std::same_as<typename std::remove_cvref_t<T>::result_type, BlueprintList>//
        or std::same_as<typename std::remove_cvref_t<T>::result_type, Blueprint>));
} // namespace cydui::detail

export namespace cydui {
  class BlueprintList: public std::list<detail::BlueprintListElement> {
  public:
    ~BlueprintList();

    BlueprintList(const BlueprintList& other);
    BlueprintList& operator=(const BlueprintList& other);

    template <detail::ConstructsBlueprint... Args>
    BlueprintList(const Args&... args) {
      (add_element(args), ...);
    }

  private:
    void add_element(const BlueprintList& arg);
    void add_element(const Blueprint& arg);
    void add_element(const fabric::is_match auto& arg) {
      add_element(*arg);
    }
  };
} // namespace cydui
