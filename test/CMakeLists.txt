cmake_minimum_required(VERSION 3.18)
project(ecs)

find_package(Catch2 REQUIRED)

file(GLOB sources "*.cpp")
add_executable(ecs-test ${sources})
add_dependencies(ecs-test ecs)
target_link_libraries(ecs-test ecs Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(ecs-test)
