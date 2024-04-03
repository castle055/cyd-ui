add_custom_target(cyd_ui_docs_dir
        ALL COMMAND ${CMAKE_COMMAND} -E make_directory docs
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
)
add_custom_target(
        cyd_ui_docs
        COMMAND /usr/bin/doxygen ${CMAKE_CURRENT_LIST_DIR}/../Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
        COMMENT "Generating DOXYGEN documentation..."
)
add_dependencies(cyd_ui_docs cyd_ui_docs_dir)
