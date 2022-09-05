cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cyd_ui)

set(CMAKE_CXX_STANDARD 23)

include_directories(/usr/include/freetype2)

include(cppcheck.cmake)
include(clang-format.cmake)

add_library(cyd_ui SHARED src/cydui.cpp src/layout/layout.cpp include/layout.hpp src/window/window.cpp include/window.hpp src/graphics/graphics.hpp src/graphics/x11/x11_impl.cpp src/graphics/x11/state/state.cpp src/graphics/x11/state/state.hpp src/logging/logging.cpp src/logging/logging.hpp src/events/events.cpp src/events/events.hpp src/graphics/x11/events/events.cpp src/graphics/events.hpp include/components.hpp src/layout/components/components.cpp src/layout/components/primitives.cpp include/primitives.hpp src/threading/threading.cpp src/threading/threading.hpp include/window_types.hpp src/graphics/x11/render/render.cpp src/graphics/x11/render/render.hpp src/graphics/x11/x11_impl.hpp src/layout/color/colors.hpp src/layout/color/colors.cpp include/containers.hpp src/layout/components/containers.cpp src/layout/fonts/fonts.cpp src/layout/fonts/fonts.hpp test/wifi/banner.hpp test/components/button.hpp test/components/flexbox.hpp src/events/properties/properties.cpp src/events/properties/properties.hpp src/events/properties/int_properties.hpp src/layout/components/geometry/component_geometry.cpp src/layout/components/geometry/component_geometry.hpp)
target_link_libraries(cyd_ui PRIVATE X11 Xft)

add_executable(cyd_ui_test test/main.cpp test/main.hpp test/wifi/wifi.hpp)
target_link_libraries(cyd_ui_test PRIVATE cyd_ui)
#add_dependencies(cppcheck clang-format)
add_dependencies(cyd_ui_test cppcheck)
