cmake_minimum_required(VERSION 3.21)
project(RTMPServer)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(GCC_COVERAGE_COMPILE_FLAGS-Wall)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#file(GLOB_RECURSE Foo_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(RTMPServer
        src/RTMPRequestHandler.cpp
        src/Queue.cpp
        src/RandomByteGenerator.cpp
        server.cpp
        src/Logger.cpp)
target_include_directories(RTMPServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(RTMPServer PRIVATE Threads::Threads)






