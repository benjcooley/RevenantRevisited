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
    vec4 chroma_key;
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
    vec4 c = texture(tex, v_uv);
    if (chroma_key.x > 0.5 && c.r > 0.55 && c.g < 0.30 && c.b < 0.30)
        discard;
    frag_color = c;
}
)GLSL";

// The 2:1 reduction; see kCompositeReduceFsMetal.
inline constexpr const char* kCompositeReduceFsGlsl = R"GLSL(
#version 330 core
layout(std140) uniform params {
    vec4 rect;
    vec4 uv_rect;
    vec4 texel;
    vec4 color_tint;
};
in vec2 v_uv;
out vec4 frag_color;
uniform sampler2D tex;
void main() {
    vec2 h = 0.5 * texel.xy;
    vec4 block[4] = vec4[4](texture(tex, v_uv + vec2(-h.x, -h.y)),
                            texture(tex, v_uv + vec2( h.x, -h.y)),
                            texture(tex, v_uv + vec2(-h.x,  h.y)),
                            texture(tex, v_uv + vec2( h.x,  h.y)));
    const vec3 steps = vec3(8.0, 4.0, 8.0);
    vec3 sum = vec3(0.0);
    float opaque = 0.0;
    for (int i = 0; i < 4; ++i)
        if (block[i].a > 0.5)
        {
            vec3 c = floor(round(block[i].rgb * 255.0) / steps);
            sum += vec3(c.r * 8.0 + floor(c.g / 8.0), c.g, c.b);
            opaque += 1.0;
        }
    if (opaque < 3.0)
        discard;
    vec3 mean = floor(sum / (opaque * vec3(8.0, 1.0, 1.0)));
    const vec3 levels = vec3(32.0, 64.0, 32.0);
    frag_color = vec4((mean * steps + floor(mean / (levels / steps))) / 255.0, 1.0) * color_tint;
}
)GLSL";
