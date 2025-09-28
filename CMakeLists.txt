cmake_minimum_required(VERSION 3.15)

project(Basic_IR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_executable(Basic_IR main.cpp)

target_include_directories(Basic_IR PRIVATE ${CMAKE_SOURCE_DIR}/include)
 
target_compile_options(Basic_IR PRIVATE
    -Wall -Wextra -Wpedantic
)
