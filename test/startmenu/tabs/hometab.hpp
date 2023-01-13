//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_HOMETAB_HPP
#define CYD_UI_HOMETAB_HPP

#include "../../../include/cydui.hpp"
#include "../../components/button.hpp"
#include "../../components/flexbox.hpp"
#include "../../components/clock_module.hpp"
#include "home/shortcuts.hpp"
#include "home/sysmon.hpp"
#include "home/quicksettings.hpp"
#include "home/projects.hpp"
#include "home/music_player.hpp"
#include "home/opened_apps.hpp"
#include "home/services.hpp"

using namespace std::chrono_literals;

STATE(HomeTab)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#2d2310");
  
  INIT_STATE(HomeTab) { }
};

COMPONENT(HomeTab)
  PROPS({
    int width = 640;
  })
  
  INIT(HomeTab)
    //DISABLE_LOG
  }
  
  REDRAW {
    WITH_STATE(HomeTab)
    
    using namespace primitives;
    add({
      COMP(ShortCuts)({
        .init = [this, state](ShortCuts* sc) {
          sc->set_pos(this, props.width - 78, 0);
          sc->set_margin(10, 10, 10, 10);
        },
      }),
      COMP(SysMon)({
        .init = [this, state](SysMon* s) {
          s->set_pos(this, 0, 270);
          s->set_margin(10, 10, 10, 10);
        },
      }),
      COMP(QuickSettings)({
        .init = [this, state](QuickSettings* q) {
          q->set_pos(this, 0, 680);
          q->set_margin(10, 10, 10, 10);
        },
      }),
      COMP(Services)({
        .init = [this, state](Services* q) {
          q->set_pos(this, 0, 490);
          q->set_size(305, 200);
          q->set_margin(10, 10, 10, 10);
        },
      }),
      COMP(MusicPlayer)({
        .init = [this, state](MusicPlayer* q) {
          q->set_pos(this, 300, 490);
          q->set_size(330, 300);
          q->set_margin(10, 10, 10, 10);
        },
      }),
      COMP(Projects)({
        .init = [this, state](Projects* q) {
          q->set_pos(this, 160, 270);
          q->set_size(410, 230);
          q->set_margin(10, 10, 10, 10);
        },
      }),
      COMP(OpenedApps)({
        .init = [this, state](OpenedApps* q) {
          q->set_pos(this, 0, 0);
          q->set_size(570, 280);
          q->set_margin(10, 10, 10, 10);
        },
      }),
      //// ROW 1
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 10, 10);
      //  thisRectangle->set_size(200, 200);
      //}),
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 220, 10);
      //  thisRectangle->set_size(200, 200);
      //}),
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 430, 10);
      //  thisRectangle->set_size(200, 200);
      //}),
      //// ROW 2
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 10, 220);
      //  thisRectangle->set_size(200, 200);
      //}),
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 220, 220);
      //  thisRectangle->set_size(200, 200);
      //}),
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 430, 220);
      //  thisRectangle->set_size(200, 200);
      //}),
      //// ROW 3
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 10, 430);
      //  thisRectangle->set_size(200, 200);
      //}),
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 220, 430);
      //  thisRectangle->set_size(200, 200);
      //}),
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 430, 430);
      //  thisRectangle->set_size(200, 200);
      //}),
      //// ROW 4
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 10, 640);
      //  thisRectangle->set_size(200, 150);
      //}),
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 220, 640);
      //  thisRectangle->set_size(200, 150);
      //}),
      //N(Rectangle, ({
      //  .color = state->color,
      //  .filled = true
      //}), ({ }), {
      //  thisRectangle->set_pos(this, 430, 640);
      //  thisRectangle->set_size(200, 150);
      //}),
      //
    });
  }
};

#endif //CYD_UI_HOMETAB_HPP
