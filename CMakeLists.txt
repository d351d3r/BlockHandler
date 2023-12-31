cmake_minimum_required(VERSION 3.10)
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
project(BlockHandler)

find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

add_compile_options(-Wall -Wextra -Werror -Wconversion)
add_compile_definitions(_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g3)
endif()
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O2")

# Только основная программа (здесь предполагается, что main.cpp — это ваш основной файл)
add_executable(BlockHandler src/main.cpp src/block_handler.cpp)
target_include_directories(BlockHandler PRIVATE include)
target_link_libraries(BlockHandler)

# Тесты (подключаем библиотеку Google Test)
enable_testing()
add_executable(BlockHandlerTests tests/block_handler_test.cpp src/block_handler.cpp)
target_include_directories(BlockHandlerTests PRIVATE include)
target_link_libraries(BlockHandlerTests gtest gtest_main)
add_test(NAME BlockHandlerTests COMMAND BlockHandlerTests)
