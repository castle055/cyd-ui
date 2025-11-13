# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later


################################################################################
#[[                            PROJECT OVERVIEW                              ]]#
################################################################################
#                                                                    CYD-UI    #
#                                                                              #
#[[  AVAILABLE OPTIONS:                                                      ]]#
#      - CYD_UI_BUILD_TESTS ............................. DEV_MODE only, ON    #
#            whether or not tests should be built                              #
#      - CYD_UI_BUILD_DOCS .............................. DEV_MODE only, ON    #
#            whether or not the documentation should be built                  #
#                                                                              #
#[[  CMAKE STRUCTURE:                                                        ]]#
#      - Project setup                                                         #
#      - Configure dependencies                                                #
#      - Configure CYD UI                                                      #
#      - Configure tests                                                       #
#      - Configure Doxygen documentation                                       #
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
option(CYD_UI_PROFILING "whether or not to enable Tracy profiler" OFF)
#option(CYD_UI_PROFILING "whether or not to enable Tracy profiler" ON)

#[[                         DEVELOPER MODE OPTIONS                           ]]#
if(CYD_UI_DEV_MODE)
    option(CYD_UI_BUILD_TESTS "whether or not tests should be built" ON)
    option(CYD_UI_BUILD_DOCS "whether or not the documentation should be built" ON)
endif ()

# Select 'Release' build type by default.
# Has to be done before the call to `project()`.
# Use `-DCMAKE_BUILD_TYPE=` to override this.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")

cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -pthread -fexperimental-library")

project(cyd_ui LANGUAGES CXX)

SET(CYD_UI_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(CYD_UI_TEST_DIR ${CYD_UI_DIR}/test)
SET(CYD_UI_INCLUDE_DIR ${CYD_UI_DIR}/include)
SET(CYD_UI_SOURCE_DIR ${CYD_UI_DIR}/src)
SET(CYD_UI_CMAKE_DIR ${CYD_UI_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CYD_UI_CMAKE_DIR}/Modules)


################################################################################
#[[                          CONFIGURE DEPENDENCIES                          ]]#
################################################################################
include(ExternalProject)
include(FetchContent)
find_package(PkgConfig)

#[[                          CONFIGURE CAIROMM-1.16                          ]]#
#find_package(CAIROMM)

##[[                          CONFIGURE CPP REFLECT                           ]]#
#FetchContent_Declare(cpp_reflect
#        GIT_REPOSITORY https://github.com/castle055/cpp-reflect.git
#        GIT_TAG dev
#        FIND_PACKAGE_ARGS
#)
#FetchContent_MakeAvailable(cpp_reflect)

#[[                           CONFIGURE CYD FABRIC                           ]]#
FetchContent_Declare(cyd_fabric
        GIT_REPOSITORY https://github.com/castle055/cyd-fabric.git
        GIT_TAG dev
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(cyd_fabric)

#[[                           CONFIGURE QUANTIFY                             ]]#
FetchContent_Declare(quantify
        GIT_REPOSITORY https://github.com/castle055/quantify.git
        GIT_TAG main
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(quantify)

#[[                            CONFIGURE TRACY                               ]]#

option(TRACY_ENABLE "" ${CYD_UI_PROFILING})
option(TRACY_ON_DEMAND "" OFF)
FetchContent_Declare (
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.12.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(tracy)

#[[                         CONFIGURE GOOGLE TESTS                           ]]#
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.17.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#[[                             CONFIGURE SDL                                ]]#
# Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if none can be found
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

#[[                           CONFIGURE CAIROMM                              ]]#
pkg_check_modules(cairomm REQUIRED IMPORTED_TARGET cairomm-1.16)
#[[                           CONFIGURE PANGOMM                              ]]#
pkg_check_modules(pangomm REQUIRED IMPORTED_TARGET pangomm-2.48)

#[[                             CONFIGURE NFD                                ]]#
FetchContent_Declare(nfd
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
        GIT_TAG v1.2.1
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(nfd)
#add_subdirectory(path/to/nativefiledialog-extended)

################################################################################
#[[                             CONFIGURE CYD UI                             ]]#
################################################################################
add_library(cyd_ui)
reflect_target(cyd_ui)

FILE(GLOB_RECURSE EXPORT_HEADERS CONFIGURE_DEPENDS
        ${CYD_UI_INCLUDE_DIR}/*.h
        ${CYD_UI_INCLUDE_DIR}/*.hpp
        ${CYD_UI_INCLUDE_DIR}/*.inc
)
FILE(GLOB_RECURSE SOURCE_HEADERS CONFIGURE_DEPENDS
        ${CYD_UI_SOURCE_DIR}/*.h
        ${CYD_UI_SOURCE_DIR}/*.hpp
        ${CYD_UI_SOURCE_DIR}/*.inc
)

FILE(GLOB_RECURSE EXPORT_MODULES CONFIGURE_DEPENDS
        ${CYD_UI_INCLUDE_DIR}/*.cppm
)
FILE(GLOB_RECURSE SOURCE_MODULES CONFIGURE_DEPENDS
        ${CYD_UI_SOURCE_DIR}/*.cppm
)
FILE(GLOB_RECURSE MODULE_IMPLS CONFIGURE_DEPENDS
        ${CYD_UI_SOURCE_DIR}/*.cxx
)

target_include_directories(cyd_ui PUBLIC ${CYD_UI_INCLUDE_DIR})
target_include_directories(cyd_ui PUBLIC ${cyd_fabric_SOURCE_DIR}/include)
target_compile_features(cyd_ui PUBLIC cxx_std_23)

# Yes, this is supposed to be bad because it's incompatible with member field initialization order.
# However, if you see this line here, it's because that's not been a problem yet :)
target_compile_options(cyd_ui PUBLIC -Wno-reorder-init-list)

target_sources(cyd_ui
        PUBLIC ${EXPORT_HEADERS}

        PUBLIC FILE_SET cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${CYD_UI_INCLUDE_DIR}
        FILES ${EXPORT_MODULES}

        PRIVATE ${SOURCE_HEADERS}
        PRIVATE ${MODULE_IMPLS}

        PRIVATE FILE_SET cxx_module_impls
        TYPE CXX_MODULES
        BASE_DIRS ${CYD_UI_SOURCE_DIR}
        FILES ${SOURCE_MODULES}
)

target_link_libraries(cyd_ui
        PUBLIC
        cyd_fabric
        quantify
        TracyClient
        SDL3::SDL3
        PkgConfig::cairomm
        PkgConfig::pangomm
        PRIVATE
        nfd
)

if (${CYD_UI_PROFILING})
    target_compile_definitions(cyd_ui PRIVATE CYD_UI_PROFILING)
endif ()

################################################################################
#[[                             CONFIGURE TESTS                              ]]#
################################################################################
#include(CTest)
enable_testing()
include(GoogleTest)
if (CYD_UI_DEV_MODE AND CYD_UI_BUILD_TESTS)
    include(cmake/ctest.cmake)
    target_configure_test_directory(cyd_ui ${CYD_UI_TEST_DIR} TEST_TARGET_LIST)
    foreach (TEST IN LISTS TEST_TARGET_LIST)
        reflect_target(${TEST})
    endforeach ()
endif ()

################################################################################
#[[                     CONFIGURE DOXYGEN DOCUMENTATION                      ]]#
################################################################################
if (CYD_UI_DEV_MODE AND CYD_UI_BUILD_DOCS)
    target_configure_doxygen(cyd_ui
            DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
            BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif ()

