cmake_minimum_required(VERSION 3.21)
project(RTMPServer)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(GCC_COVERAGE_COMPILE_FLAGS-Wall)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#AMF deserializer and serializer
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/AMF)

#file(GLOB_RECURSE Foo_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(RTMPServer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RTMPRequestHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Queue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/RandomByteGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RTMP.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/TimestampManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Bytes.cpp)
target_include_directories(RTMPServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Threads lib and AMF lib
target_link_libraries(RTMPServer PUBLIC Threads::Threads)
target_link_libraries(RTMPServer PUBLIC AMF)




