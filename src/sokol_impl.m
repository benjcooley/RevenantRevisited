// *************************************************************************
// *                      RevenantRevisited (port)                         *
// *  sokol_impl.m - Translation unit that instantiates the sokol headers. *
// *                                                                       *
// *  macOS build only: sokol_app + sokol_gfx share the Metal backend and  *
// *  need to be compiled as Objective-C. Everything else in the project   *
// *  includes the sokol headers as declarations.                          *
// *************************************************************************

#define SOKOL_IMPL
#define SOKOL_METAL

#import <sokol_gfx.h>
#import <sokol_app.h>
#import <sokol_glue.h>
#import <sokol_time.h>
#import <sokol_args.h>
