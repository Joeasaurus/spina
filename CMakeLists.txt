cmake_minimum_required(VERSION 3.2)
project(spina)
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE false)

message(STATUS "${CMAKE_VERSION}")
if (${CMAKE_VERSION} VERSION_GREATER 3.4)
    set(ZMQ_VOID_CONVERT "*")
endif()

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build")

add_library(SPINE OBJECT
	"${CMAKE_CURRENT_SOURCE_DIR}/src/spina/spina.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/spina/spina_commands.cpp"
)
add_library(SOCKETER OBJECT
	"${CMAKE_CURRENT_SOURCE_DIR}/src/messages/socketer.cpp"
)
add_library(ROUTING OBJECT
	"${CMAKE_CURRENT_SOURCE_DIR}/submodules/simple-cpp-router/src/Routing/Router.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/submodules/simple-cpp-router/src/Routing/PathMatch.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/submodules/simple-cpp-router/src/Routing/Details/PathTemplate.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/submodules/simple-cpp-router/src/Routing/Details/RegexConverter.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/submodules/simple-cpp-router/src/Routing/Details/PathTemplateParser.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/submodules/simple-cpp-router/src/Routing/Details/StringTokenizer.cpp"
)

include("${CMAKE_SOURCE_DIR}/${CMAKE_SYSTEM_NAME}.cmake")

install(TARGETS ${BINARY_NAME} spina spina-module
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/" DESTINATION include/spina FILES_MATCHING PATTERN "*.hpp")

## cppcheck --force --inline-suppr --template '{file}:{line},{severity},{id},{message}' \
##  --xml --enable=all --max-configs=25 -i /Users/jme/GDrive/code/cppmodules/src ./ 2> err.xml
## cppcheck-htmlreport --file=err.xml --title=CppModules --report-dir=cppcheck_reports --source-dir=.
## valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes -v cppmodules
