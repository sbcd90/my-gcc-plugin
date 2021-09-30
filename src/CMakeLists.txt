cmake_minimum_required(VERSION 3.16)
project(my_gcc_plugin)

set(CMAKE_CXX_STANDARD 11)
set(GCC_SRC_DIR "/home/sbcd90/Documents/programs/gcc-10/gcc-10.1.0")
set(GCC_BUILD_DIR "/home/sbcd90/Documents/programs/gcc-10/gcc-build")
set(GCC_PLUGIN_DIR "/home/sbcd90/Documents/programs/gcc-10/gcc-install/lib/gcc/x86_64-pc-linux-gnu/10.1.0/plugin")
set(GCC_LIB_CPP "/home/sbcd90/Documents/programs/gcc-10/gcc-10.1.0/libcpp")

add_library(my_gcc_plugin MODULE warn_unused.cpp gcc-pass.cpp)
set_target_properties(my_gcc_plugin PROPERTIES PREFIX "")
target_include_directories(my_gcc_plugin PUBLIC ${GCC_PLUGIN_DIR}/include include/)
target_compile_options(my_gcc_plugin PRIVATE -Wall -Wextra -pedantic -fno-rtti -g -O3)