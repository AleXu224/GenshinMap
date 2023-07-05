cmake_minimum_required(VERSION 3.23.0)
project(map VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

file(GLOB_RECURSE all_files CONFIGURE_DEPENDS "src/*.cpp" "include/*.h")

add_executable(map main.cpp ${all_files})
target_include_directories(map PUBLIC include)

add_subdirectory(glt)
target_link_libraries(map PRIVATE glt)
find_package(simdjson CONFIG REQUIRED)
target_link_libraries(map PRIVATE simdjson::simdjson)
