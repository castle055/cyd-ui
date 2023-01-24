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
                state->dim.cx.val() + state->dim.cw.val(),
                state->dim.cy.val() + state->dim.ch.val()
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
    }

    RENDER(win) {
        graphics::drw_rect(
                win->win_ref,
                props.color,
                state->dim.cx.val(),
                state->dim.cy.val(),
                state->dim.cw.val(),
                state->dim.ch.val(),
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
                state->dim.cw.val(),
                state->dim.ch.val(),
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
                state->dim.cw.val(),
                state->dim.ch.val(),
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
        state->dim.w = 125;
        state->dim.h = this->props.font->size;
        state->dim.given_w = true;
        state->dim.given_h = true;
    }

    RENDER(win) {
        graphics::drw_text(
                win->win_ref, props.font, props.color, props.text,
                state->dim.cx.val(),
                state->dim.cy.val()
        );
    }
  };

}// namespace primitives

#define rectangle(CLR, W, H, FILLED) \
new primitives::Rectangle(CLR,0,0,W,H,FILLED)

#endif//CYD_UI_PRIMITIVES_HPP
