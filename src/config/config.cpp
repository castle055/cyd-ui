//
// Created by castle on 1/4/23.
//

#include "tools/config.hpp"

#ifndef CYD_UI_CONFIG_HPP

using namespace cydui::config;

ConfigManager::ConfigManager(std::string path)
  : path(std::move(path)) {
}

void ConfigManager::parse_raw() {
  try {
    auto file = YAML::LoadFile(path);
    deserialize(file);
  } catch (YAML::BadFile &e) {
  
  }
}

void ConfigManager::refresh() {
  parse_raw();
}

void ConfigManager::update_config_file() {

}
#endif //CYD_UI_CONFIG_HPP
