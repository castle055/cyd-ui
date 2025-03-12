/*! \file  dimension.cppm
 *! \brief
 *!
 */

export module cydui.dimensions:context;

import std;

import fabric.logging;

export import :types;

export template <typename Type>
class cydui::dimensions::context {
public:
  using value_type = Type;

  template <typename V>
  void set_parameter(const std::string& name, V&& getter) {
    if (getters.contains(name)) {
      getters.at(name) = getter;
    } else {
      getters.emplace(name, std::forward<V>(getter));
    }
  }

  void delete_parameter(const std::string& name) {
    getters.erase(name);
  }

  void clear_parameters() {
    getters.clear();
  }

  std::size_t count_parameters() const {
    return getters.size();
  }

  bool contains(const std::string& name) const {
    // LOG::print{INFO}("{{");
    // for (const auto& [name, g]: getters) {
    //   LOG::print{INFO}("  {},", name);
    // }
    // LOG::print{INFO}("}}");
    return getters.contains(name);
  }

  auto& operator[](const std::string& name) {
    return getters.at(name);
  }

private:
  std::unordered_map<std::string, dimension<value_type>> getters{};
};
