#pragma once

#include "sokol_gfx.h"

// Shader for basic 2D blitting
static sg_shader_desc blit_shader_desc(void) {
    sg_shader_desc desc = {};
    
    desc.vs.source =
        "#version 330\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec2 texcoord0;\n"
        "out vec2 uv;\n"
        "void main() {\n"
        "  gl_Position = vec4(position, 1.0);\n"
        "  uv = texcoord0;\n"
        "}\n";

    desc.fs.source =
        "#version 330\n"
        "uniform sampler2D tex;\n"
        "in vec2 uv;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "  frag_color = texture(tex, uv);\n"
        "}\n";

    return desc;
}
