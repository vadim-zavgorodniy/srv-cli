cmake_minimum_required(VERSION 2.6)

get_filename_component(PROJECT_DIR "." ABSOLUTE)
include_directories(${PROJECT_DIR}/libs)

if (NOT DEFINED debug)
  set(debug FALSE)
endif()

if (debug)
  set(generic_compile_options "${generic_compile_options} -g -D_DEBUG")
else()
  set(generic_compile_options "${generic_compile_options} -O2 -DNDEBUG")
endif()

add_subdirectory(libs)
add_subdirectory(src)
