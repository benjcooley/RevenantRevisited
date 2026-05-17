cmake_minimum_required(VERSION 3.15)

# Project details. OBJC is only needed on Apple for the Metal-backed
# sokol_impl.m; enable it unconditionally where available -- CMake will
# just ignore OBJC if no .m files end up in the build on a given platform.
project(RevenantRevisited
  VERSION 1.0
  DESCRIPTION "Revenant PC 1998 built for modern machines."
  LANGUAGES C CXX
)
include(FetchContent)
if(APPLE)
  enable_language(OBJC)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------------------------------------------
# Sanitizer build (ASan + UBSan). Off by default — opt in by configuring
# with -DREVENANT_ASAN=ON. Use a separate build dir so the normal build
# stays unaffected:
#
#   cmake -S . -B build-asan -DREVENANT_ASAN=ON
#   cmake --build build-asan
#   ./build-asan/Revenant      # ASan runtime errors stream to stderr
#
# ASan overhead is ~2x runtime + ~3x memory; fine for catching crashes
# but not what you ship.
# ----------------------------------------------------------------------------
option(REVENANT_ASAN "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
if(REVENANT_ASAN)
  message(STATUS "Sanitizers: AddressSanitizer + UndefinedBehaviorSanitizer enabled")
  # vptr is disabled because several polymorphic effect classes
  # (TBloodEffect, TBurnEffect, TDripEffect, TPulpEffect, TParticle3DAnimator)
  # have all virtuals inline → no key function → no typeinfo emitted →
  # link error. Re-enable once those have an out-of-line virtual.
  add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=vptr
                      -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -O1)
  add_link_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=vptr)
endif()

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
set(MINIAUDIO_DIR "${THIRDPARTY_DIR}/miniaudio")
set(IMGUI_DIR    "${THIRDPARTY_DIR}/imgui")
set(IMGUIZMO_DIR "${THIRDPARTY_DIR}/imguizmo")
set(IMPLOT_DIR   "${THIRDPARTY_DIR}/implot")
set(IMGUIFD_DIR  "${THIRDPARTY_DIR}/imguifiledialog")
set(IMGUITGL_DIR "${THIRDPARTY_DIR}/imgui_toggle")
set(FONTS_DIR    "${THIRDPARTY_DIR}/fonts")

# Dear ImGui core + first-party extensions (ImGuizmo, ImPlot, ImGuiFileDialog).
# No backends here -- sokol_imgui.h provides the backend.
add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUIZMO_DIR}/ImGuizmo.cpp
  ${IMPLOT_DIR}/implot.cpp
  ${IMPLOT_DIR}/implot_items.cpp
  ${IMGUIFD_DIR}/ImGuiFileDialog.cpp
  ${IMGUITGL_DIR}/imgui_toggle.cpp
  ${IMGUITGL_DIR}/imgui_toggle_palette.cpp
  ${IMGUITGL_DIR}/imgui_toggle_presets.cpp
  ${IMGUITGL_DIR}/imgui_toggle_renderer.cpp
)
target_include_directories(imgui PUBLIC
  ${IMGUI_DIR} ${IMGUIZMO_DIR} ${IMPLOT_DIR} ${IMGUIFD_DIR} ${IMGUITGL_DIR} ${FONTS_DIR})

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

# Pick up the editor's macOS Obj-C++ shim. .mm files aren't part of the
# default *.cpp glob so add it explicitly. Skipped on non-Apple builds
# where there's no Cocoa to call into.
if(APPLE)
  list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/src/platform/macosx/editorplatform.mm")
  list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/src/platform/macosx/renderer_readback.mm")
  list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/src/platform/macosx/cursor.mm")
endif()

add_executable(Revenant ${SOURCES} ${MINIZ_DIR}/miniz.c ${LOG_DIR}/log.c)
target_link_libraries(Revenant PRIVATE imgui)

# Propagate the SOKOL_<BACKEND> macro to every TU that pulls in
# <sokol_gfx.h> / <sokol_app.h> / sokol_imgui.h.
target_compile_definitions(Revenant PRIVATE SOKOL_${SOKOL_BACKEND})

# Absolute path to vendored fonts so the editor can load them regardless of
# the current working directory at runtime.
target_compile_definitions(Revenant PRIVATE REV_FONTS_DIR="${FONTS_DIR}")
target_compile_definitions(Revenant PRIVATE REV_EDITOR_DATA_DIR="${CMAKE_SOURCE_DIR}/data/editor")

# Include directories for third-party dependencies
target_include_directories(Revenant PRIVATE
  ${SOKOL_DIR}
  ${HANDMADE_MATH_DIR}
  ${SIMPLEINI_DIR}
  ${MINIZ_DIR}
  ${LOG_DIR}
  ${ARGH_DIR}
  ${STB_DIR}
  ${MINIAUDIO_DIR}
  ${IMGUI_DIR}
)

add_executable(test_defdoc tools/test_defdoc.cpp)

add_executable(test_render_metadata
  tools/test_render_metadata.cpp
  src/render_metadata.cpp)

add_executable(test_anim_system
  tools/test_anim_system.cpp
  src/animsystem.cpp)

add_executable(test_asset_cache
  tools/test_asset_cache.cpp
  src/assetcache.cpp)

add_executable(test_transform
  tools/test_transform.cpp
  src/transform.cpp
  src/animsystem.cpp
  src/math3d.cpp)
target_include_directories(test_transform PRIVATE
  ${HANDMADE_MATH_DIR})

add_executable(test_particlefx
  tools/test_particlefx.cpp
  src/particlefx.cpp)
target_include_directories(test_particlefx PRIVATE
  ${SOKOL_DIR}
  ${HANDMADE_MATH_DIR})

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
target_link_libraries(test_particlefx PRIVATE GTest::gtest_main)

add_executable(cpu_tile_perspective_test
  tools/cpu_tile_perspective_test.cpp)

# Pin parser behavior with regression tests. Only compiles parse.cpp
# (plus a tiny FatalError stub baked into the test) so we avoid pulling
# in the rest of the engine. parse.h includes revenant.h, which expects
# the same include dirs and SOKOL_<backend> define as the main target.
add_executable(test_parse
  tools/test_parse.cpp
  src/parse.cpp)
target_include_directories(test_parse PRIVATE
  ${SOKOL_DIR}
  ${HANDMADE_MATH_DIR}
  ${SIMPLEINI_DIR}
  ${MINIZ_DIR}
  ${LOG_DIR}
  ${ARGH_DIR}
  ${STB_DIR}
  ${IMGUI_DIR}
)
target_compile_definitions(test_parse PRIVATE SOKOL_${SOKOL_BACKEND})

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
