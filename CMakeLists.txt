cmake_minimum_required(VERSION 2.6)

if(CMAKE_COMPILER_IS_GNUCXX)
  exec_program(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE _compiler_version_output)
  string(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]).*" "\\1" gcc_compiler_version ${_compiler_version_output})
else()
  set(gcc_compiler_version "0")
endif()

set(generic_compile_options "-march=i586 -pipe -Wall -DBOOST_FILESYSTEM_VERSION=2")
set(generic_compile_options "${generic_compile_options} -DBOOST_FILESYSTEM_VERSION=2")
if (debug)
  set(generic_compile_options "${generic_compile_options} -g -D_DEBUG")
else()
  set(generic_compile_options "${generic_compile_options} -O2 -DNDEBUG")
endif()


#add_subdirectory(libs)
add_subdirectory(src)
