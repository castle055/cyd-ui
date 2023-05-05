//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_PRIMITIVES_HPP
#define CYD_UI_PRIMITIVES_HPP


#include "cydui.hpp"

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
      
      RENDER(target) {
        graphics::drw_line(target,
          props.color,
          state->dim.cx.val(),
          state->dim.cy.val(),
          state->dim.cx.val() + state->dim.cw.val(),
          state->dim.cy.val() + state->dim.ch.val());
      }// namespace primitives
    };
    
    
    STATE(Rectangle) { };
    
    COMPONENT(Rectangle) {PROPS({
        Color* color;
        bool filled = false;
      })
      
      INIT(Rectangle) {
        //ENABLE_LOG
      }
      
      RENDER(target) {
        //log.debug("X= %d, CX= %d", dim->x.val(), dim->cx.val());
        graphics::drw_rect(target,
          props.color,
          state->dim.cx.val(),
          state->dim.cy.val(),
          CLAMP_ZERO(state->dim.cw.val()),
          CLAMP_ZERO(state->dim.ch.val()),
          props.filled);
      }
    };
    
    
    STATE(Arc) { };
    
    COMPONENT(Arc) {PROPS({
        Color* color;
        bool filled = false;
        int a0 = 0;
        int a1 = 180;
      })
      
      INIT(Arc) {
      }
      
      RENDER(target) {
        graphics::drw_arc(target,
          props.color,
          state->dim.cx.val(),
          state->dim.cy.val(),
          CLAMP_ZERO(state->dim.cw.val()),
          CLAMP_ZERO(state->dim.ch.val()),
          props.a0,
          props.a1,
          props.filled);
      }
    };
    
    
    STATE(Circle) { };
    
    COMPONENT(Circle) {PROPS({
        Color* color;
        bool filled;
      })
      
      INIT(Circle) {
      }
      
      RENDER(target) {
        graphics::drw_arc(target,
          props.color,
          state->dim.cx.val(),
          state->dim.cy.val(),
          CLAMP_ZERO(state->dim.cw.val()),
          CLAMP_ZERO(state->dim.ch.val()),
          0,
          360,
          props.filled);
      }
    };
    
    
    STATE(Text) { };
    
    COMPONENT(Text) {PROPS({
        Color* color;
        layout::fonts::Font* font;
        str text;
      })
      
      INIT(Text) {//ENABLE_LOG
        std::pair<int, int> text_size = graphics::get_text_size(
          this->props.font, this->props.text);
        state->dim.w = text_size.first;
        state->dim.h = text_size.second;
        state->dim.given_w = true;
        state->dim.given_h = true;
      }
      
      RENDER(target) {
        log.debug("w = %d, h = %d", state->dim.cx.val(), state->dim.cy.val());
        graphics::drw_text(target,
          props.font,
          props.color,
          props.text,
          state->dim.cx.val(),
          state->dim.cy.val() + state->dim.ch.val());
      }
    };
    
    STATE(Image) {
      int img_w = 0, img_h = 0;
      
      layout::images::image_t* image = nullptr;
      
      INIT_STATE(Image) {
        events::emit<RedrawEvent>({.component = this->component_instance});
      }
    };
    
    COMPONENT(Image) {
      PROPS({
        bool preserve_ratio = true;
        const char* img;
      })
      
      INIT(Image) {
        //ENABLE_LOG
        if (!state->image || str(this->props.img) != state->image->path) {
          state->image = new cydui::layout::images::image_t {this->props.img};
        }
        std::pair<int, int> img_size = graphics::get_image_size(state->image);
        state->img_w = img_size.first;
        state->img_h = img_size.second;
        if (this->props.preserve_ratio) {
          const int fix_p = 65535;
          int ratio = fix_p * img_size.second / img_size.first;
          state->dim.w = (fix_p * state->dim.h) / ratio;
          state->dim.h = (state->dim.w * ratio) / fix_p;
        } else {
          state->dim.w = img_size.first;
          state->dim.h = img_size.second;
          state->dim.given_w = true;
          state->dim.given_h = true;
        }
      }
      
      REDRAW {
        //log.info("img: %d", dim->given_h);
        if (props.preserve_ratio) {
          if (!state->dim.given_w && !state->dim.given_h) {
            state->dim.w = state->img_w;
            state->dim.h = state->img_h;
          }
          state->dim.given_w = true;
          state->dim.given_h = true;
        }
      };
      
      RENDER(target) {
        graphics::drw_image(target,
          state->image,
          state->dim.cx.val(),
          state->dim.cy.val(),
          state->dim.cw.val(),
          state->dim.ch.val());
      }
    };
    
    
    STATE(ViewPort) {
      INIT_STATE(ViewPort) {
      }
    };
    
    COMPONENT(ViewPort) {
      PROPS({
        int x;
        int y;
      })
      INIT(ViewPort) {
        ENABLE_LOG
        state->sub_render_target or __((), {
          state->sub_render_target = new graphics::render_target_t(*state->win.unwrap(), 10, 10);
        });
      }
      
      REDRAW {
        state->sub_render_event_offset = {props.x, props.y};
      };
      RENDER(target) {
        if (state->sub_render_target) {
          graphics::drw_target(target,
            *state->sub_render_target.unwrap(),
            props.x + state->dim.cx.val(),
            props.y + state->dim.cy.val(),
            state->dim.cx.val(),
            state->dim.cy.val(),
            state->dim.cw.val(),
            state->dim.ch.val());
        }
      }
    };
}// namespace primitives

#undef CLAMP_ZERO
#endif//CYD_UI_PRIMITIVES_HPP
