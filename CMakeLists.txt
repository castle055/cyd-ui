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

enable_cxx_compiler_flag_if_supported("-fmodules-ts")

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(/usr/include/freetype2)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIROMM-1.16 REQUIRED cairomm-1.16)
include_directories(${CAIROMM-1.16_INCLUDE_DIRS})

include(cppcheck.cmake)
include(clang-format.cmake)

FILE(GLOB_RECURSE SRC_LIST
        ${CMAKE_CURRENT_LIST_DIR}/include/*.*
        ${CMAKE_CURRENT_LIST_DIR}/src/*.*
)
list(APPEND SRC_LIST ${EXTRA_LIBS_LIST})
add_library(cyd_ui ${SRC_LIST})
target_link_libraries(cyd_ui PUBLIC ${STATIC_EXTRA_LIBS})
target_link_libraries(cyd_ui PUBLIC X11 Xft Xrender fontconfig)
target_link_libraries(cyd_ui PUBLIC yaml-cpp)
target_link_libraries(cyd_ui PUBLIC jpeg)
target_link_libraries(cyd_ui PUBLIC ${CAIROMM-1.16_LIBRARIES})

FILE(GLOB_RECURSE TEST_LIST
        ${CMAKE_CURRENT_LIST_DIR}/test/*.*
)
add_executable(cyd_ui_test ${TEST_LIST})
target_link_libraries(cyd_ui_test PRIVATE cyd_ui)

#add_dependencies(cppcheck clang-format)
#add_dependencies(cyd_ui_test cppcheck)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    add_custom_command(TARGET cyd_ui POST_BUILD
            COMMAND /usr/bin/cp
            -v
            ${CMAKE_CURRENT_LIST_DIR}/cmake-build-release/libcyd_ui.a
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Generating distribution files..."
    )
    add_custom_command(TARGET cyd_ui POST_BUILD
            COMMAND /usr/bin/cp
            -rv
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Copying header files..."
    )
    add_custom_command(TARGET cyd_ui POST_BUILD
            COMMAND /usr/bin/cp
            -v
            ${CMAKE_CURRENT_LIST_DIR}/cyd.yml
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Copying CYD files..."
    )
endif ()
