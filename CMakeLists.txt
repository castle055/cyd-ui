cmake_minimum_required(VERSION 3.26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cyd_ui)

set(CMAKE_CXX_STANDARD 23)

include(CheckCXXCompilerFlag)

set(CMAKE_REQUIRED_QUIET true)
function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if (flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if (flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
            message("   [ OK  ] Compiler flag: ${flag}")
        else ()
            message("   [ERROR] Compiler flag: ${flag} <--(Not supported)")
        endif ()
        unset(flag_supported CACHE)
    else ()
        message("   [WARN ] Compiler flag: ${flag} <--(Already set)")
    endif ()
endfunction()

function(compiler_flags)
    message("Setting Compiler Flags:")
    foreach (flag ${ARGN})
        enable_cxx_compiler_flag_if_supported("${flag}")
    endforeach ()
endfunction()


compiler_flags(
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"

        "-Wliteral-suffix"

        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-variable"
        "-Wno-unused-value"
        "-Wno-unused-but-set-variable"
        "-Wno-missing-field-initializers"
        #"-Wno-error=unused"

        "-Wno-unknown-pragmas"
)


include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(/usr/include/freetype2)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIROMM-1.16 REQUIRED cairomm-1.16)
include_directories(${CAIROMM-1.16_INCLUDE_DIRS})


FILE(GLOB_RECURSE SRC_LIST
        ${CMAKE_CURRENT_LIST_DIR}/include/*.h
        ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/*.h
        ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

add_library(cyd_ui)
target_sources(cyd_ui
        PUBLIC
        ${SRC_LIST}
)
target_link_libraries(cyd_ui
        PRIVATE
        X11 Xft Xrender fontconfig
        yaml-cpp jpeg ${CAIROMM-1.16_LIBRARIES}
)

# Build Tests
FILE(GLOB_RECURSE TEST_LIST
        ${CMAKE_CURRENT_LIST_DIR}/test/*.*
)
add_executable(cyd_ui_test ${TEST_LIST})
target_link_libraries(cyd_ui_test PRIVATE cyd_ui)

# Build Doxygen Documentation
add_custom_target(
        cyd_ui_docs
        COMMAND /usr/bin/doxygen ${CMAKE_CURRENT_LIST_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMENT "Generating DOXYGEN documentation..."
)

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
    add_dependencies(cyd_ui cyd_ui_docs)
endif ()
