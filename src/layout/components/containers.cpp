//
// Created by castle on 8/22/22.
//

#include "../../../include/containers.h"
#include "../../../include/primitives.h"
#include "../../logging/logging.h"

logging::logger cont_log = {.name = "CONT"};

containers::VBox::VBox(
    cydui::components::ComponentState* state,
    bool border,
    int spacing,
    std::vector<cydui::components::Component*> children
)
    : Component(state, children) {
  this->spacing = spacing;
  this->border  = border;
  
  int max_w = 0;
  int h     = 0;
  
  for (const auto &ch: children) {
    h += ch->state->geom.abs_h();
    if (ch->state->geom.abs_w() > max_w)
      max_w = ch->state->geom.abs_w();
  }
  //state->geom.w = max_w + 2;
  //state->geom.h = h + 2;
  //state->geom.h += spacing * (children.size() - 1);
}

void containers::VBox::on_redraw(cydui::events::layout::CLayoutEvent* ev) {
  if (border) {
    auto* c = new cydui::layout::color::Color("#FCAE1E");
    add(
        {
            new primitives::Rectangle(
                c, 0, 0, state->geom.w - 1, state->geom.h - 1, false
            )
        }
    );
  }
  
  int       max_w = 0;
  int       cur_h = 0;
  for (auto &ch: param_children) {
    ch->set_pos(this, 1, cur_h + 1);
    cur_h += ch->state->geom.abs_h() + spacing;
    if (ch->state->geom.abs_w() > max_w)
      max_w = ch->state->geom.abs_w();
  }
  set_min_size(max_w + 2, cur_h - spacing);
}

containers::HBox::HBox(
    cydui::components::ComponentState* _state,
    bool border,
    int spacing,
    std::vector<cydui::components::Component*> children
)
    : Component(_state, children) {
  this->spacing = spacing;
  this->border  = border;
  
  int max_h = 0;
  int w     = 0;
  
  for (const auto &ch: children) {
    w += ch->state->geom.abs_w();
    if (ch->state->geom.abs_h() > max_h)
      max_h = ch->state->geom.abs_h();
  }
  
  //state->geom.h = max_h + 2;
  //state->geom.w = w + 2;
  //state->geom.w += spacing * (children.size() - 1);
}

void containers::HBox::on_redraw(cydui::events::layout::CLayoutEvent* ev) {
  if (border) {
    auto* c = new cydui::layout::color::Color("#FCAE1E");
    add(
        {
            new primitives::Rectangle(
                c, 0, 0, state->geom.w - 1, state->geom.h - 1, false
            )
        }
    );
  }
  
  int       max_h = 0;
  int       cur_w = 0;
  for (auto &ch: param_children) {
    ch->set_pos(this, cur_w, 1);
    cur_w += ch->state->geom.abs_w() + spacing;
    if (ch->state->geom.abs_h() > max_h)
      max_h = ch->state->geom.abs_h();
  }
  set_min_size(cur_w - spacing, max_h + 2);
}
