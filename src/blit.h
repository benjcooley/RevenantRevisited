// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    blit.h - Blit Shader Header                       *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "sokol_gfx.h"

// Get shader descriptor for basic 2D blitting
sg_shader_desc blit_shader_desc(void);
// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    blit.h - Blit Shader Header                       *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "sokol_gfx.h"

// Shader uniforms structure
typedef struct {
    float mvp[16];
    float tex_scale[2]; 
} blit_vs_params_t;

// Get shader descriptor for basic 2D blitting
sg_shader_desc blit_shader_desc(void);

// Create pipeline for blitting
sg_pipeline_desc blit_pipeline_desc(void);

// Vertex format for blit quads
typedef struct {
    float position[3];
    float texcoord[2];
} blit_vertex_t;
