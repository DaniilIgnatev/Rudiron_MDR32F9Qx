cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_VERBOSE_MAKEFILE OFF)

file(GLOB SOURCES "src/*")

add_library(DummyLibrary STATIC ${SOURCES})
target_include_directories(DummyLibrary PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)