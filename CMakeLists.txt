cmake_minimum_required(VERSION 3.15...3.21)

project(RevenantRevisited
  VERSION
    1.0
  DESCRIPTION
    "Revenant PC 1998 built for modern machines."
  LANGUAGES
    CXX
)

set(CMAKE_CXX_STANDARD 11)

include_directories("thirdparty/sokol" "thirdparty/handmademath" )

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp" "src/*.h")

add_executable(Revenant ${SOURCES})
