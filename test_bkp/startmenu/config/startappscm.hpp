//
// Created by castle on 1/5/23.
//

#ifndef CYD_UI_STARTAPPSCM_HPP
#define CYD_UI_STARTAPPSCM_HPP

#include "../../../include/cydui.hpp"

struct Application {
  std::string              name     = "";
  std::string              icon     = "";
  std::string              exec     = "";
  bool                     terminal = false;
  bool                     group    = false;
  std::vector<Application> grouped_apps;
};

CONFIG(StartAppsCM)
  CONFIG_T({
    struct group_t {
      std::string                     name;
      std::unordered_set<std::string> apps;
    };
    struct section_t {
      std::string                     name;
      std::unordered_set<std::string> apps;
    };
    
    std::vector<group_t>            groups;
    std::vector<section_t>          sections;
    std::unordered_set<std::string> hide;
  })
  
  void serialize() override {
    config.hide.contains("");
  }
  
  void deserialize(YAML::Node root) override {
    config.groups.clear();
    if (root["groups"] && root["groups"].IsSequence()) {
      auto             groups = root["groups"];
      for (std::size_t i      = 0; i < groups.size(); i++) {
        auto group = groups[i];
        
        config_t::group_t g;
        g.name = group["name"].as<std::string>("Group");
        
        if (group["apps"] && group["apps"].IsSequence()) {
          auto             apps = group["apps"];
          for (std::size_t j    = 0; j < apps.size(); j++) {
            g.apps.insert(apps[j].as<std::string>("unknown app (oh no!)"));
          }
        }
        config.groups.push_back(g);
      }
    }
    
    config.sections.clear();
    if (root["sections"] && root["sections"].IsSequence()) {
      auto             sections = root["sections"];
      for (std::size_t i        = 0; i < sections.size(); i++) {
        auto section = sections[i];
        
        config_t::section_t s;
        s.name = section["name"].as<std::string>("SECTION");
        
        if (section["apps"] && section["apps"].IsSequence()) {
          auto             apps = section["apps"];
          for (std::size_t j    = 0; j < apps.size(); j++) {
            s.apps.insert(apps[j].as<std::string>("unknown app (oh no!)"));
          }
        }
        config.sections.push_back(s);
      }
    }
    
    config.hide.clear();
    if (root["hide"] && root["hide"].IsSequence()) {
      auto      hide = root["hide"];
      for (auto &&h: hide) {
        config.hide.insert(h.as<std::string>("unknown app (oh no!)"));
      }
    }
  }
};

#endif //CYD_UI_STARTAPPSCM_HPP
