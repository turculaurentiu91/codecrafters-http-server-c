cmake_minimum_required(VERSION 3.13)

project(codecrafters-http-server)

find_package(ZLIB REQUIRED)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

add_executable(http-server ${SOURCE_FILES})
target_link_libraries(http-server PRIVATE ZLIB::ZLIB)
target_include_directories(http-server PRIVATE ${ZLIB_INCLUDE_DIRS})

# Tests
add_executable(strings_test src/strings.h src/strings.c tests/strings_test.c)
add_executable(string_dict_test src/strings.h src/strings.c src/string_dict.h src/string_dict.c tests/string_dict_test.c)
add_executable(request_test src/request.h src/request.c src/strings.h src/strings.c src/string_dict.h src/string_dict.c tests/request_test.c)
