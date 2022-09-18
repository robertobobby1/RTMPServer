cmake_minimum_required(VERSION 3.21)
project(RTMPServer)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#AMF3 deserializer and serializer
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/AMF0)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/AMF3)

#file(GLOB_RECURSE Foo_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(RTMPServer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rtmp/RTMPConnexion.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rtmp/RTMPHeaders.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rtmp/RTMPHandshake.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rtmp/RTMPServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Buffer.cpp
        )
target_include_directories(RTMPServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Threads lib and AMF3 lib
target_link_libraries(RTMPServer PUBLIC Threads::Threads)
target_link_libraries(RTMPServer PUBLIC AMF0)
target_link_libraries(RTMPServer PUBLIC AMF3)
target_compile_options(RTMPServer PRIVATE -O2)
target_compile_options(RTMPServer PRIVATE -Wall)
target_compile_options(RTMPServer PRIVATE -g)
