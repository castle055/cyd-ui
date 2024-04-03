enable_testing()

FILE(GLOB_RECURSE TEST_LIST
        ${CMAKE_CURRENT_LIST_DIR}/../test/**/*.cpp
)
foreach (test ${TEST_LIST})
    get_filename_component(TName ${test} NAME_WLE)
    add_executable(${TName} ${test})
    target_link_libraries(${TName} PRIVATE cyd_ui)
    target_include_directories(${TName} PRIVATE ${TEST_DIR}/common)
    add_test(NAME ${TName} COMMAND $<TARGET_FILE:${TName}>)
endforeach ()
