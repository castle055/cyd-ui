//
// Created by castle on 8/18/24.
//

#ifndef COMPONENT_EVENT_MACROS_H
#define COMPONENT_EVENT_MACROS_H

#define CYDUI_INTERNAL_EV_redraw_RETURN std::vector<cydui::components::component_holder_t>
#define CYDUI_INTERNAL_EV_redraw_ARGS                                                              \
  (cydui::dimension_t & $x,                                                                        \
   cydui::dimension_t & $y,                                                                        \
   cydui::dimension_t & $width,                                                                    \
   cydui::dimension_t & $height,                                                                   \
   cydui::dimension_t & $padding_top,                                                              \
   cydui::dimension_t & $padding_bottom,                                                           \
   cydui::dimension_t & $padding_left,                                                             \
   cydui::dimension_t & $padding_right,                                                            \
   cydui::components::component_builder_t & $content)

#define CYDUI_INTERNAL_EV_button_ARGS                                                              \
  (Button                                button,                                                   \
   const cydui::dimension_t::value_type& x,                                                        \
   const cydui::dimension_t::value_type& y,                                                        \
   const cydui::dimension_t::value_type& $x,                                                       \
   const cydui::dimension_t::value_type& $y,                                                       \
   const cydui::dimension_t::value_type& $width,                                                   \
   const cydui::dimension_t::value_type& $height,                                                  \
   const cydui::dimension_t::value_type& $padding_top,                                             \
   const cydui::dimension_t::value_type& $padding_bottom,                                          \
   const cydui::dimension_t::value_type& $padding_left,                                            \
   const cydui::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_mouse_ARGS                                                               \
  (const cydui::dimension_t::value_type& x,                                                        \
   const cydui::dimension_t::value_type& y,                                                        \
   const cydui::dimension_t::value_type& $x,                                                       \
   const cydui::dimension_t::value_type& $y,                                                       \
   const cydui::dimension_t::value_type& $width,                                                   \
   const cydui::dimension_t::value_type& $height,                                                  \
   const cydui::dimension_t::value_type& $padding_top,                                             \
   const cydui::dimension_t::value_type& $padding_bottom,                                          \
   const cydui::dimension_t::value_type& $padding_left,                                            \
   const cydui::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_scroll_ARGS                                                              \
  (const cydui::dimension_t::value_type& dx,                                                       \
   const cydui::dimension_t::value_type& dy,                                                       \
   const cydui::dimension_t::value_type& $x,                                                       \
   const cydui::dimension_t::value_type& $y,                                                       \
   const cydui::dimension_t::value_type& $width,                                                   \
   const cydui::dimension_t::value_type& $height,                                                  \
   const cydui::dimension_t::value_type& $padding_top,                                             \
   const cydui::dimension_t::value_type& $padding_bottom,                                          \
   const cydui::dimension_t::value_type& $padding_left,                                            \
   const cydui::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_key_ARGS                                                                 \
  (const KeyEvent&                       ev,                                                       \
   const cydui::dimension_t::value_type& $x,                                                       \
   const cydui::dimension_t::value_type& $y,                                                       \
   const cydui::dimension_t::value_type& $width,                                                   \
   const cydui::dimension_t::value_type& $height,                                                  \
   const cydui::dimension_t::value_type& $padding_top,                                             \
   const cydui::dimension_t::value_type& $padding_bottom,                                          \
   const cydui::dimension_t::value_type& $padding_left,                                            \
   const cydui::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_text_input_ARGS                                                          \
  (const TextInputEvent&                 ev,                                                       \
   const cydui::dimension_t::value_type& $x,                                                       \
   const cydui::dimension_t::value_type& $y,                                                       \
   const cydui::dimension_t::value_type& $width,                                                   \
   const cydui::dimension_t::value_type& $height,                                                  \
   const cydui::dimension_t::value_type& $padding_top,                                             \
   const cydui::dimension_t::value_type& $padding_bottom,                                          \
   const cydui::dimension_t::value_type& $padding_left,                                            \
   const cydui::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_fragment_ARGS                                                            \
  (vg::fragment_t & fragment,                                                                      \
   const cydui::dimension_t::value_type& $x,                                                       \
   const cydui::dimension_t::value_type& $y,                                                       \
   const cydui::dimension_t::value_type& $width,                                                   \
   const cydui::dimension_t::value_type& $height,                                                  \
   const cydui::dimension_t::value_type& $padding_top,                                             \
   const cydui::dimension_t::value_type& $padding_bottom,                                          \
   const cydui::dimension_t::value_type& $padding_left,                                            \
   const cydui::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_focus_changed_ARGS                                                       \
  (const cydui::dimension_t::value_type& $x,                                                       \
   const cydui::dimension_t::value_type& $y,                                                       \
   const cydui::dimension_t::value_type& $width,                                                   \
   const cydui::dimension_t::value_type& $height,                                                  \
   const cydui::dimension_t::value_type& $padding_top,                                             \
   const cydui::dimension_t::value_type& $padding_bottom,                                          \
   const cydui::dimension_t::value_type& $padding_left,                                            \
   const cydui::dimension_t::value_type& $padding_right)

#define CYDUI_INTERNAL_EV_button_press_ARGS   CYDUI_INTERNAL_EV_button_ARGS
#define CYDUI_INTERNAL_EV_button_release_ARGS CYDUI_INTERNAL_EV_button_ARGS

#define CYDUI_INTERNAL_EV_mouse_enter_ARGS    CYDUI_INTERNAL_EV_mouse_ARGS
#define CYDUI_INTERNAL_EV_mouse_exit_ARGS     CYDUI_INTERNAL_EV_mouse_ARGS
#define CYDUI_INTERNAL_EV_mouse_motion_ARGS   CYDUI_INTERNAL_EV_mouse_ARGS

#define CYDUI_INTERNAL_EV_key_press_ARGS      CYDUI_INTERNAL_EV_key_ARGS
#define CYDUI_INTERNAL_EV_key_release_ARGS    CYDUI_INTERNAL_EV_key_ARGS


#endif // COMPONENT_EVENT_MACROS_H
