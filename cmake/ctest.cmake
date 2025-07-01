# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later

macro(target_configure_test_directory TEST_TARGET TEST_DIR TEST_TARGET_LIST)
    enable_testing()

    add_custom_target(TEST_SUITE_${TEST_TARGET})

    get_filename_component(TEST_DIR ${TEST_DIR} REALPATH)

    FILE(GLOB_RECURSE TEST_LIST
            ${TEST_DIR}/**/*.cpp
            ${TEST_DIR}/*.cpp
    )
    foreach (test ${TEST_LIST})
        set(TDir ${test})
        get_filename_component(TDir ${TDir} REALPATH)
        get_filename_component(TDir ${TDir} DIRECTORY)
        string(REPLACE ${TEST_DIR} "" TDir ${TDir})
        if (NOT "${TDir}" STREQUAL "")
            string(SUBSTRING ${TDir} 1 -1 TDir)
        endif ()


        get_filename_component(TName ${test} NAME_WLE)
        set(TFullName ${TName})
        if (NOT "${TDir}" STREQUAL "")
            set(TFullName "${TDir}/${TName}")
            string(REPLACE "/" "." TFullName ${TFullName})
        endif ()

        add_executable(${TFullName} ${test})
        LIST(APPEND "${TEST_TARGET_LIST}" "${TFullName}")
        target_link_libraries(${TFullName} PRIVATE ${TEST_TARGET})
        target_link_libraries(${TFullName} PUBLIC gtest gtest_main)
        target_include_directories(${TFullName} PRIVATE ${TEST_DIR}/common)

        add_dependencies(TEST_SUITE_${TEST_TARGET} ${TFullName})

        gtest_add_tests(
                TARGET      ${TFullName}
                TEST_PREFIX "${TFullName}::")

#
#        file(STRINGS ${test} TLines)
#        foreach (line ${TLines})
#            if ("${line}" MATCHES "^TEST\\(\"(.*)\"\\)")
#                set(case ${CMAKE_MATCH_1})
#                add_test(NAME "${TFullName}.${case}" COMMAND $<TARGET_FILE:TEST_${TName}> "${case}")
#            endif ()
#        endforeach ()
    endforeach ()
endmacro()