cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cyd_ui)

set(CMAKE_CXX_STANDARD 23)

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if (flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if (flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif ()
        unset(flag_supported CACHE)
    endif ()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-Werror")

enable_cxx_compiler_flag_if_supported("-Wno-unused-function")
enable_cxx_compiler_flag_if_supported("-Wno-unused-parameter")
enable_cxx_compiler_flag_if_supported("-Wno-unused-variable")
enable_cxx_compiler_flag_if_supported("-Wno-unused-value")
enable_cxx_compiler_flag_if_supported("-Wno-unused-but-set-variable")
enable_cxx_compiler_flag_if_supported("-Wno-missing-field-initializers")
#enable_cxx_compiler_flag_if_supported("-Wno-error=unused")

enable_cxx_compiler_flag_if_supported("-Wno-unknown-pragmas")

include_directories(/usr/include/freetype2)

include(cppcheck.cmake)
include(clang-format.cmake)

add_library(cyd_ui SHARED
        src/cydui.cpp
        src/layout/layout.cpp
        include/layout.hpp
        src/window/window.cpp
        include/window.hpp
        include/graphics.hpp
        src/graphics/x11/x11_impl.cpp
        src/graphics/x11/state/state.cpp
        src/graphics/x11/state/state.hpp
        src/logging/logging.cpp
        include/logging.hpp
        src/events/events.cpp
        include/events.hpp
        src/graphics/x11/events/events.cpp
        src/graphics/events.hpp
        include/components.hpp
        src/layout/components/components.cpp
        include/primitives.hpp
        src/threading/threading.cpp
        include/threading.hpp
        include/window_types.hpp
        src/graphics/x11/render/render.cpp
        src/graphics/x11/render/render.hpp
        include/x11_impl.hpp
        include/colors.hpp
        src/layout/color/colors.cpp
        include/containers.hpp
        src/layout/components/containers.cpp
        src/layout/fonts/fonts.cpp
        include/fonts.hpp
        src/events/properties/properties.cpp
        include/properties.hpp
        include/int_properties.hpp
        src/layout/components/geometry/component_geometry.cpp
        include/component_geometry.hpp
        src/layout/components/state/children_state_collection.cpp
        include/children_state_collection.hpp
        include/change_events.h include/config.hpp src/config/config.cpp test/startmenu/tabs/home/shortcuts.hpp)
target_link_libraries(cyd_ui PRIVATE X11 Xft)
target_link_libraries(cyd_ui PUBLIC yaml-cpp)

add_executable(cyd_ui_test
        test/main.cpp
        test/wifi/wifi.hpp
        test/status_bar/workspaces.hpp
        test/components/terminal_button.hpp
        test/tasks/open_terminal_task.hpp
        test/tasks/run_command_task.h
        test/wifi/banner.hpp
        test/components/button.hpp
        test/components/flexbox.hpp
        test/components/clock_module.hpp
        test/status_bar/workspace_selector.hpp
        test/status_bar/window_title.hpp
        test/startmenu/startmenu.hpp test/startmenu/startmenuselector.hpp test/startmenu/powerselector.hpp test/startmenu/tabs/hometab.hpp test/startmenu/tabs/appstab.hpp test/startmenu/tabs/searchtab.hpp test/startmenu/tabs/appbutton.hpp test/startmenu/tabs/appsection.hpp test/startmenu/tabs/listview.hpp test/tasks/list_apps_task.hpp test/startmenu/config/startappscm.hpp test/startmenu/tabs/home/sysmon.hpp test/startmenu/tabs/home/radial_indicator.hpp test/startmenu/tabs/home/bar_indicator.hpp test/startmenu/tabs/home/quicksettings.hpp)
target_link_libraries(cyd_ui_test PRIVATE cyd_ui)
#add_dependencies(cppcheck clang-format)
#add_dependencies(cyd_ui_test cppcheck)

