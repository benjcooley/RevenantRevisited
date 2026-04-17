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

// Shader vertex parameters
_STRUCTDEF(SBlitVertexParams)
struct SBlitVertexParams
{
    float mvp[16];
    float tex_scale[2];
};

// Vertex format for blit quads
_STRUCTDEF(SBlitVertex) 
struct SBlitVertex
{
    float position[3];
    float texcoord[2];
};

// Get shader descriptor for basic 2D blitting
sg_shader_desc GetBlitShaderDesc(void);

// Create pipeline for blitting 
sg_pipeline_desc GetBlitPipelineDesc(void);
