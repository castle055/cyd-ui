//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_PRIMITIVES_HPP
#define CYD_UI_PRIMITIVES_HPP


#include "colors.hpp"
#include "fonts.hpp"
#include "window_types.hpp"
#include "cyd-log/dist/include/logging.hpp"
#include "components.hpp"

namespace primitives {
    using namespace cydui;
    using namespace cydui::components;
    using namespace cydui::layout::color;

#define CLAMP_ZERO(VAL) std::max(VAL, 0)
    
    STATE(Line) { };
    
    COMPONENT(Line) {
      PROPS({
        Color* color;
      })
      
      INIT(Line) {
      }
      
      RENDER(win) {
        graphics::drw_line(
          win->win_ref,
          props.color,
          state->dim.cx.val(),
          state->dim.cy.val(),
          state->dim.cx.val() + CLAMP_ZERO(state->dim.cw.val()),
          state->dim.cy.val() + CLAMP_ZERO(state->dim.ch.val())
        );
      }
    };
    
    
    STATE(Rectangle) { };
    
    COMPONENT(Rectangle) {
      PROPS({
        Color* color;
        bool filled = false;
      })
      
      INIT(Rectangle) {
        //ENABLE_LOG
      }
      
      RENDER(win) {
        log.debug("X= %d, CX= %d", dim->x.val(), dim->cx.val());
        graphics::drw_rect(
          win->win_ref,
          props.color,
          state->dim.cx.val(),
          state->dim.cy.val(),
          CLAMP_ZERO(state->dim.cw.val()),
          CLAMP_ZERO(state->dim.ch.val()),
          props.filled
        );
      }
    };
    
    
    STATE(Arc) { };
    
    COMPONENT(Arc) {
      PROPS({
        Color* color;
        bool filled = false;
        int a0 = 0;
        int a1 = 180;
      })
      
      INIT(Arc) {
      }
      
      RENDER(win) {
        graphics::drw_arc(
          win->win_ref,
          props.color,
          state->dim.cx.val(),
          state->dim.cy.val(),
          CLAMP_ZERO(state->dim.cw.val()),
          CLAMP_ZERO(state->dim.ch.val()),
          props.a0, props.a1, props.filled
        );
      }
    };
    
    
    STATE(Circle) { };
    
    COMPONENT(Circle) {
      PROPS({
        Color* color;
        bool filled;
      })
      
      INIT(Circle) {
      }
      
      RENDER(win) {
        graphics::drw_arc(
          win->win_ref, props.color,
          state->dim.cx.val(),
          state->dim.cy.val(),
          CLAMP_ZERO(state->dim.cw.val()),
          CLAMP_ZERO(state->dim.ch.val()),
          0, 360, props.filled
        );
      }
    };
    
    
    STATE(Text) { };
    
    COMPONENT(Text) {
      PROPS({
        Color* color;
        layout::fonts::Font* font;
        std::string text;
      })
      
      INIT(Text) {
        ENABLE_LOG
        std::pair<int, int> text_size = graphics::get_text_size(
          this->props.font,
          this->props.text
        );
        state->dim.w = text_size.first;
        state->dim.h = text_size.second;
        state->dim.given_w = true;
        state->dim.given_h = true;
      }
      
      RENDER(win) {
        log.debug("w = %d, h = %d", state->dim.cx.val(), state->dim.cy.val());
        graphics::drw_text(
          win->win_ref, props.font, props.color, props.text,
          state->dim.cx.val(),
          state->dim.cy.val() + state->dim.ch.val()
        );
      }
    };

#undef CLAMP_ZERO
}// namespace primitives

#endif//CYD_UI_PRIMITIVES_HPP
