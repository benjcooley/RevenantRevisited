cmake_minimum_required(VERSION 3.15)

# Project details. OBJC is only needed on Apple for the Metal-backed
# sokol_impl.m; enable it unconditionally where available -- CMake will
# just ignore OBJC if no .m files end up in the build on a given platform.
project(RevenantRevisited
  VERSION 1.0
  DESCRIPTION "Revenant PC 1998 built for modern machines."
  LANGUAGES C CXX
)
if(APPLE)
  enable_language(OBJC)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------------------------------------------
# Sokol backend selection.
#   Apple  -> Metal        (SOKOL_METAL, sokol_impl.m)
#   Win32  -> Direct3D 11  (SOKOL_D3D11, sokol_impl.c)
#   Linux  -> OpenGL 3.3   (SOKOL_GLCORE33, sokol_impl.c)
# Override with -DSOKOL_BACKEND=METAL|D3D11|GLCORE33 on the CMake command
# line when cross-configuring for a non-default backend.
# ----------------------------------------------------------------------------
if(NOT DEFINED SOKOL_BACKEND)
  if(APPLE)
    set(SOKOL_BACKEND "METAL")
  elseif(WIN32)
    set(SOKOL_BACKEND "D3D11")
  elseif(UNIX)
    set(SOKOL_BACKEND "GLCORE33")
  else()
    message(FATAL_ERROR "Unsupported platform; set -DSOKOL_BACKEND manually.")
  endif()
endif()
message(STATUS "Sokol backend: ${SOKOL_BACKEND}")

# Define paths for third-party libraries
set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")
set(SOKOL_DIR "${THIRDPARTY_DIR}/sokol")
set(HANDMADE_MATH_DIR "${THIRDPARTY_DIR}/handmademath")
set(SIMPLEINI_DIR "${THIRDPARTY_DIR}/simpleini")
set(MINIZ_DIR "${THIRDPARTY_DIR}/miniz")
set(LOG_DIR "${THIRDPARTY_DIR}/log")
set(ARGH_DIR "${THIRDPARTY_DIR}/argh")
set(STB_DIR "${THIRDPARTY_DIR}/stb")
set(IMGUI_DIR "${THIRDPARTY_DIR}/imgui")

# Dear ImGui core (no backends -- sokol_imgui.h provides the backend).
add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR})

# Gather C++ sources. Sokol TU is added explicitly per-backend below so
# the GLOB doesn't pick up the wrong one on a given platform.
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.h")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/countlines.cpp")

if(SOKOL_BACKEND STREQUAL "METAL")
  list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/src/sokol_impl.m")
else()
  list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/src/sokol_impl.c")
endif()

add_executable(Revenant ${SOURCES} ${MINIZ_DIR}/miniz.c ${LOG_DIR}/log.c)
target_link_libraries(Revenant PRIVATE imgui)

# Propagate the SOKOL_<BACKEND> macro to every TU that pulls in
# <sokol_gfx.h> / <sokol_app.h> / sokol_imgui.h.
target_compile_definitions(Revenant PRIVATE SOKOL_${SOKOL_BACKEND})

# Include directories for third-party dependencies
target_include_directories(Revenant PRIVATE
  ${SOKOL_DIR}
  ${HANDMADE_MATH_DIR}
  ${SIMPLEINI_DIR}
  ${MINIZ_DIR}
  ${LOG_DIR}
  ${ARGH_DIR}
  ${STB_DIR}
  ${IMGUI_DIR}
)

# ----------------------------------------------------------------------------
# Platform libs / frameworks.
# ----------------------------------------------------------------------------
if(APPLE)
  target_link_libraries(Revenant PRIVATE
    "-framework Metal"
    "-framework MetalKit"
    "-framework Cocoa"
    "-framework AppKit"
    "-framework QuartzCore"
    "-framework Foundation"
    "-framework AudioToolbox")
elseif(WIN32)
  # D3D11 + Win32 + WASAPI (sokol_audio uses WASAPI on Windows).
  target_link_libraries(Revenant PRIVATE
    d3d11 dxgi d3dcompiler
    user32 gdi32 ole32 shell32 kernel32)
elseif(UNIX)
  # GL + X11 input + ALSA (sokol_audio) + dl + pthread.
  find_package(Threads REQUIRED)
  find_package(OpenGL REQUIRED)
  target_link_libraries(Revenant PRIVATE
    OpenGL::GL
    X11 Xi Xcursor Xrandr
    asound
    dl
    Threads::Threads)
endif()
