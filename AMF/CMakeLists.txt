cmake_minimum_required(VERSION 3.16)
project(RTMPServer)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#file(GLOB_RECURSE Foo_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_library(AMF OBJECT
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfarray.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfbool.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfbytearray.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfdate.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfdictionary.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfdouble.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfinteger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfobject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfpacket.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfstring.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfvector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfxml.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/amfxmldocument.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/deserializer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/serializationcontext.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/serializer.cpp
        )
target_include_directories(AMF PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)





