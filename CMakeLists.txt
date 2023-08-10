cmake_minimum_required(VERSION 3.25)
project(System)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS system json thread program_options)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE BEAST_FILES ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)
source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

add_executable(System
        main.cpp
        ${BEAST_FILES}
        services/http_configuration.h
        services/http_request.cpp
        services/http_request.h
        services/http_listener.cpp
        services/http_listener.h
        services/http_session.cpp
        services/http_session.h
        services/http_fail.cpp
        services/http_fail.h
        services/queue_configuration.h
        services/queue_task.h)

target_link_libraries(System PRIVATE -lssl -lcrypto ${Boost_LIBRARIES})