//
// Created by castle on 8/22/22.
//

#include "../../../include/containers.hpp"

#include <utility>
#include "../../../include/primitives.hpp"
#include "../../logging/logging.hpp"

logging::logger cont_log = {.name = "CONT"};

containers::VBox::VBox(
    containers::VBoxState* _state,
    int spacing,
    std::function<void(cydui::components::Component*)> IN
)
    : Component(_state, IN) {
  _state->spacing = spacing;
}

void containers::VBox::on_redraw(cydui::events::layout::CLayoutEvent* data) {
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
    std::function<void(cydui::components::Component*)> IN
)
    : Component(_state, IN) {
  _state->spacing = spacing;
}

void containers::HBox::on_redraw(cydui::events::layout::CLayoutEvent* data) {
  auto* state = (HBoxState*)this->state;
  
  IntProperty::IntBinding cur_w = {.property = &state->offset};
  for (auto               &ch: children) {
    ch->set_pos(this, cur_w.val(), 1);
    cur_w.property = (ch->state->geom.abs_w() + state->spacing + cur_w).unwrap();
  }
  cur_w.compute();
}
