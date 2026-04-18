// Instantiates sokol_imgui.h against native C++ Dear ImGui. sokol_gfx and
// sokol_app implementations live in sokol_impl.m; this TU only pulls in
// their declarations and emits the simgui_* implementation.

#include "imgui.h"

#define SOKOL_METAL
#define SOKOL_IMGUI_IMPL
#include <sokol_gfx.h>
#include <sokol_app.h>
#include "sokol_imgui.h"
