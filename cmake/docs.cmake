add_custom_target(
        cyd_ui_docs
        COMMAND /usr/bin/doxygen ${CMAKE_CURRENT_LIST_DIR}/../Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
        COMMENT "Generating DOXYGEN documentation..."
)
add_custom_command(TARGET cyd_ui_docs PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory docs
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
)
