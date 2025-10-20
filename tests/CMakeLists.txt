cmake_minimum_required(VERSION 3.10)

find_package(GTest REQUIRED)


add_executable(gtests tests.cpp)

target_link_libraries(gtests PRIVATE IRlib ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} glog::glog)