#### COMMON ####
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(flags "-O3 -Wall -Wextra -Wno-unused-parameter")

	set(CMAKE_CXX_FLAGS "${flags} -std=c++11"
		CACHE STRING "Flags used by the compiler during all build types." FORCE)
	set(CMAKE_C_FLAGS   "${flags}"
		CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

include_directories(
	${Boost_INCLUDE_DIRS}
	"${CMAKE_SOURCE_DIR}/submodules/spdlog/include"
	#"${CMAKE_SOURCE_DIR}/submodules/catch/single_include"
	"${CMAKE_SOURCE_DIR}/submodules/cppzmq"
	"${CMAKE_SOURCE_DIR}/submodules/predef/include"
)

find_library(LIBZMQ   zmq)
find_library(BOOSTFS  boost_filesystem-mt)
find_library(BOOSTSYS boost_system-mt)

set(BOTH_LINK_LIBRARIES
)

#### MAIN ####
add_library(spina-module
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Module.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleChain.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleDataMap.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp"
)
set_target_properties(spina-module
	PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
)
target_link_libraries(spina-module
	${BOTH_LINK_LIBRARIES}
)

add_library(spina
	$<TARGET_OBJECTS:SPINAOBJ>
)
set_target_properties(spina
	PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
)
target_link_libraries(spina
	${BOTH_LINK_LIBRARIES}
	${BOOSTFS}
	${BOOSTSYS}
	spina-module
)

#### TESTS ####
#include("${CMAKE_CURRENT_SOURCE_DIR}/src/tests/tests.cmake")
