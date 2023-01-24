//
// Created by castle on 1/14/23.
//

#ifndef CYD_UI_PROJECT_ITEM_HPP
#define CYD_UI_PROJECT_ITEM_HPP

#include "../../../../../include/cydui.hpp"

STATE(ProjectItem)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#AABBFF");
  cydui::layout::fonts::Font name_font {
    .name = "Fira Code Retina",
    .size = 10,
  };
  
  OpenTerminalTask open_terminal_task;
  
  int selected = false;
  
  INIT_STATE(ProjectItem) {
    border.color = c_fg;
  }
};

COMPONENT(ProjectItem)
  PROPS({
    std::string              name;
    std::string              path;
    std::vector<std::string> actions;
  })
  INIT(ProjectItem) }
  
  REDRAW {
    add({
      COMP(Text)({
        .props = {
          .color = state->c_fg,
          .font = &state->name_font,
          .text = props.name,
        }
      }),
      state->selected?
      COMP(FlexBox)({
        .props = {.vertical = false},
        .inner = {
          COMP(Button)({
            .props = {
              .text = "OPN",
              .on_action = [this]() {
                std::string c = "ranger ";
                c.append(props.path.substr(0, props.path.size() - 8));
                this->state->open_terminal_task.run({.cmd = std::move(c)});
              },
            },
            .init = [](auto* b) {
              b->set_width(35);
            },
          }),
          COMP(Button)({
            .props = {
              .text = "IDE",
            },
            .init = [](auto* b) {
              b->set_width(35);
            },
          }),
          COMP(Line)({
            .props = {
              .color = state->c_fg,
            },
            .init = [](auto* l) {
              l->set_width(0);
              l->set_height(18);
            },
          }),
          FOR_EACH(Button)(props.actions, [](auto a) {
            return c_init_t<Button> {
              .props = {
                .text = a,
              },
              .init = [](auto* b) {
                b->set_width(35);
                b->set_margin(0, 00, 0, 2);
              },
            };
          }),
        },
        .init = [this](auto* f) {
          f->set_pos(this, 0, 20);
          f->set_width(((35 + 4) * 2 + 1) + ((35 + 4) * props.actions.size()));
        },
      }) : NULLCOMP,
    });
    set_padding(10, 10, 9, 10);
    //set_border_enable(true);
  }
  
  void on_mouse_click(int x, int y, int button) override {
    state->selected = !state->selected;
    events::emit<RedrawEvent>({ });
    events::emit<RedrawEvent>({ });
  }
};

#endif //CYD_UI_PROJECT_ITEM_HPP
