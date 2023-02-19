//
// Created by castle on 2/16/23.
//

#ifndef CYD_UI_CONFIG_HPP
#define CYD_UI_CONFIG_HPP

#include <yaml-cpp/yaml.h>

namespace cydui::config {
    class ConfigManager;
}

class cydui::config::ConfigManager {
  std::string path;
  
  void parse_raw();

protected:
  virtual void serialize() = 0;
  
  virtual void deserialize(YAML::Node root) = 0;
  
  explicit ConfigManager(std::string path);

public:
  
  void refresh();
  
  void update_config_file();
  
};

#define CONFIG(NAME) \
struct NAME: cydui::config::ConfigManager { \
  explicit NAME(str path): cydui::config::ConfigManager(std::move(path)) { \
  refresh(); \
  }


#define CONFIG_T(STRUCT) \
struct config_t STRUCT config;
#endif //CYD_UI_CONFIG_HPP
