//
// Created by castle on 8/18/24.
//

#ifndef COMPONENT_EVENT_MACROS_H
#define COMPONENT_EVENT_MACROS_H

#define CYDUI_INTERNAL_EV_redraw_RETURN std::vector<cyd::ui::components::component_holder_t>
#define CYDUI_INTERNAL_EV_redraw_ARGS                                                              \
  (cyd::ui::components::dimension_t & $x,                                                          \
   cyd::ui::components::dimension_t & $y,                                                          \
   cyd::ui::components::dimension_t & $width,                                                      \
   cyd::ui::components::dimension_t & $height,                                                     \
   cyd::ui::components::dimension_t & $padding_top,                                                \
   cyd::ui::components::dimension_t & $padding_bottom,                                             \
   cyd::ui::components::dimension_t & $padding_left,                                               \
   cyd::ui::components::dimension_t & $padding_right,                                              \
   cyd::ui::components::component_builder_t & $content)

#define CYDUI_INTERNAL_EV_button_ARGS                                                              \
  (Button                                              button,                                     \
   const cyd::ui::components::dimension_t::value_type& x,                                          \
   const cyd::ui::components::dimension_t::value_type& y,                                          \
   const cyd::ui::components::dimension_t::value_type& $x,                                         \
   const cyd::ui::components::dimension_t::value_type& $y,                                         \
   const cyd::ui::components::dimension_t::value_type& $width,                                     \
   const cyd::ui::components::dimension_t::value_type& $height,                                    \
   const cyd::ui::components::dimension_t::value_type& $padding_top,                               \
   const cyd::ui::components::dimension_t::value_type& $padding_bottom,                            \
   const cyd::ui::components::dimension_t::value_type& $padding_left,                              \
   const cyd::ui::components::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_mouse_ARGS                                                               \
  (const cyd::ui::components::dimension_t::value_type& x,                                          \
   const cyd::ui::components::dimension_t::value_type& y,                                          \
   const cyd::ui::components::dimension_t::value_type& $x,                                         \
   const cyd::ui::components::dimension_t::value_type& $y,                                         \
   const cyd::ui::components::dimension_t::value_type& $width,                                     \
   const cyd::ui::components::dimension_t::value_type& $height,                                    \
   const cyd::ui::components::dimension_t::value_type& $padding_top,                               \
   const cyd::ui::components::dimension_t::value_type& $padding_bottom,                            \
   const cyd::ui::components::dimension_t::value_type& $padding_left,                              \
   const cyd::ui::components::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_scroll_ARGS                                                              \
  (const cyd::ui::components::dimension_t::value_type& dx,                                         \
   const cyd::ui::components::dimension_t::value_type& dy,                                         \
   const cyd::ui::components::dimension_t::value_type& $x,                                         \
   const cyd::ui::components::dimension_t::value_type& $y,                                         \
   const cyd::ui::components::dimension_t::value_type& $width,                                     \
   const cyd::ui::components::dimension_t::value_type& $height,                                    \
   const cyd::ui::components::dimension_t::value_type& $padding_top,                               \
   const cyd::ui::components::dimension_t::value_type& $padding_bottom,                            \
   const cyd::ui::components::dimension_t::value_type& $padding_left,                              \
   const cyd::ui::components::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_key_ARGS                                                                 \
  (const KeyEvent&                                     ev,                                         \
   const cyd::ui::components::dimension_t::value_type& $x,                                         \
   const cyd::ui::components::dimension_t::value_type& $y,                                         \
   const cyd::ui::components::dimension_t::value_type& $width,                                     \
   const cyd::ui::components::dimension_t::value_type& $height,                                    \
   const cyd::ui::components::dimension_t::value_type& $padding_top,                               \
   const cyd::ui::components::dimension_t::value_type& $padding_bottom,                            \
   const cyd::ui::components::dimension_t::value_type& $padding_left,                              \
   const cyd::ui::components::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_fragment_ARGS                                                            \
  (vg::vg_fragment_t & fragment,                                                                   \
   const cyd::ui::components::dimension_t::value_type& $x,                                         \
   const cyd::ui::components::dimension_t::value_type& $y,                                         \
   const cyd::ui::components::dimension_t::value_type& $width,                                     \
   const cyd::ui::components::dimension_t::value_type& $height,                                    \
   const cyd::ui::components::dimension_t::value_type& $padding_top,                               \
   const cyd::ui::components::dimension_t::value_type& $padding_bottom,                            \
   const cyd::ui::components::dimension_t::value_type& $padding_left,                              \
   const cyd::ui::components::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_button_press_ARGS   CYDUI_INTERNAL_EV_button_ARGS
#define CYDUI_INTERNAL_EV_button_release_ARGS CYDUI_INTERNAL_EV_button_ARGS

#define CYDUI_INTERNAL_EV_mouse_enter_ARGS    CYDUI_INTERNAL_EV_mouse_ARGS
#define CYDUI_INTERNAL_EV_mouse_exit_ARGS     CYDUI_INTERNAL_EV_mouse_ARGS
#define CYDUI_INTERNAL_EV_mouse_motion_ARGS   CYDUI_INTERNAL_EV_mouse_ARGS

#define CYDUI_INTERNAL_EV_key_press_ARGS      CYDUI_INTERNAL_EV_key_ARGS
#define CYDUI_INTERNAL_EV_key_release_ARGS    CYDUI_INTERNAL_EV_key_ARGS


#endif //COMPONENT_EVENT_MACROS_H
