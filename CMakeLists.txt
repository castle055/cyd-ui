cmake_minimum_required(VERSION 3.21)
project(cyd_ui)

set(CMAKE_CXX_STANDARD 23)

include_directories(/usr/include/freetype2)

include(cppcheck.cmake)
include(clang-format.cmake)

add_library(cyd_ui SHARED src/cydui.cpp src/layout/layout.cpp include/layout.h src/window/window.cpp include/window.h src/graphics/graphics.h src/graphics/x11/x11_impl.cpp src/graphics/x11/state/state.cpp src/graphics/x11/state/state.h src/logging/logging.cpp src/logging/logging.h src/events/events.cpp src/events/events.h src/graphics/x11/events/events.cpp src/graphics/events.h include/components.h src/layout/components/components.cpp src/layout/components/primitives.cpp include/primitives.h src/threading/threading.cpp src/threading/threading.h include/window_types.h src/graphics/x11/render/render.cpp src/graphics/x11/render/render.h src/graphics/x11/x11_impl.h src/layout/color/colors.h src/layout/color/colors.cpp include/containers.h src/layout/components/containers.cpp src/layout/fonts/fonts.cpp src/layout/fonts/fonts.h test/wifi/banner.h test/components/button.h test/components/flexbox.h src/events/properties/properties.cpp src/events/properties/properties.h src/events/properties/int_properties.h)
target_link_libraries(cyd_ui PRIVATE X11 Xft)

add_executable(cyd_ui_test test/main.cpp test/main.h test/wifi/wifi.h)
target_link_libraries(cyd_ui_test PRIVATE cyd_ui)
#add_dependencies(cppcheck clang-format)
add_dependencies(cyd_ui_test cppcheck)
