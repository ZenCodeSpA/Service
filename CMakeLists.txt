cmake_minimum_required(VERSION 3.25)
project(System)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS system json thread)

file(GLOB_RECURSE BEAST_FILES ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)
source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

add_executable(System main.cpp ${BEAST_FILES})
