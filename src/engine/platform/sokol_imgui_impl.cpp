// Instantiates sokol_imgui.h against native C++ Dear ImGui. sokol_gfx and
// sokol_app implementations live in sokol_impl.{m,c} (picked per-platform
// by CMake); this TU only pulls in their declarations and emits the
// simgui_* implementation against whichever SOKOL_<BACKEND> the CMake
// target defines for the entire project.

#include "imgui.h"

#define SOKOL_IMGUI_IMPL
#include <sokol_gfx.h>
#include <sokol_app.h>
#include "sokol_imgui.h"
