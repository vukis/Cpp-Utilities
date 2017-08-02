file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

add_custom_target(
        cppcheck
        COMMAND /usr/bin/cppcheck
		--enable=all
  #      --std=c++11
   #     --library=qt.cfg
        --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
        --verbose
   #     --quiet
        ${ALL_SOURCE_FILES}
)