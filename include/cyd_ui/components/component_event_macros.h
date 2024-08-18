//
// Created by castle on 8/18/24.
//

#ifndef COMPONENT_EVENT_MACROS_H
#define COMPONENT_EVENT_MACROS_H

#define CYDUI_INTERNAL_EV_redraw_RETURN std::vector<cyd::ui::components::component_holder_t>
#define CYDUI_INTERNAL_EV_redraw_ARGS   ()

#define CYDUI_INTERNAL_EV_button_ARGS (Button button, int x, int y)
#define CYDUI_INTERNAL_EV_mouse_ARGS  (int x, int y)
#define CYDUI_INTERNAL_EV_scroll_ARGS  (int dx, int dy)

#define CYDUI_INTERNAL_EV_button_press_ARGS       CYDUI_INTERNAL_EV_button_ARGS
#define CYDUI_INTERNAL_EV_button_release_ARGS     CYDUI_INTERNAL_EV_button_ARGS

#define CYDUI_INTERNAL_EV_mouse_enter_ARGS        CYDUI_INTERNAL_EV_mouse_ARGS
#define CYDUI_INTERNAL_EV_mouse_exit_ARGS         CYDUI_INTERNAL_EV_mouse_ARGS
#define CYDUI_INTERNAL_EV_mouse_motion_ARGS       CYDUI_INTERNAL_EV_mouse_ARGS

#define CYDUI_INTERNAL_EV_key_press_ARGS          (const KeyEvent::DataType& ev)
#define CYDUI_INTERNAL_EV_key_release_ARGS        (const KeyEvent::DataType& ev)





#endif //COMPONENT_EVENT_MACROS_H
