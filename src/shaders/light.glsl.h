// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *     light.glsl.h  - GLSL deferred lighting (GL 3.3 core variant)      *
// *************************************************************************
//
// Linux / SOKOL_GLCORE33 port of light.metal.h.
//
// NOTE: KPL must match TRenderer::kMaxPointLights in renderer.h. If you
// change one, change the other.
//
// *************************************************************************

#pragma once

inline constexpr const char* kLightVsGlsl = R"GLSL(
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;
out vec2 v_uv;
void main() {
    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);
    v_uv = uv;
}
)GLSL";

inline constexpr const char* kLightFsGlsl = R"GLSL(
#version 330 core
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
#define KPL 16
layout(std140) uniform params {
    vec4 vp;
    vec4 recon;
    vec4 light_dir;
    vec4 light_col;
    vec4 ambient_col;
    vec4 settings;
    vec4 plight_pos[KPL];
    vec4 plight_col[KPL];
    vec4 shadow;
    vec4 shadow_dir;
    vec4 shadow_world_dir;
    vec4 normal_lighting;
};
in vec2 v_uv;
uniform sampler2D albedo_tex;
uniform sampler2D normal_tex;
uniform sampler2D depth_tex;
uniform sampler2D ao_tex;
out vec4 frag_color;
void main() {
    vec4 alb = texture(albedo_tex, v_uv);
    if (alb.a < 0.01) discard;
    float d  = texture(depth_tex, v_uv).r;
    vec3  np = texture(normal_tex, v_uv).xyz;
    vec3  N  = normalize(np * 2.0 - 1.0);
    float ao = texture(ao_tex, v_uv).r;
    vec2  ts = vec2(textureSize(albedo_tex, 0));
    float fbw = ts.x, fbh = ts.y;
    float S   = v_uv.x * fbw - vp.x;
    float T   = v_uv.y * fbh - vp.y;
    float scene_z = d * vp.w + vp.z;
    float K   = recon.z - scene_z;
    float wz  = (K - 2.0 * T * ISO_COS30) / ISO_WZ_DENOM;
    float sum_r = 2.0 * (T + wz * ISO_COS30);
    vec3  W = vec3(recon.x + (sum_r + S) * 0.5,
                   recon.y + (sum_r - S) * 0.5,
                   wz);
    int vm = int(settings.x);
    if (vm == 1) { frag_color = vec4(alb.rgb, 1.0); return; }
    if (vm == 2) { float vd = clamp(1.0 - d, 0.0, 1.0); frag_color = vec4(vd, vd, vd, 1.0); return; }
    if (vm == 3) { frag_color = vec4(np, 1.0); return; }
    int nl_dbg = int(settings.y);
    if (vm == 4) {
        vec3 accum = vec3(0.0);
        const float kMP = 0.0022436;
        const float kSC = 50.0 / (1.0 - kMP);
        for (int i = 0; i < nl_dbg; ++i) {
            vec3  delta = plight_pos[i].xyz - W;
            float dist  = length(delta);
            float rad   = plight_pos[i].w;
            if (rad > 0.0 && dist < rad) {
                vec3  Lp    = delta / max(dist, 1e-5);
                float pterm = mix(1.0, max(dot(N, Lp), 0.0), clamp(normal_lighting.x, 0.0, 1.0));
                float normd = dist * 254.0 / rad;
                float pw    = pow(normd + 1.0, -1.1) - kMP;
                float attn  = clamp(pw * kSC, 0.0, 1.0);
                accum += plight_col[i].rgb * plight_col[i].w * attn * pterm;
            }
        }
        frag_color = vec4(accum, 1.0); return;
    }
    if (vm == 5) {
        float ws = max(settings.z, 1.0);
        float zs = max(settings.w, 1.0);
        frag_color = vec4(fract(W.x / ws), fract(W.y / ws), fract(W.z / zs), 1.0);
        return;
    }
    if (vm == 7) { frag_color = vec4(ao, ao, ao, 1.0); return; }
    vec3 light = ambient_col.rgb * light_col.w;
    int mode = int(settings.z);
    if (mode == 1) light *= ao;
    float sun_shadow = 1.0;
    vec3  Ldir = normalize(light_dir.xyz);
    float sun_ndotl = max(dot(N, Ldir), 0.0);
    float normal_hardness = clamp(normal_lighting.x, 0.0, 1.0);
    float sun_term = mix(1.0, sun_ndotl, normal_hardness);
    if (mode == 1 && shadow.w > 0.5 && sun_term > 0.0) {
        vec3 Sdir = normalize(shadow_world_dir.xyz);
        if (length(Sdir) < 1e-5) Sdir = Ldir;
        float S_per = (-Sdir.x + Sdir.y) + shadow_dir.x;
        float T_per = ((-Sdir.x - Sdir.y) * 0.5 + Sdir.z * ISO_COS30)
                    + shadow_dir.y;
        float Sz    = Sdir.z * shadow_dir.z;
        float step_wu  = shadow.x;
        float soft_px  = shadow.y;
        int   maxSteps = int(shadow.z);
        const float kBiasWu = 2.0;
        vec2 sun_uv  = vec2(S_per, T_per);
        vec2 perp_uv = normalize(vec2(-sun_uv.y, sun_uv.x));
        float h = fract(sin(dot(v_uv, vec2(12.9898, 78.233))) * 43758.5453);
        const int kRays = 4;
        float hits = 0.0;
        for (int r = 0; r < kRays; ++r) {
            float rf = (float(r) + h) / float(kRays);
            float off_px = (rf - 0.5) * 2.0 * soft_px;
            vec2 perp_off = perp_uv * off_px / vec2(fbw, fbh);
            float rh = fract(h + float(r) * 0.6180339);
            bool hit = false;
            for (int i = 1; i <= maxSteps; ++i) {
                float t = (float(i) - 0.5 + rh) * step_wu;
                vec2 suv = v_uv + perp_off +
                           vec2(S_per * t / fbw, T_per * t / fbh);
                if (suv.x < 0.0 || suv.x > 1.0 ||
                    suv.y < 0.0 || suv.y > 1.0) break;
                vec4 albs = texture(albedo_tex, suv);
                if (albs.a < 0.01) continue;
                float drs = texture(depth_tex, suv).r;
                float szs = drs * vp.w + vp.z;
                float Tt  = suv.y * fbh - vp.y;
                float Ks  = recon.z - szs;
                float wzs = (Ks - 2.0 * Tt * ISO_COS30) / ISO_WZ_DENOM;
                float ray_wz = W.z + t * Sz;
                if (wzs > ray_wz + kBiasWu) { hit = true; break; }
            }
            if (hit) hits += 1.0;
        }
        float occ = hits / float(kRays);
        sun_shadow = 1.0 - occ * 0.8;
    }
    if (mode == 1) {
        light += light_col.rgb * light_dir.w * sun_term * sun_shadow;
    }
    if (vm == 6) { frag_color = vec4(vec3(sun_shadow), 1.0); return; }
    const float kMinPower = 0.0022436;
    const float kScale    = 50.0 / (1.0 - kMinPower);
    int nl = int(settings.y);
    for (int i = 0; i < KPL; ++i) {
        if (i >= nl) break;
        vec3  delta = plight_pos[i].xyz - W;
        float dist  = length(delta);
        float rad   = plight_pos[i].w;
        if (rad > 0.0 && dist < rad) {
            vec3  Lp    = delta / max(dist, 1e-5);
            float pterm = mix(1.0, max(dot(N, Lp), 0.0), normal_hardness);
            float normd = dist * 254.0 / rad;
            float pw    = pow(normd + 1.0, -1.1) - kMinPower;
            float attn  = clamp(pw * kScale, 0.0, 1.0);
            light += plight_col[i].rgb * plight_col[i].w * attn * pterm;
        }
    }
    frag_color = vec4(alb.rgb * light, 1.0);
}
)GLSL";
