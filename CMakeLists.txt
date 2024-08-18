# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later


################################################################################
#[[                            PROJECT OVERVIEW                              ]]#
################################################################################
#                                                                    CYD-UI    #
#                                                                              #
#[[  AVAILABLE OPTIONS:                                                      ]]#
#      - CYD_UI_BUILD_APPS .................. ON in DEV_MODE, OFF otherwise    #
#            whether or not app executables should be built                    #
#      - CYD_UI_BUILD_TESTS ............................. DEV_MODE only, ON    #
#            whether or not tests should be built                              #
#      - CYD_UI_BUILD_DOCS .............................. DEV_MODE only, ON    #
#            whether or not the documentation should be built                  #
#                                                                              #
#[[  CMAKE STRUCTURE:                                                        ]]#
#      - Project setup                                                         #
#      - Configure dependencies:                                               #
#          - CYD Fabric (dev)                                                  #
#      - Configure CYD UI                                                      #
#      - Configure CYD UI APPS                                                 #
#      - Configure tests                                                       #
#      - Configure Doxygen documentation                                       #
#      - Configure install                                                     #
#                                                                              #
################################################################################


################################################################################
#[[                              PROJECT SETUP                               ]]#
################################################################################
#[[                          PREVENT IN-TREE BUILD                           ]]#
if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. Create a `build/` directory and call `cmake ..` from there.")
endif ()

#[[                          CHECK DEVELOPER MODE                            ]]#
set(CYD_UI_DEV_MODE OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CYD_UI_DEV_MODE ON)
endif ()

#[[                           CONFIGURE OPTIONS                              ]]#
if (CYD_UI_DEV_MODE)
    option(CYD_UI_BUILD_APPS "whether or not app executables should be built" ON)
else ()
    option(CYD_UI_BUILD_APPS "whether or not app executables should be built" OFF)
endif ()

#[[                         DEVELOPER MODE OPTIONS                           ]]#
if(CYD_UI_DEV_MODE)
    option(CYD_UI_BUILD_TESTS "whether or not tests should be built" ON)
    option(CYD_UI_BUILD_DOCS "whether or not the documentation should be built" ON)
endif ()

# Select 'Release' build type by default.
# Has to be done before the call to `project()`.
# Use `-DCMAKE_BUILD_TYPE=` to override this.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")

cmake_minimum_required(VERSION 3.29.20240518 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cyd_ui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CYD_UI_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(CYD_UI_TEST_DIR ${CYD_UI_DIR}/test)
SET(CYD_UI_INCLUDE_DIR ${CYD_UI_DIR}/include)
SET(CYD_UI_SOURCE_DIR ${CYD_UI_DIR}/src)
SET(CYD_UI_APPS_DIR ${CYD_UI_DIR}/apps)

include(cmake/compiler_flags.cmake)
compiler_flags(
        "-stdlib=libc++"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"

        "-Wliteral-suffix"

        "-Wno-dollar-in-identifier-extension"
        "-Wno-macro-redefine"
        "-Wno-unused-function"
        "-Wno-unused-lambda-capture"
        "-Wno-unused-parameter"
        "-Wno-unused-variable"
        "-Wno-unused-value"
        "-Wno-unused-but-set-variable"
        "-Wno-missing-field-initializers"
        #"-Wno-error=unused"

        "-Wno-unknown-pragmas"
        "-Wno-extra-semi"
)


################################################################################
#[[                          CONFIGURE DEPENDENCIES                          ]]#
################################################################################
include(FetchContent)

#[[                           CONFIGURE CYD FABRIC                           ]]#
FetchContent_Declare(cyd_fabric
        GIT_REPOSITORY https://github.com/castle055/cyd-fabric.git
        GIT_TAG dev
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(cyd_fabric)


################################################################################
#[[                             CONFIGURE CYD UI                             ]]#
################################################################################
add_library(cyd_ui)

FILE(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
        ${CYD_UI_INCLUDE_DIR}/*.h
        ${CYD_UI_INCLUDE_DIR}/*.hpp
        ${CYD_UI_SOURCE_DIR}/*.h
        ${CYD_UI_SOURCE_DIR}/*.hpp
        ${CYD_UI_SOURCE_DIR}/*.cpp
)
FILE(GLOB_RECURSE SRC_MOD_LIST CONFIGURE_DEPENDS
        ${CYD_UI_INCLUDE_DIR}/*.cppm
        ${CYD_UI_SOURCE_DIR}/*.cppm
)
FILE(GLOB_RECURSE SRC_MODI_LIST CONFIGURE_DEPENDS
        ${CYD_UI_INCLUDE_DIR}/*.cppi
        ${CYD_UI_SOURCE_DIR}/*.cppi
)

target_include_directories(cyd_ui PUBLIC ${CYD_UI_INCLUDE_DIR})
target_include_directories(cyd_ui PUBLIC ${cyd_fabric_SOURCE_DIR}/include)
target_compile_features(cyd_ui PUBLIC cxx_std_23)

target_sources(cyd_ui
        PUBLIC ${SRC_LIST}
        PUBLIC ${SRC_MODI_LIST}
        PUBLIC
        FILE_SET cxx_modules
        #        FILE_SET "${APP_NAME}_modfiles"
        TYPE CXX_MODULES
        BASE_DIRS ${CYD_UI_SOURCE_DIR} ${CYD_UI_INCLUDE_DIR}
        FILES ${SRC_MOD_LIST}
)

target_link_libraries(cyd_ui
        PUBLIC cyd_fabric
)


################################################################################
#[[                          CONFIGURE CYD UI APPS                           ]]#
################################################################################
if (CYD_UI_BUILD_APPS)
    FILE(GLOB APPS_LIST CONFIGURE_DEPENDS
            ${CYD_UI_APPS_DIR}/**/CMakeLists.txt
    )
    if (NOT "${APPS_LIST}" STREQUAL "")
        foreach (APP IN LISTS APPS_LIST)
            message("Including app CMakeLists.txt: " ${APP})
            include(${APP})
        endforeach ()
    endif ()
endif ()


################################################################################
#[[                             CONFIGURE TESTS                              ]]#
################################################################################
enable_testing()
if (CYD_UI_DEV_MODE AND CYD_UI_BUILD_TESTS)
    target_configure_test_directory(cyd_ui ${CYD_UI_TEST_DIR})
endif ()


################################################################################
#[[                     CONFIGURE DOXYGEN DOCUMENTATION                      ]]#
################################################################################
if (CYD_UI_DEV_MODE AND CYD_UI_BUILD_DOCS)
    target_configure_doxygen(cyd_ui
            DOXYFILE ${CYD_UI_DIR}/Doxyfile
            BASE_DIR ${CYD_UI_DIR}
    )
endif ()



################################################################################
#[[                            CONFIGURE INSTALL                             ]]#
################################################################################
if (CYD_UI_DEV_MODE AND CMAKE_BUILD_TYPE MATCHES "Release")
    add_custom_command(TARGET cyd_ui POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory dist
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    add_custom_command(TARGET cyd_ui POST_BUILD
            COMMAND cp -v
            $<TARGET_FILE:cyd_ui>
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Copying binary into distribution directory..."
    )
    add_custom_command(TARGET cyd_ui POST_BUILD
            COMMAND cp -rv
            ${CYD_UI_INCLUDE_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Copying header files into distribution directory..."
    )
endif ()

