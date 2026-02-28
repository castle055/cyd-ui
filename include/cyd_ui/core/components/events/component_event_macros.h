//
// Created by castle on 8/18/24.
//

#ifndef COMPONENT_EVENT_MACROS_H
#define COMPONENT_EVENT_MACROS_H

// clang-format off
#define CYDUI_INTERNAL_EV_DIM_LIST(X)                                                                                  \
  X( x                     , position[layout::X_AXIS]),                                                                \
  X( y                     , position[layout::Y_AXIS]),                                                                \
  X( width                 , viewport_size[layout::X_AXIS]),                                                           \
  X( height                , viewport_size[layout::Y_AXIS]),                                                           \
  X( box_width             , box_size[layout::X_AXIS]),                                                                \
  X( box_height            , box_size[layout::Y_AXIS]),                                                                \
  X( x_abs                 , screen_position[layout::X_AXIS]),                                                         \
  X( y_abs                 , screen_position[layout::Y_AXIS])
// clang-format on

#define CYDUI_INTERNAL_EV_DIM_PARAM(NAME, DEF) cydui::dimension_t& $##NAME
#define CYDUI_INTERNAL_EV_DIM_PARAMS           CYDUI_INTERNAL_EV_DIM_LIST(CYDUI_INTERNAL_EV_DIM_PARAM)
#define CYDUI_INTERNAL_EV_DIM_ARG(NAME, DEF)   geom.DEF
#define CYDUI_INTERNAL_EV_DIM_ARGS             CYDUI_INTERNAL_EV_DIM_LIST(CYDUI_INTERNAL_EV_DIM_ARG)

#define CYDUI_INTERNAL_EV_VAL_PARAM(NAME, DEF) const cydui::dimension_t::value_type& $##NAME
#define CYDUI_INTERNAL_EV_VAL_PARAMS           CYDUI_INTERNAL_EV_DIM_LIST(CYDUI_INTERNAL_EV_VAL_PARAM)
#define CYDUI_INTERNAL_EV_VAL_ARG(NAME, DEF)   cydui::dimensions::get_value(geom.DEF)
#define CYDUI_INTERNAL_EV_VAL_ARGS             CYDUI_INTERNAL_EV_DIM_LIST(CYDUI_INTERNAL_EV_VAL_ARG)

#define CYDUI_INTERNAL_EV_mount_ARGS           (CYDUI_INTERNAL_EV_DIM_PARAMS, const cydui::BlueprintList& $content)

#define CYDUI_INTERNAL_EV_dismount_ARGS        (CYDUI_INTERNAL_EV_DIM_PARAMS)

#define CYDUI_INTERNAL_EV_redraw_RETURN        cydui::BlueprintList
#define CYDUI_INTERNAL_EV_redraw_ARGS          (CYDUI_INTERNAL_EV_DIM_PARAMS, const cydui::BlueprintList& $content)

#define CYDUI_INTERNAL_EV_layout_ARGS          (CYDUI_INTERNAL_EV_DIM_PARAMS, const std::list<cydui::Component*>& $content)

#define CYDUI_INTERNAL_EV_button_ARGS                                                                                  \
  (cydui::Button                         button,                                                                       \
   const cydui::dimension_t::value_type& x,                                                                            \
   const cydui::dimension_t::value_type& y,                                                                            \
   CYDUI_INTERNAL_EV_VAL_PARAMS,                                                                                       \
   bool& propagate)

#define CYDUI_INTERNAL_EV_mouse_propagate_ARGS                                                                         \
  (const cydui::dimension_t::value_type& x,                                                                            \
   const cydui::dimension_t::value_type& y,                                                                            \
   CYDUI_INTERNAL_EV_VAL_PARAMS,                                                                                       \
   bool& propagate)

#define CYDUI_INTERNAL_EV_mouse_ARGS                                                                                   \
  (const cydui::dimension_t::value_type& x, const cydui::dimension_t::value_type& y, CYDUI_INTERNAL_EV_VAL_PARAMS)

#define CYDUI_INTERNAL_EV_scroll_ARGS                                                                                  \
  (const cydui::dimension_t::value_type& dx,                                                                           \
   const cydui::dimension_t::value_type& dy,                                                                           \
   CYDUI_INTERNAL_EV_VAL_PARAMS,                                                                                       \
   bool& propagate)

#define CYDUI_INTERNAL_EV_key_ARGS            (const cydui::KeyEvent& ev, CYDUI_INTERNAL_EV_VAL_PARAMS, bool& propagate)

#define CYDUI_INTERNAL_EV_text_input_ARGS     (const cydui::TextInputEvent& ev, CYDUI_INTERNAL_EV_VAL_PARAMS)

#define CYDUI_INTERNAL_EV_fragment_ARGS       (CYDUI_INTERNAL_EV_VAL_PARAMS)

#define CYDUI_INTERNAL_EV_focus_changed_ARGS  (CYDUI_INTERNAL_EV_VAL_PARAMS)

#define CYDUI_INTERNAL_EV_button_press_ARGS   CYDUI_INTERNAL_EV_button_ARGS
#define CYDUI_INTERNAL_EV_button_release_ARGS CYDUI_INTERNAL_EV_button_ARGS

#define CYDUI_INTERNAL_EV_mouse_enter_ARGS    CYDUI_INTERNAL_EV_mouse_ARGS
#define CYDUI_INTERNAL_EV_mouse_exit_ARGS     CYDUI_INTERNAL_EV_mouse_ARGS
#define CYDUI_INTERNAL_EV_mouse_motion_ARGS   CYDUI_INTERNAL_EV_mouse_propagate_ARGS

#define CYDUI_INTERNAL_EV_key_press_ARGS      CYDUI_INTERNAL_EV_key_ARGS
#define CYDUI_INTERNAL_EV_key_release_ARGS    CYDUI_INTERNAL_EV_key_ARGS


#endif // COMPONENT_EVENT_MACROS_H
