cmake_minimum_required(VERSION 2.6)

get_filename_component(PROJECT_DIR "." ABSOLUTE)
include_directories(${PROJECT_DIR}/libs)

add_subdirectory(libs)
add_subdirectory(src)
