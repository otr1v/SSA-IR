cmake_minimum_required(VERSION 3.15)

project(Basic_IR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(glog REQUIRED)
add_library(IRlib STATIC
    lib/BB.cpp
    lib/Graph.cpp
)

target_include_directories(IRlib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(IRlib PUBLIC glog::glog)

add_executable(Basic_IR main.cpp)

target_link_libraries(Basic_IR PRIVATE IRlib)
 
target_compile_options(Basic_IR PRIVATE
    -Wall -Wextra -Wpedantic
)

option(BUILD_TESTING "Build the tests for the project" ON) 
if (BUILD_TESTING)
    add_subdirectory(tests) 
endif()