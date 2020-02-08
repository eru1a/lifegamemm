project(lifegamemm)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --pedantic-error")

# Ninjaでも色をつける
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options (-fcolor-diagnostics)
endif ()

add_subdirectory(src)
