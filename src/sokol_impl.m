// *************************************************************************
// *                      RevenantRevisited (port)                         *
// *  sokol_impl.m - Sokol implementation TU for Apple / Metal builds.     *
// *                                                                       *
// *  macOS only: sokol_app + sokol_gfx share the Metal backend and need   *
// *  to be compiled as Objective-C. On Windows / Linux, CMake drops this  *
// *  file and compiles sokol_impl.c against SOKOL_D3D11 / SOKOL_GLCORE33  *
// *  instead. The SOKOL_<BACKEND> macro is set by the Revenant CMake      *
// *  target so every TU that pulls in <sokol_gfx.h> sees the same        *
// *  backend selection.                                                  *
// *************************************************************************

#define SOKOL_IMPL

#import <sokol_gfx.h>
#import <sokol_app.h>
#import <sokol_glue.h>
#import <sokol_time.h>
#import <sokol_args.h>
