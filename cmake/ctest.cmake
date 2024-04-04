enable_testing()

FILE(GLOB_RECURSE TEST_LIST
        ${CMAKE_CURRENT_LIST_DIR}/../test/**/*.cpp
)
foreach (test ${TEST_LIST})
    get_filename_component(TName ${test} NAME_WLE)
    add_executable(${TName} ${test})
    target_link_libraries(${TName} PRIVATE cyd_ui)
    target_include_directories(${TName} PRIVATE ${TEST_DIR}/common)

    file(STRINGS ${test} TLines)
    message("TEST CASES")
    foreach (line ${TLines})
        if ("${line}" MATCHES "TEST\\(\"(.*)\"\\)")
            message("${line}")
            set(case ${CMAKE_MATCH_1})
            message("${case}")

            add_test(NAME "${TName} - ${case}" COMMAND $<TARGET_FILE:${TName}> "${case}")
        endif ()
    endforeach ()
endforeach ()
