// *************************************************************************
// *                      RevenantRevisited (port)                         *
// *  sokol_impl.c - Sokol implementation TU for Windows / Linux builds.   *
// *                                                                       *
// *  Non-Apple path: sokol_app + sokol_gfx compile as plain C against     *
// *  either SOKOL_D3D11 (Windows) or SOKOL_GLCORE33 (Linux). Apple ships  *
// *  with sokol_impl.m instead, where the Metal backend requires an       *
// *  Objective-C TU. The SOKOL_<BACKEND> macro is set by the Revenant     *
// *  CMake target so every TU that pulls in <sokol_gfx.h> sees the same   *
// *  backend selection.                                                   *
// *************************************************************************

#define SOKOL_IMPL

#include <sokol_gfx.h>
#include <sokol_app.h>
#include <sokol_glue.h>
#include <sokol_time.h>
#include <sokol_args.h>
