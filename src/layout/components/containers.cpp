//
// Created by castle on 8/22/22.
//

#include "../../../include/containers.h"

#include <utility>
#include "../../../include/primitives.h"
#include "../../logging/logging.h"

logging::logger cont_log = {.name = "CONT"};

containers::VBox::VBox(
    cydui::components::ComponentState* state,
    int spacing,
    std::function<void(cydui::components::Component*)> inner
)
    : Component(state, inner) {
  this->spacing = spacing;
}

void containers::VBox::on_redraw(cydui::events::layout::CLayoutEvent* ev) {
  int       max_w = 0;
  int       cur_h = 0;
  for (auto &ch: children) {
    ch->set_pos(this, 1, cur_h + 1);
    cur_h += ch->state->geom.abs_h() + spacing;
    if (ch->state->geom.abs_w() > max_w)
      max_w = ch->state->geom.abs_w();
  }
  set_min_size(max_w + 2, cur_h - spacing);
}

containers::HBox::HBox(
    cydui::components::ComponentState* _state,
    int spacing,
    std::function<void(cydui::components::Component*)> inner
)
    : Component(_state, inner) {
  this->spacing = spacing;
}

void containers::HBox::on_redraw(cydui::events::layout::CLayoutEvent* ev) {
  int       max_h = 0;
  int       cur_w = 0;
  for (auto &ch: children) {
    ch->set_pos(this, cur_w, 1);
    cur_w += ch->state->geom.abs_w() + spacing;
    if (ch->state->geom.abs_h() > max_h)
      max_h = ch->state->geom.abs_h();
  }
  set_min_size(cur_w - spacing, max_h + 2);
}
