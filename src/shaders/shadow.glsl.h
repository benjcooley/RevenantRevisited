// *************************************************************************
// *                         Cinematix Revenant                            *
// *        shadow.glsl.h - GLSL sun shadow mask + blur passes             *
// *************************************************************************

#pragma once

inline constexpr const char* kShadowVsGlsl = R"GLSL(
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;
out vec2 v_uv;
void main() {
    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);
    v_uv = uv;
}
)GLSL";

inline constexpr const char* kShadowFsGlsl = R"GLSL(
#version 330 core
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
layout(std140) uniform params {
    vec4 vp;
    vec4 recon;
    vec4 camera;
    vec4 shadow;
    vec4 shadow_dir;
    vec4 shadow_world_dir;
    vec4 normal_lighting;
};
in vec2 v_uv;
uniform sampler2D albedo_tex;
uniform sampler2D depth_tex;
layout(location = 0) out float out_shadow;
vec3 reconstruct_world(vec2 uv, float d, float fbw, float fbh) {
    float scene_z = d * vp.w + vp.z;
    float zoom = max(camera.x, 0.0001);
    float S = (uv.x * fbw - vp.x) / zoom;
    float T = (uv.y * fbh - vp.y) / zoom;
    if (recon.w > 0.5) {
        float focal = max(recon.z, 1.0);
        S = (S / focal) * scene_z;
        T = (T / focal) * scene_z;
    }
    float K = recon.z - scene_z;
    float wz = (K - 2.0 * T * ISO_COS30) / ISO_WZ_DENOM;
    float sum_r = 2.0 * (T + wz * ISO_COS30);
    return vec3(recon.x + (sum_r + S) * 0.5,
                recon.y + (sum_r - S) * 0.5,
                wz);
}
vec2 project_world_uv(vec3 W, float fbw, float fbh) {
    float dx = W.x - recon.x;
    float dy = W.y - recon.y;
    float S = dx - dy;
    float T = 0.5 * (dx + dy) - W.z * ISO_COS30;
    float scale = max(camera.x, 0.0001);
    if (recon.w > 0.5) {
        float scene_z = recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
        scale *= recon.z / max(scene_z, 1.0);
    }
    return vec2((S * scale + vp.x) / fbw, (T * scale + vp.y) / fbh);
}
float scene_depth_world(vec3 W) {
    float dx = W.x - recon.x;
    float dy = W.y - recon.y;
    return recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
}
float interpolate_ray_depth(float z0, float z1, float a) {
    if (recon.w > 0.5) {
        float inv_z = mix(1.0 / max(z0, 1.0), 1.0 / max(z1, 1.0), a);
        return 1.0 / max(inv_z, 1e-6);
    }
    return mix(z0, z1, a);
}
float sample_scene_depth(vec2 uv) {
    ivec2 ts = textureSize(depth_tex, 0);
    ivec2 p = clamp(ivec2(uv * vec2(ts)), ivec2(0), ts - ivec2(1));
    return texelFetch(depth_tex, p, 0).r;
}
void main() {
    vec4 alb = texture(albedo_tex, v_uv);
    if (alb.a < 0.01 || shadow.w <= 0.5) { out_shadow = 1.0; return; }
    float d = sample_scene_depth(v_uv);
    if (d >= 0.9999) { out_shadow = 1.0; return; }

    vec2 ts = vec2(textureSize(albedo_tex, 0));
    float fbw = ts.x, fbh = ts.y;
    vec3 W = reconstruct_world(v_uv, d, fbw, fbh);
    vec3 Sdir = shadow_world_dir.xyz;
    if (length(Sdir) < 1e-5) { out_shadow = 1.0; return; }
    Sdir.z *= shadow_dir.z;
    vec3 Rdir = normalize(Sdir);

    float step_wu = shadow.x;
    int maxSteps = int(shadow.z);
    float reach_wu = step_wu * float(maxSteps);
    vec3 rayEndW = W + Rdir * reach_wu;
    vec2 rayEndUv = project_world_uv(rayEndW, fbw, fbh);
    vec2 ray_uv = rayEndUv - v_uv;
    if (length(ray_uv) <= 1e-6 || reach_wu <= 0.0) { out_shadow = 1.0; return; }

    float ray_z0_wu = scene_depth_world(W);
    float ray_z1_wu = scene_depth_world(rayEndW);
    float depth_cutoff_wu = max(shadow_dir.y, 1.0);
    float bias_wu = normal_lighting.y;

    for (int i = 1; i <= 512; ++i) {
        if (i > maxSteps) break;
        float a = (float(i) - 0.5) / float(maxSteps);
        vec2 suv = mix(v_uv, rayEndUv, a);
        if (suv.x < 0.0 || suv.x > 1.0 || suv.y < 0.0 || suv.y > 1.0) break;
        float drs = sample_scene_depth(suv);
        if (drs >= 0.9999) continue;
        float scene_z_wu = drs * vp.w + vp.z;
        float ray_z_wu = interpolate_ray_depth(ray_z0_wu, ray_z1_wu, a);
        float depth_delta_wu = scene_z_wu - ray_z_wu;
        if (depth_delta_wu >= -depth_cutoff_wu && depth_delta_wu <= bias_wu) {
            out_shadow = 0.0;
            return;
        }
    }
    out_shadow = 1.0;
}
)GLSL";

inline constexpr const char* kShadowBlurFsGlsl = R"GLSL(
#version 330 core
layout(std140) uniform params {
    vec4 blur;
};
in vec2 v_uv;
uniform sampler2D shadow_tex;
layout(location = 0) out float out_shadow;
void main() {
    vec2 ts = vec2(textureSize(shadow_tex, 0));
    vec2 dir = blur.xy / max(ts, vec2(1.0));
    float radius = max(blur.z, 0.0);
    float c = 0.38774 * texture(shadow_tex, v_uv).r;
    c += 0.24477 * texture(shadow_tex, v_uv + dir * radius).r;
    c += 0.24477 * texture(shadow_tex, v_uv - dir * radius).r;
    c += 0.06136 * texture(shadow_tex, v_uv + dir * radius * 2.0).r;
    c += 0.06136 * texture(shadow_tex, v_uv - dir * radius * 2.0).r;
    out_shadow = clamp(c, 0.0, 1.0);
}
)GLSL";
