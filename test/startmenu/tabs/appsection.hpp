//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_APPSECTION_HPP
#define CYD_UI_APPSECTION_HPP

#include "../../../include/cydui.hpp"
#include "../../components/button.hpp"
#include "../../components/flexbox.hpp"
#include "../../components/clock_module.hpp"
#include "appbutton.hpp"
#include "../../tasks/list_apps_task.hpp"
#include "../../tasks/run_command_task.h"
#include "../../tasks/open_terminal_task.hpp"

STATE(AppSection)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#FCAE1E");
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 14
  };
  RunCommandTask             run_command_task;
  OpenTerminalTask           open_terminal_task;
  
  INIT_STATE(AppSection) {
  }
};

COMPONENT(AppSection)
  PROPS({
    std::string              letter = "-";
    std::vector<Application> apps;
  })
  
  INIT(AppSection)
    int s    = this->props.apps.size();
    int rows = s / 3;
    if (s % 3 > 0)
      rows++;
    this->set_height(rows * 44 + 45);
  }
  
  REDRAW {
    
    int x       = 10;
    int w       = 200;
    int y       = 45;
    int spacing = 10;
    int total_w = 3 * w + 4 * spacing;
    
    Line::Props line_props = {
      .color = state->c_fg,
    };
    
    using namespace primitives;
    add({
      COMP(Text)({
        .props = {
          .color = state->c_fg,
          .font = &state->font,
          .text = props.letter,
        },
        .init = [this](Text* t) { t->set_pos(this, 17, 10); },
      }),
      COMP(Line)({
        .props = line_props,
        .init = [this](Line* l) {
          int w = 1;
          l->set_pos(this, 10, 33);
          l->set_size(w, 0);
        },
      }),
      COMP(Line)({
        .props = line_props,
        .init = [this](Line* l) {
          l->set_pos(this, 10 + 5, 33);
          int w = 609;
          if (props.apps.size() < 3) {
            w = props.apps.size() * 210 - 11 - 5;
          }
          l->set_size(w, 0);
        },
      }),
      COMP(Line)({
        .props = line_props,
        .init = [this](Line* l) {
          int w  = 1;
          int w1 = 30 + 4 + 1;
          int s  = 4;
          l->set_pos(this, 629 - w1 - s - w, 33);
          l->set_size(w, 0);
        },
      }),
      COMP(Line)({
        .props = line_props,
        .init = [this](Line* l) {
          int w  = 30;
          int w1 = 1;
          int s  = 4;
          l->set_pos(this, 629 - w1 - s - w, 33);
          l->set_size(w, 0);
        },
      }),
      COMP(Line)({
        .props = line_props,
        .init = [this](Line* l) {
          int w = 1;
          l->set_pos(this, 629 - w, 33);
          l->set_size(w, 0);
        },
      }),
      FOR_EACH(AppButton)(props.apps, [this, &x, &y, &w, &total_w, &spacing](Application app) {
        c_init_t<AppButton> init = {
          .props = {
            .app = app,
            .x = x,
            .y = y,
            .on_action = [this, app](int button) {
              if (!app.group) {
                if (app.terminal) {
                  state->open_terminal_task.run({
                    .w = 110,
                    .h = 30,
                    .cmd = app.exec.c_str(),
                  });
                } else {
                  state->run_command_task.run({app.exec.c_str()});
                }
              }
            }
          },
          .init = [this](AppButton* app_button) {
            app_button->set_pos(this, app_button->props.x, app_button->props.y);
            app_button->set_size(200, 34);
          }
        };
        x += w + spacing;
        if (x >= total_w) {
          x -= total_w - spacing;
          y += 34 + 10;
        }
        return init;
      })
    });
  }
};

#endif //CYD_UI_APPSECTION_HPP
