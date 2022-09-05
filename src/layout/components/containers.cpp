//
// Created by castle on 8/22/22.
//

#include "../../../include/containers.h"

#include <utility>
#include "../../../include/primitives.h"
#include "../../logging/logging.h"

logging::logger cont_log = {.name = "CONT"};

containers::VBox::VBox(
    containers::VBoxState* _state,
    int spacing,
    std::function<void(cydui::components::Component*)> inner
)
    : Component(_state, inner) {
  _state->spacing = spacing;
}

void containers::VBox::on_redraw(cydui::events::layout::CLayoutEvent* ev) {
  auto* state = (VBoxState*)this->state;
  
  IntProperty::IntBinding cur_h = {.property = &state->offset};
  for (auto               &ch: children) {
    ch->set_pos(this, 1, cur_h.val());
    cur_h.property = (ch->state->geom.abs_h() + state->spacing + cur_h).unwrap();
  }
  cur_h.compute();
}

containers::HBox::HBox(
    containers::HBoxState* _state,
    int spacing,
    std::function<void(cydui::components::Component*)> inner
)
    : Component(_state, inner) {
  _state->spacing = spacing;
}

void containers::HBox::on_redraw(cydui::events::layout::CLayoutEvent* ev) {
  auto* state = (HBoxState*)this->state;
  
  IntProperty::IntBinding cur_w = {.property = &state->offset};
  for (auto               &ch: children) {
    ch->set_pos(this, cur_w.val(), 1);
    cur_w.property = (ch->state->geom.abs_w() + state->spacing + cur_w).unwrap();
  }
  cur_w.compute();
}
