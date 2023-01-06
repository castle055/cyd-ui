//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_LIST_APPS_TASK_HPP
#define CYD_UI_LIST_APPS_TASK_HPP

#include <string>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <filesystem>
#include <map>
#include <vector>
#include "../../include/logging.hpp"
#include <algorithm>
#include "../../include/cydui.hpp"
#include "../startmenu/config/startappscm.hpp"

namespace fs = std::filesystem;

class ListAppsTask {
  float progress = 0.0f;
  bool  running  = false;
  bool  complete = false;
  bool  error    = false;
  
  std::string is_in_group(StartAppsCM config, std::string name) {
    for (const auto &group: config.config.groups) {
      if (group.apps.contains(name))
        return group.name;
    }
    return "";
  }
  
  std::string is_in_custom_section(StartAppsCM config, std::string name) {
    for (const auto &section: config.config.sections) {
      if (section.apps.contains(name))
        return section.name;
    }
    return "";
  }
  
  void add_to_group(
    StartAppsCM config,
    std::string group,
    Application app
  ) {
    std::string section = is_in_custom_section(config, group);
    if (group.size() > 19) {
      group = group.substr(0, 16);
      group.append("...");
    }
    bool custom_section = true;
    
    if (section.empty()) {
      custom_section = false;
      section.append(" ");
      section[0] = toupper(group[0]);
    }
    
    bool      existed = false;
    for (auto &item: apps[section]) {
      if (item.name == group) {
        item.grouped_apps.push_back(app);
        existed = true;
        break;
      }
    }
    
    if (!existed) {
      if (custom_section) {
        section_apps[section].push_back({
          .name = group,
          .group = true,
          .grouped_apps = {app},
        });
      } else {
        apps[section].push_back({
          .name = group,
          .group = true,
          .grouped_apps = {app},
        });
      }
    }
  }

public:
  logging::logger log {.name = "LIST_APPS"};
  
  void run(StartAppsCM config) {
    running = true;
    apps.clear();
    
    std::string path = "/usr/share/applications";
    
    std::ifstream   file;
    for (const auto &entry: fs::directory_iterator(path)) {
      if (entry.path().string().ends_with(".desktop")) {
        file.open(entry.path());
        if (file.is_open()) {
          std::string line;
          bool        display  = true;
          std::string name     = "UNKNOWN APP";
          std::string icon     = "UNKNOWN";
          std::string exec     = "UNKNOWN";
          bool        terminal = false;
          while (file) {
            std::getline(file, line);
            if (line.starts_with("[Desktop Entry]")) {
              //name = line.substr(5);
            } else if (line.starts_with("[Desktop Action")) {
              //name = line.substr(5);
              break;
            } else if (line.starts_with("Name=")) {
              name = line.substr(5);
            } else if (line.starts_with("Icon=")) {
              icon = line.substr(5);
            } else if (line.starts_with("Exec=")) {
              exec = line.substr(5);
            } else if (line.starts_with("Terminal=") && line.ends_with("true")) {
              terminal = true;
            } else if (line.starts_with("NoDisplay=") && line.ends_with("true")) {
              //log.debug("LINE= %s", line.c_str());
              display = false;
              break;
            }
          }
          
          if (display && !config.config.hide.contains(name)) {
            bool      replace = false;
            for (auto &item: exec) {
              if (replace) {
                item    = ' ';
                replace = false;
              }
              if (item == '%') {
                item    = ' ';
                replace = true;
              }
            }
            
            std::string group = is_in_group(config, name);
            if (group.empty()) {
              std::string section = is_in_custom_section(config, name);
              
              if (name.size() > 19) {
                name = name.substr(0, 16);
                name.append("...");
              }
              
              if (section.empty()) {
                section.append(" ");
                section[0] = toupper(name[0]);
                apps[section].push_back({
                  .name = name,
                  .icon = icon,
                  .exec = exec,
                  .terminal = terminal,
                });
              } else {
                section_apps[section].push_back({
                  .name = name,
                  .icon = icon,
                  .exec = exec,
                  .terminal = terminal,
                });
              }
              
            } else {
              add_to_group(config, group, {
                .name = name,
                .icon = icon,
                .exec = exec,
                .terminal = terminal,
              });
            }
          }
          
          file.close();
        }
      }
    }
    
  }
  
  std::unordered_map<std::string, std::vector<Application>> section_apps = { };
  std::map<std::string, std::vector<Application>>           apps         = { };
  
};

#endif //CYD_UI_LIST_APPS_TASK_HPP
