#### COMMON ####
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(flags "-O3 -Wall -Wextra -Wno-unused-parameter")

	set(CMAKE_CXX_FLAGS "${flags} -std=c++11"
		CACHE STRING "Flags used by the compiler during all build types." FORCE)
	set(CMAKE_C_FLAGS   "${flags}"
		CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

find_package(Boost REQUIRED)
include_directories("${CMAKE_SOURCE_DIR}/src" "${CMAKE_BINARY_DIR}/generated")
include_directories(
	${Boost_INCLUDE_DIRS}
	"${CMAKE_SOURCE_DIR}/submodules/spdlog/include"
	#"${CMAKE_SOURCE_DIR}/submodules/catch/single_include"
	"${CMAKE_SOURCE_DIR}/submodules/cppzmq"
	"${CMAKE_SOURCE_DIR}/submodules/predef/include"
	"${CMAKE_SOURCE_DIR}/submodules/simple-cpp-router/src"
)

find_library(LIBZMQ   zmq)

set(BOTH_LINK_LIBRARIES
	${LIBZMQ}
)

#### MAIN ####
add_library(spina-module
	$<TARGET_OBJECTS:ROUTING>
	$<TARGET_OBJECTS:SOCKETER>
	"${CMAKE_CURRENT_SOURCE_DIR}/src/uri/uri.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/uri/urirouter.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/module.cpp"
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
	$<TARGET_OBJECTS:ROUTING>
	$<TARGET_OBJECTS:SPINE>
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
