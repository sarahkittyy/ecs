cmake_minimum_required(VERSION 3.18)
project(ecs)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build the tests." ON)

file(GLOB_RECURSE sources "src/*.cpp")

add_library(ecs ${sources})
add_subdirectory(ext/range-v3)

if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(test/)
endif()

if(BUILD_SANDBOX)
	add_subdirectory(sandbox/)
endif()

target_compile_options(ecs PUBLIC -Wall -Wno-unused-variable)
target_include_directories(ecs PUBLIC
	"include/"
	PRIVATE
	"src/"
	"ext/range-v3/include/"
)
target_link_libraries(ecs
	range-v3::range-v3
)
