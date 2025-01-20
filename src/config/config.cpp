// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cyd_ui/tools/config.hpp"

#ifndef CYD_UI_CONFIG_HPP

using namespace cyd::ui::config;

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
