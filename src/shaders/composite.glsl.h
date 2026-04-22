// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *    composite.glsl.h  - GLSL composite quad (GL 3.3 core variant)      *
// *************************************************************************
//
// Linux / SOKOL_GLCORE33 port of composite.metal.h. Same uniform layout
// and math; only the language spelling differs.
//
// *************************************************************************

#pragma once

inline constexpr const char* kCompositeVsGlsl = R"GLSL(
#version 330 core
layout(std140) uniform params {
    vec4 rect;
    vec4 uv_rect;
};
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;
out vec2 v_uv;
void main() {
    gl_Position = vec4(rect.xy + pos * rect.zw, 0.0, 1.0);
    v_uv = uv_rect.xy + uv * uv_rect.zw;
}
)GLSL";

inline constexpr const char* kCompositeFsGlsl = R"GLSL(
#version 330 core
in vec2 v_uv;
out vec4 frag_color;
uniform sampler2D tex;
void main() {
    frag_color = texture(tex, v_uv);
}
)GLSL";
