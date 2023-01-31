//
// Created by castle on 1/13/23.
//

#ifndef CYD_UI_PROJECTS_HPP
#define CYD_UI_PROJECTS_HPP

#include "../../../../../include/cydui.hpp"
#include "../../../../components/button.hpp"
#include "../../../../components/flexbox.hpp"
#include "../../../../components/clock_module.hpp"
#include "../../listview.hpp"
#include "../../appbutton.hpp"
#include "../../../config/startappscm.hpp"
#include "../radial_indicator.hpp"
#include "../bar_indicator.hpp"
#include "./project_item.hpp"
#include <map>
#include <utility>
#include <filesystem>

struct project_t {
  std::string              name;
  std::string              path;
  std::vector<std::string> actions;
};

namespace fs = std::filesystem;
TASK_W_RESULT(GetProjectsTask, {
  std::vector<std::string> project_directories;
}, {
  std::vector<project_t> projects;
}, {
  std::ifstream file;
  result.projects.clear();
  for (const auto &path: args.project_directories) {
    if (!fs::exists(path)) continue;
    for (const auto &entry: fs::directory_iterator(path)) {
      if (entry.path().string().ends_with("cyd.yml")) {
        parse_file(entry.path().string());
      } else if (entry.is_directory()) {
        for (const auto &sub_entry: fs::directory_iterator(entry.path())) {
          if (sub_entry.path().string().ends_with("cyd.yml")) {
            parse_file(sub_entry.path().string());
          }
        }
      }
    }
  }
  //get();
})
  void parse_file(std::string path) {
    auto root = YAML::LoadFile(path);
    if (root["project"] && root["project"].IsMap()) {
      auto p    = root["project"];
      auto name = p["name"].as<std::string>("Project");
      
      std::vector<std::string> acts;
      if (p["actions"] && p["actions"].IsSequence()) {
        auto actions = p["actions"];
        if (root["cmd"] && root["cmd"].IsMap()) {
          auto             cmds = root["cmd"];
          for (std::size_t i    = 0; i < actions.size(); i++) {
            auto a = actions[i].as<std::string>("Project");
            acts.push_back(a);
            if (cmds[a]) {
            
            }
          }
        }
      }
      
      result.projects.push_back({
        .name = name,
        .path = path,
        .actions = acts,
      });
    }
  }
};


STATE(Projects)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#AABBFF");
  GetProjectsTask get_projects_task;
  
  int list_scroll = 0;
  
  INIT_STATE(Projects) {
    border.color = c_fg;
    get_projects_task.run({
      .project_directories = {
        "/home/castle/repos/inhouse",
        "/home/castle/repos/public",
        "/home/castle/repos/star",
        "/home/castle/corium/sources",
      }
    });
  }
};

COMPONENT(Projects)
  NO_PROPS INIT(Projects) }
  
  REDRAW {
    auto* prjs = state->get_projects_task.res();
    add({
      state->get_projects_task.is_complete()?
      COMP(ListView)({
        .props = {
          .scroll = &state->list_scroll,
          .spacing = 30,
          .height = this->state->geom.content_h().val(),
        },
        .inner = {
          FOR_EACH(ProjectItem)(prjs->projects,
            [this](auto project) {
              return c_init_t<ProjectItem> {
                .props = {
                  .name = project.name,
                  .path = project.path,
                  .actions = project.actions,
                },
                .init = [this](auto* item) {
                  item->set_width(this->state->geom.content_w().val());
                },
              };
            }),
        },
      }) : NULLCOMP,
    });
    
    set_padding(10, 10, 14, 10);
    set_border_enable(true);
  }
  
  void on_scroll(int d) override {
    state->list_scroll += d;
    if (state->list_scroll > 0) state->list_scroll = 0;
    state->dirty();
  }
};

#endif //CYD_UI_PROJECTS_HPP
