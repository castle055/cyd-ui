cmake_minimum_required(VERSION 3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cyd_ui)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_SCAN_FOR_MODULES YES)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")

SET(TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)
SET(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
SET(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

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

        "-Wno-macro-redefine"
        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-variable"
        "-Wno-unused-value"
        "-Wno-unused-but-set-variable"
        "-Wno-missing-field-initializers"
        #"-Wno-error=unused"

        "-Wno-unknown-pragmas"
)


include_directories(${INCLUDE_DIR})
include_directories(/usr/include/freetype2)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIROMM-1.16 REQUIRED cairomm-1.16)
include_directories(${CAIROMM-1.16_INCLUDE_DIRS})


FILE(GLOB_RECURSE SRC_LIST
        ${INCLUDE_DIR}/*.h
        ${INCLUDE_DIR}/*.hpp
        ${INCLUDE_DIR}/*.cpp
        ${SOURCE_DIR}/*.h
        ${SOURCE_DIR}/*.hpp
        ${SOURCE_DIR}/*.cpp
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
        ${TEST_DIR}/*.h
        ${TEST_DIR}/*.hpp
        ${TEST_DIR}/*.cpp
        #        ${TEST_DIR}/**/*.*
)
#FILE(GLOB_RECURSE TEST_MOD_LIST
#        ${TEST_DIR}/*.cppm
#        ${TEST_DIR}/**/*.*
#)
add_executable(cyd_ui_test ${TEST_LIST})
#target_sources(cyd_ui_test
#        PRIVATE FILE_SET CXX_MODULES FILES ${TEST_MOD_LIST})
target_include_directories(cyd_ui_test PRIVATE ${TEST_DIR})
target_link_libraries(cyd_ui_test
        PRIVATE cyd_ui
        sensors)

# Build Doxygen Documentation
add_custom_target(cyd_ui_docs_dir ALL COMMAND ${CMAKE_COMMAND} -E make_directory docs)
add_custom_target(
        cyd_ui_docs
        COMMAND /usr/bin/doxygen ${CMAKE_CURRENT_LIST_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMENT "Generating DOXYGEN documentation..."
)
add_dependencies(cyd_ui_docs cyd_ui_docs_dir)

#if (CMAKE_BUILD_TYPE MATCHES "Release")
#    add_custom_command(TARGET cyd_ui POST_BUILD
#            COMMAND /usr/bin/cp
#            -v
#            ${CMAKE_CURRENT_LIST_DIR}/cmake-build-release/libcyd_ui.a
#            ${CMAKE_CURRENT_LIST_DIR}/dist/
#            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#            COMMENT "Generating distribution files..."
#    )
#    add_custom_command(TARGET cyd_ui POST_BUILD
#            COMMAND /usr/bin/cp
#            -rv
#            ${INCLUDE_DIR}
#            ${CMAKE_CURRENT_LIST_DIR}/dist/
#            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#            COMMENT "Copying header files..."
#    )
#    add_custom_command(TARGET cyd_ui POST_BUILD
#            COMMAND /usr/bin/cp
#            -v
#            ${CMAKE_CURRENT_LIST_DIR}/cyd.yml
#            ${CMAKE_CURRENT_LIST_DIR}/dist/
#            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#            COMMENT "Copying CYD files..."
#    )
#    add_dependencies(cyd_ui cyd_ui_docs)
#endif ()
