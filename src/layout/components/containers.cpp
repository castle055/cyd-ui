//
// Created by castle on 8/22/22.
//

#include "../../../include/containers.h"
#include "../../../include/primitives.h"
#include "../../logging/logging.h"

logging::logger cont_log = { .name = "CONT" };

containers::VBox::VBox(cydui::components::ComponentState* state, bool border, int spacing, std::vector<cydui::components::Component*> children)
    : Component(state, children) {
  this->spacing = spacing;
  this->border = border;
  
  int max_w = 0;
  int h = 0;
  
  for (const auto &ch : children) {
    h += ch->state->h;
    if (ch->state->w > max_w)
      max_w = ch->state->w;
  }
  
  state->w = max_w + 2;
  state->h = h + 2;
  state->h += spacing * (children.size()-1);
}

void containers::VBox::on_redraw(cydui::events::layout::CLayoutEvent *ev) {
  if (border) {
    auto *c = new cydui::layout::color::Color("#FCAE1E");
    add({
            new primitives::Rectangle(c, state->x, state->y, state->w-1, state->h-1, false)
        });
  }
  
  int cur_h = 0;
  for (auto &ch : paramChildren) {
    ch->set_pos(state->x + 1, state->y + cur_h + 1);
    cur_h += ch->state->h + spacing;
  }
}

containers::HBox::HBox(cydui::components::ComponentState* state, bool border, int spacing, std::vector<cydui::components::Component*> children)
    : Component(state, children) {
  this->spacing = spacing;
  this->border = border;
  
  int max_h = 0;
  int w = 0;
  
  for (const auto &ch : children) {
    w += ch->state->w;
    if (ch->state->h > max_h)
      max_h = ch->state->h;
  }
  
  state->h = max_h + 2;
  state->w = w + 2;
  state->w += spacing * (children.size()-1);
}

void containers::HBox::on_redraw(cydui::events::layout::CLayoutEvent *ev) {
  if (border) {
    auto *c = new cydui::layout::color::Color("#FCAE1E");
    add({
            new primitives::Rectangle(c, state->x, state->y, state->w-1, state->h-1, false)
        });
  }
  
  int cur_w = 0;
  for (auto &ch : paramChildren) {
    ch->set_pos(state->x + cur_w + 1, state->y + 1);
    cur_w += ch->state->w + spacing;
  }
}
