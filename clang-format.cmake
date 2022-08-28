
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

add_custom_target(
        clang-format
        COMMAND /usr/bin/clang-format -i --style=file ${ALL_SOURCE_FILES})