// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    blit.cpp - Blit Shader Implementation             *
// *************************************************************************

#include "blit.h"

sg_shader_desc GetBlitShaderDesc(void)
{
    sg_shader_desc desc = {};
    
    desc.vs.source =
        "#version 330\n"
        "uniform SBlitVertexParams {\n"
        "    mat4 mvp;\n"
        "    vec2 tex_scale;\n"
        "};\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec2 texcoord0;\n"
        "out vec2 uv;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "    uv = texcoord0 * tex_scale;\n"
        "}\n";

    desc.fs.source =
        "#version 330\n"
        "uniform sampler2D tex;\n"
        "in vec2 uv;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = texture(tex, uv);\n"
        "}\n";

    return desc;
}

sg_pipeline_desc GetBlitPipelineDesc(void)
{
    sg_pipeline_desc desc = {};
    desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    desc.shader = sg_make_shader(GetBlitShaderDesc());
    desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    desc.colors[0].blend.enabled = true;
    desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    return desc;
}
