# Copyright (c) 2025, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later

################################################################################
#[[                     CONFIGURE DOXYGEN DOCUMENTATION                      ]]#
################################################################################
#include(FetchContent)
#FetchContent_Declare(
#    doxygen-awesome-css
#    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
#)
#FetchContent_MakeAvailable(doxygen-awesome-css)
#
## Save the location the files were cloned into
## This allows us to get the path to doxygen-awesome.css
#FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)
#
## Generate the Doxyfile
#set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
#set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
#configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

if (CYD_UI_DEV_MODE AND CYD_UI_BUILD_DOCS)
    target_configure_doxygen(cyd_ui
            DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
            BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
endif ()
