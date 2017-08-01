add_custom_target(
        cppcheck
  #      COMMAND /usr/bin/cppcheck
 #       --enable=warning,performance,portability,information,missingInclude
  #      --std=c++11
   #     --library=qt.cfg
        --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
        --verbose
   #     --quiet
        ${ALL_SOURCE_FILES}
)