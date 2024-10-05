/*! \file  dimension.cppm
 *! \brief 
 *!
 */

export module cydui.dimensions:context;

import std;

import fabric.logging;

import :types;

template <typename Type>
class cyd::ui::dimensions::context {
public:
  using value_type = Type;
  using getter_t   = std::function<dimension<value_type>()>;

  void set_parameter(const std::string& name, auto&& getter) {
    getters[name] = getter;
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

  getter_t& operator[](const std::string& name) {
    return getters.at(name);
  }
private:
  std::unordered_map<std::string, getter_t> getters{};
};