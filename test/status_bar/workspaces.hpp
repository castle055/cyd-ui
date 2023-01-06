//
// Created by castle on 9/27/22.
//

#ifndef CYD_UI_WORKSPACES_HPP
#define CYD_UI_WORKSPACES_HPP

#include "../../include/cydui.hpp"
#include "../components/button.hpp"
#include "../components/flexbox.hpp"
#include "../components/clock_module.hpp"
#include "workspace_selector.hpp"

STATE(Workspaces)
  IntProperty occupied_workspaces;
  IntProperty selected_workspaces = 1;
  
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  INIT_STATE(Workspaces) {
    occupied_workspaces.bind(this);
    selected_workspaces.bind(this);
  }
};

COMPONENT(Workspaces)
  NO_PROPS
  
  INIT(Workspaces)
    DISABLE_LOG
  }
  
  REDRAW {
    WITH_STATE(Workspaces)
    
    ADD_TO(this, {
      N(FlexBox, ({ .vertical = false }), ({
        N(ClockModule),
          N(WorkspaceSelector, ({
            .text = "1",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 0)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 0)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 0);}
          })),
          N(WorkspaceSelector, ({
            .text = "2",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 1)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 1)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 1);}
          })),
          N(WorkspaceSelector, ({
            .text = "3",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 2)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 2)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 2);}
          })),
          N(WorkspaceSelector, ({
            .text = "4",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 3)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 3)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 3);}
          })),
          N(WorkspaceSelector, ({
            .text = "5",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 4)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 4)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 4);}
          })),
          N(WorkspaceSelector, ({
            .text = "6",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 5)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 5)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 5);}
          })),
          N(WorkspaceSelector, ({
            .text = "7",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 6)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 6)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 6);}
          })),
          N(WorkspaceSelector, ({
            .text = "8",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 7)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 7)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 7);}
          })),
          N(WorkspaceSelector, ({
            .text = "9",
            .font = &state->font,
            .occupied = (state->occupied_workspaces.val() & (1 << 8)) > 0,
            .selected = (state->selected_workspaces.val() & (1 << 8)) > 0,
            .on_click = action {state->selected_workspaces = state->selected_workspaces.val() ^ (1 << 8);}
          })),
      }), {
        thisFlexBox->set_width(270);
        thisFlexBox->set_pos(this, 10, 1);
      })
    })
  }
};

#endif//CYD_UI_WORKSPACES_HPP
