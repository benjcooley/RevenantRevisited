cmake_minimum_required(VERSION 3.15)

# Project details
project(RevenantRevisited
  VERSION 1.0
  DESCRIPTION "Revenant PC 1998 built for modern machines."
  LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define paths for third-party libraries
set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")
set(SOKOL_DIR "${THIRDPARTY_DIR}/sokol")
set(HANDMADE_MATH_DIR "${THIRDPARTY_DIR}/handmademath")

# Add executable target
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.h")
add_executable(Revenant ${SOURCES})

# Include directories for third-party dependencies
target_include_directories(Revenant PRIVATE
  ${SOKOL_DIR}
  ${HANDMADE_MATH_DIR}
)

# Link necessary frameworks for macOS (if applicable)
if(APPLE)
  target_link_libraries(Revenant PRIVATE "-framework Metal" "-framework AppKit")
endif()

