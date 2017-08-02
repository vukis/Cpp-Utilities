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
  #      --error-exitcode=1
		--check-config
        ${ALL_SOURCE_FILES}
)