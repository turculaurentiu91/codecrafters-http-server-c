cmake_minimum_required(VERSION 3.13)

project(codecrafters-http-server)

find_package(ZLIB REQUIRED)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

add_executable(http-server ${SOURCE_FILES})
target_link_libraries(http-server PRIVATE ZLIB::ZLIB)
target_include_directories(http-server PRIVATE ${ZLIB_INCLUDE_DIRS})
add_executable(strings_test src/strings.h src/strings.c tests/strings_test.c)
