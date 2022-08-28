
add_custom_target(
        cppcheck
        COMMAND /usr/bin/cppcheck
        --enable=warning,performance,portability,information,missingInclude
        --std=c++23
        --verbose
        --quiet
        --template="[{severity}:{id}]\\t{file}:{line}:\\n\\t{message}\\n{code}\\n"
        ${PROJECT_SOURCE_DIR}/{src,include,test})