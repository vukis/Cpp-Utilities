# Hack
# file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

add_custom_target(
        cppcheck
        COMMAND /usr/bin/cppcheck
		--enable=all
		--suppress=missingIncludeSystem
        --std=c++11
        --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
        --verbose
		# --check-config
	    # --error-exitcode=1
		# -- I [dir] // to include STL/Boost/Windows headers
        ${SOURCE_FILES_TO_ANALYZE}
)