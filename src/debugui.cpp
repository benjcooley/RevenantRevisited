// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    debugui.cpp - Debug UI overlay                     *
// *************************************************************************

#include "debugui.h"
#include "maprenderer_internal.h"

#include "assetcache.h"
#include "display.h"
#include "editor.h"
#include "imgui.h"
#include "playscreen.h"
#include "runtimemode.h"
#include "time.h"

#include <HandmadeMath.h>

#include <algorithm>
#include <cstdio>

namespace {
std::vector<IDebugTabContributor*> g_debugContributors;

static void DrawArrow2D(ImDrawList* dl, const ImVec2& a, const ImVec2& b,
                        ImU32 color, float thickness)
{
    dl->AddLine(a, b, color, thickness);
    const ImVec2 d = ImVec2(b.x - a.x, b.y - a.y);
    const float len2 = d.x * d.x + d.y * d.y;
    if (len2 < 1.0f) return;
    const float inv_len = 1.0f / std::sqrt(len2);
    const ImVec2 dir(d.x * inv_len, d.y * inv_len);
    const ImVec2 perp(-dir.y, dir.x);
    const float head = 8.0f, wing = 4.0f;
    const ImVec2 h0(b.x - dir.x * head + perp.x * wing, b.y - dir.y * head + perp.y * wing);
    const ImVec2 h1(b.x - dir.x * head - perp.x * wing, b.y - dir.y * head - perp.y * wing);
    dl->AddLine(b, h0, color, thickness);
    dl->AddLine(b, h1, color, thickness);
}
}

// *************************
// * DebugUI Module Hooks  *
// *************************

namespace DebugUI {

void RegisterContributor(IDebugTabContributor* contributor)
{
    if (!contributor) return;
    if ((std::find)(g_debugContributors.begin(), g_debugContributors.end(), contributor) == g_debugContributors.end())
        g_debugContributors.push_back(contributor);
}

void UnregisterContributor(IDebugTabContributor* contributor)
{
    g_debugContributors.erase((std::remove)(g_debugContributors.begin(), g_debugContributors.end(), contributor),
                              g_debugContributors.end());
}

// Hide the legacy debug panel by default. Toggle via DebugUI::SetVisible
// (menu hook lives in the new ImGui editor). Off-by-default so the
// shipping editor isn't cluttered with developer-tab UI.
static bool g_debugVisible = false;

bool IsVisible()              { return g_debugVisible; }
void SetVisible(bool on)      { g_debugVisible = on; }

void DrawFrame()
{
    if (!g_debugVisible) return;
    if (g_debugContributors.empty()) return;

    ImGui::SetNextWindowSize(ImVec2(460, 420), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Debug", &g_debugVisible))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("debug_tabs"))
    {
        for (IDebugTabContributor* contributor : g_debugContributors)
        {
            if (!contributor) continue;
            if (ImGui::BeginTabItem(contributor->GetDebugTabName()))
            {
                contributor->DrawDebugTab();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

}  // namespace DebugUI

// ************************************
// * TMapRenderer Debug Tab Rendering *
// ************************************

void TMapRenderer::DrawDebugTab()
{
    Impl& s = *impl;
    auto lightRadius = [](const SSectorLight& L) {
        TObjectInstance* oi = L.ref.Get();
        PSLightDef ld = oi ? oi->GetLightDef() : nullptr;
        return ld ? float(ld->intensity) : 0.0f;
    };
    auto lightColor = [](const SSectorLight& L, float rgb[3]) {
        TObjectInstance* oi = L.ref.Get();
        PSLightDef ld = oi ? oi->GetLightDef() : nullptr;
        if (ld) {
            rgb[0] = ld->color.red / 255.0f;
            rgb[1] = ld->color.green / 255.0f;
            rgb[2] = ld->color.blue / 255.0f;
        } else {
            rgb[0] = rgb[1] = rgb[2] = 0.0f;
        }
    };
    auto lightIntensity = [](const SSectorLight& L) {
        TObjectInstance* oi = L.ref.Get();
        PSLightDef ld = oi ? oi->GetLightDef() : nullptr;
        return (!ld || ld->multiplier <= 0) ? 0.0f : float(ld->multiplier) / 10.0f;
    };
    auto lightClassName = [](const SSectorLight& L) {
        TObjectInstance* oi = L.ref.Get();
        return oi ? oi->GetClassName() : nullptr;
    };

    static float dt_ema = 0.0f;
    const float dt = float(TTime::RealDeltaTime());
    const float sim_dt = float(TTime::DeltaTime());
    dt_ema = dt_ema == 0.0f ? dt : (dt_ema * 0.95f + dt * 0.05f);
    const ImGuiIO& io = ImGui::GetIO();

    uint64_t map_tile_refs = 0;
    uint64_t map_mesh_refs = 0;
    uint32_t map_tile_zero_refs = 0;
    uint32_t map_mesh_zero_refs = 0;
    for (const SSectorTileTex& tex : s.sectorTileTex)
    {
        map_tile_refs += tex.ref_count;
        if (tex.ref_count == 0)
            ++map_tile_zero_refs;
    }
    for (const SSectorMeshAsset& mesh : s.sectorMeshAsset)
    {
        map_mesh_refs += mesh.ref_count;
        if (mesh.ref_count == 0)
            ++map_mesh_zero_refs;
    }

    const SRendererAssetStats gpu_stats = Renderer ? Renderer->GetAssetStats() : SRendererAssetStats{};
    const SRendererTilePassStats tile_stats =
        Renderer ? Renderer->GetLastTilePassStats() : SRendererTilePassStats{};
    auto mb = [](uint64_t bytes) { return double(bytes) / (1024.0 * 1024.0); };
    const SMapFrameTimings& ft = s.last_frame_timings;

    // Persistent header: just frame pacing. Anything more detailed lives
    // in a tab. Keeps the top of the window readable.
    ImGui::Text("%.2f ms (ema %.2f)   sim %.2gx", dt * 1000.0f, dt_ema * 1000.0f, TTime::TimeScale());
    ImGui::Separator();

    if (ImGui::BeginTabBar("map_debug_tabs"))
    {
        // -------- Scene --------------------------------------------------
        // What to display + how the camera sees it. Per-frame stats moved
        // to the Perf tab.
        if (ImGui::BeginTabItem("Scene")) {
            ImGui::TextUnformatted("view:");
            ImGui::SameLine(); if (ImGui::RadioButton("lit",     s.view_mode == 0)) s.view_mode = 0;
            ImGui::SameLine(); if (ImGui::RadioButton("albedo",  s.view_mode == 1)) s.view_mode = 1;
            ImGui::SameLine(); if (ImGui::RadioButton("depth",   s.view_mode == 2)) s.view_mode = 2;
            ImGui::SameLine(); if (ImGui::RadioButton("z edges", s.view_mode == 8)) s.view_mode = 8;
            ImGui::SameLine(); if (ImGui::RadioButton("ground z", s.view_mode == 9)) s.view_mode = 9;
            ImGui::SameLine(); if (ImGui::RadioButton("normals", s.view_mode == 3)) s.view_mode = 3;
            ImGui::SameLine(); if (ImGui::RadioButton("points",  s.view_mode == 4)) s.view_mode = 4;
            ImGui::SameLine(); if (ImGui::RadioButton("shadow",  s.view_mode == 6)) s.view_mode = 6;
            ImGui::SameLine(); if (ImGui::RadioButton("ao",      s.view_mode == 7)) s.view_mode = 7;

            ImGui::TextUnformatted("lighting mode:");
            ImGui::SameLine(); if (ImGui::RadioButton("retail 1998", s.lighting_mode == 0)) s.lighting_mode = 0;
            ImGui::SameLine(); if (ImGui::RadioButton("modern",      s.lighting_mode == 1)) s.lighting_mode = 1;

            ImGui::Separator();
            ImGui::SliderFloat("zoom", &s.sectorCameraZoom, 0.25f, 4.0f, "%.2fx", ImGuiSliderFlags_Logarithmic);

            ImGui::Checkbox("perspective camera", &s.sectorPerspectiveCamera);
            ImGui::BeginDisabled(!s.sectorPerspectiveCamera);
            ImGui::SliderFloat("FOV", &s.sectorPerspectiveFovDeg, 2.0f, 20.0f, "%.1f deg");
            ImGui::TextUnformatted("perspective debug:");
            ImGui::SameLine(); if (ImGui::RadioButton("normal##perspdbg", s.sectorPerspectiveDebugMode == 0)) s.sectorPerspectiveDebugMode = 0;
            ImGui::SameLine(); if (ImGui::RadioButton("proxy fill##perspdbg", s.sectorPerspectiveDebugMode == 1)) s.sectorPerspectiveDebugMode = 1;
            ImGui::SameLine(); if (ImGui::RadioButton("hits##perspdbg", s.sectorPerspectiveDebugMode == 2)) s.sectorPerspectiveDebugMode = 2;
            ImGui::SameLine(); if (ImGui::RadioButton("proxy wire##perspdbg", s.sectorPerspectiveDebugMode == 3)) s.sectorPerspectiveDebugMode = 3;
            ImGui::TextUnformatted("projection:");
            ImGui::SameLine(); if (ImGui::RadioButton("flat##perspproj", s.sectorPerspectiveProjectionMode == 0)) s.sectorPerspectiveProjectionMode = 0;
            ImGui::SameLine(); if (ImGui::RadioButton("relief##perspproj", s.sectorPerspectiveProjectionMode == 2)) s.sectorPerspectiveProjectionMode = 2;
            ImGui::SameLine(); if (ImGui::RadioButton("volume ref##perspproj", s.sectorPerspectiveProjectionMode == 1)) s.sectorPerspectiveProjectionMode = 1;
            ImGui::SliderFloat("tile coverage scale", &s.sectorPerspectiveTileScale, 1.0f, 1.08f, "%.3f");
            ImGui::SliderFloat("proxy raster scale", &s.sectorPerspectiveProxyRasterScale, 0.25f, 1.25f, "%.3f");
            ImGui::SliderInt("height steps", &s.sectorPerspectiveSteps, 4, 128);
            ImGui::SliderInt("height refine", &s.sectorPerspectiveRefine, 0, 8);
            ImGui::SliderFloat("screen z offset", &s.sectorPerspectiveZOffset, -2048.0f, 2048.0f, "%.0f");
            ImGui::SliderFloat("screen z scale", &s.sectorPerspectiveZScale, 0.1f, 4.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            ImGui::EndDisabled();

            ImGui::Separator();
            const float focal = s.sectorCameraForward(HEIGHT);
            ImGui::Text("camera world=(%d,%d,%d)  forward=%.0f wu",
                        s.sectorCameraWorld.x, s.sectorCameraWorld.y, s.sectorCameraWorld.z, focal);
            ImGui::Text("mouse=(%.0f,%.0f) dragging=%d",
                        io.MousePos.x, io.MousePos.y, s.sectorDragging ? 1 : 0);
            ImGui::EndTabItem();
        }

        // -------- Perf ---------------------------------------------------
        // Everything pacing-related: render timings, draw/submit counts,
        // asset refcounts, simulation timescale.
        if (ImGui::BeginTabItem("Perf")) {
            if (ImGui::CollapsingHeader("Frame timing", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("total           %.2f ms", ft.total_ms);
                ImGui::Text("  sync/cache    %.2f ms", ft.sync_ms);
                ImGui::Text("  animate       %.2f ms", ft.animate_ms);
                ImGui::Text("  refresh recs  %.2f ms", ft.refresh_ms);
                ImGui::Text("  camera/setup  %.2f ms", ft.setup_ms);
                ImGui::Text("  depth fit     %.2f ms", ft.depth_fit_ms);
                ImGui::Text("  renderer st.  %.2f ms", ft.render_state_ms);
                ImGui::Text("  point lights  %.2f ms", ft.point_lights_ms);
                ImGui::Text("  begin pass    %.2f ms", ft.begin_pass_ms);
                ImGui::Text("  submit build  %.2f ms", ft.submit_ms);
                ImGui::Text("  end tile pass %.2f ms", ft.end_tile_pass_ms);
                ImGui::Text("  lighting pass %.2f ms", ft.lighting_pass_ms);
            }

            if (ImGui::CollapsingHeader("Draw counts", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("records=%d candidates=%d",
                            s.last_draw_counts.total_drawables,
                            s.last_draw_counts.draw_candidates);
                ImGui::Text("tiles submitted=%d visible=%d gbuf-border=%d",
                            s.last_draw_counts.tiles_submitted,
                            s.last_draw_counts.tiles_visible_submitted,
                            s.last_draw_counts.tiles_gbuffer_border_submitted);
                ImGui::Text("meshes submitted=%d culled=%d gbuf-pad=%d",
                            s.last_draw_counts.meshes_submitted,
                            s.last_draw_counts.offscreen_culled,
                            TRenderer::kGBufPad);
                ImGui::Text("tile raster: draws=%u proxy=%u hull=%u/%u rect-culled=%u",
                            tile_stats.tile_draws,
                            tile_stats.tile_proxy_draws,
                            tile_stats.tile_tight_proxy_draws,
                            tile_stats.tile_tight_proxy_points,
                            tile_stats.tile_rects_culled);
                ImGui::Text("tile pixels: clipped=%llu projected=%llu",
                            (unsigned long long)tile_stats.tile_clipped_pixels,
                            (unsigned long long)tile_stats.tile_projected_pixels);
                ImGui::Text("lights: considered=%d submitted=%d active=%d resident=%d",
                            s.last_draw_counts.point_lights_considered,
                            s.last_draw_counts.point_lights_submitted,
                            s.last_draw_counts.active_lights,
                            s.last_draw_counts.resident_lights);
                ImGui::Text("map: resident=%d range=(%d..%d, %d..%d)",
                            s.lastSyncedObjectSetCount,
                            s.drawRangeMinSx, s.drawRangeMaxSx,
                            s.drawRangeMinSy, s.drawRangeMaxSy);
            }

            if (ImGui::CollapsingHeader("Assets")) {
                ImGui::Text("source assets: %zu", AssetCache.AssetCount());
                ImGui::Text("map tile assets: %zu  refs=%llu zero=%u",
                            s.sectorTileTex.size(),
                            (unsigned long long)map_tile_refs,
                            map_tile_zero_refs);
                ImGui::Text("map mesh assets: %zu  refs=%llu zero=%u",
                            s.sectorMeshAsset.size(),
                            (unsigned long long)map_mesh_refs,
                            map_mesh_zero_refs);
                ImGui::Text("gpu image pairs: %u  refs=%llu zero=%u  %.1f MB",
                            gpu_stats.image_pair_count,
                            (unsigned long long)gpu_stats.image_pair_ref_total,
                            gpu_stats.image_pair_zero_ref_count,
                            mb(gpu_stats.image_pair_gpu_bytes));
                ImGui::Text("gpu textures:    %u  refs=%llu zero=%u  %.1f MB",
                            gpu_stats.texture_count,
                            (unsigned long long)gpu_stats.texture_ref_total,
                            gpu_stats.texture_zero_ref_count,
                            mb(gpu_stats.texture_gpu_bytes));
                ImGui::Text("gpu meshes:      %u  refs=%llu zero=%u  %.1f MB (v=%.1f i=%.1f)",
                            gpu_stats.mesh_count,
                            (unsigned long long)gpu_stats.mesh_ref_total,
                            gpu_stats.mesh_zero_ref_count,
                            mb(gpu_stats.mesh_gpu_bytes),
                            mb(gpu_stats.mesh_vertex_bytes),
                            mb(gpu_stats.mesh_index_bytes));
                ImGui::Text("renderer pools: buffers=%u/%d images=%u/%d",
                            gpu_stats.renderer_buffer_count,
                            gpu_stats.buffer_pool_size,
                            gpu_stats.renderer_image_count,
                            gpu_stats.image_pool_size);
                ImGui::Text("mesh cache: keyed=%u unkeyed=%u",
                            gpu_stats.keyed_mesh_count,
                            gpu_stats.unkeyed_mesh_count);
            }

            if (ImGui::CollapsingHeader("Simulation pacing")) {
                ImGui::Text("scale=%.3gx  dt=%.2f ms  legacy=%lld + %.3f",
                            TTime::TimeScale(),
                            sim_dt * 1000.0f,
                            (long long)TTime::LegacyFrameCount(),
                            TTime::LegacyFrameFraction());
                float time_scale = float(TTime::TimeScale());
                ImGui::SetNextItemWidth(180.0f);
                if (ImGui::SliderFloat("time scale", &time_scale, 0.0f, 4.0f, "%.3fx"))
                    TTime::SetTimeScale(time_scale);
                ImGui::TextUnformatted("presets:");
                ImGui::SameLine(); if (ImGui::RadioButton("1x##ts",   TTime::TimeScale() == 1.0))   TTime::SetTimeScale(1.0);
                ImGui::SameLine(); if (ImGui::RadioButton("1/2##ts",  TTime::TimeScale() == 0.5))   TTime::SetTimeScale(0.5);
                ImGui::SameLine(); if (ImGui::RadioButton("1/4##ts",  TTime::TimeScale() == 0.25))  TTime::SetTimeScale(0.25);
                ImGui::SameLine(); if (ImGui::RadioButton("1/8##ts",  TTime::TimeScale() == 0.125)) TTime::SetTimeScale(0.125);
                ImGui::SameLine(); if (ImGui::RadioButton("2x##ts",   TTime::TimeScale() == 2.0))   TTime::SetTimeScale(2.0);
                ImGui::SameLine(); if (ImGui::RadioButton("4x##ts",   TTime::TimeScale() == 4.0))   TTime::SetTimeScale(4.0);
            }
            ImGui::EndTabItem();
        }

        // -------- Lighting -----------------------------------------------
        // Everything tone/brightness/ambient + point-light controls.
        // Shadow / AO / normal-reconstruction params live on Effects.
        if (ImGui::BeginTabItem("Lighting")) {
            // ---- Sun orbit ----------------------------------------------
            // Sun rotates AROUND `rotation axis` on the plane perpendicular
            // to it. Noon = world up projected onto that plane; midnight =
            // opposite. Quick-pick buttons cover the conventional axes
            // (Y = east-rises / west-sets; X = north-south orbit).
            ImGui::SeparatorText("Sun orbit");
            ImGui::Checkbox("animate (sweep sun ignoring clock)", &s.animate);
            ImGui::SliderFloat3("rotation axis (xyz)", s.sun_rotation_axis, -1.0f, 1.0f, "%.2f");
            if (ImGui::Button("Y axis (E-W sun)"))  { s.sun_rotation_axis[0] = 0; s.sun_rotation_axis[1] = 1; s.sun_rotation_axis[2] = 0; }
            ImGui::SameLine();
            if (ImGui::Button("X axis (N-S sun)"))  { s.sun_rotation_axis[0] = 1; s.sun_rotation_axis[1] = 0; s.sun_rotation_axis[2] = 0; }
            ImGui::SameLine();
            if (ImGui::Button("diagonal"))           { s.sun_rotation_axis[0] = 0.7071f; s.sun_rotation_axis[1] = 0.7071f; s.sun_rotation_axis[2] = 0; }
            ImGui::Text("sun dir: (%+.2f, %+.2f, %+.2f)",
                        s.light_dir[0], s.light_dir[1], s.light_dir[2]);

            ImGui::Separator();
            ImGui::SliderFloat("intensity", &s.intensity, 0.0f, 4.0f);
            ImGui::ColorEdit3("sun color", s.color);

            // Ambient color + the live ambient term get overwritten each
            // frame from the active TArea (AMBLIGHT/AMBCOLOR via MapPane).
            // Show them read-only, and expose the tunable divisor that
            // controls the scaling from AMBLIGHT's arbitrary units to
            // the 0..1 shader value. See maprenderer.cpp ambient bridge.
            ImGui::BeginDisabled(true);
            ImGui::ColorEdit3("ambient color (from area.def)", s.ambient_color);
            ImGui::SliderFloat("ambient (live)", &s.ambient, 0.0f, 1.0f);
            ImGui::EndDisabled();
            ImGui::SliderFloat("ambient divisor", &s.ambient_divisor,
                               10.0f, 100.0f, "%.1f");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(
                    "ambient = AMBLIGHT * Ambient3D / (divisor * 100)\n"
                    "Lower = brighter ambient. AMBLIGHT is authored in\n"
                    "arbitrary units. At divisor=100 with AMBLIGHT=30\n"
                    "and Ambient3D=100, s.ambient = 0.30. Drop the\n"
                    "divisor to push ambient overbright (paired with\n"
                    "light_ceiling > 1.0 below).");

            ImGui::SliderFloat("light ceiling", &s.light_ceiling,
                               0.25f, 8.0f, "%.2f",
                               ImGuiSliderFlags_Logarithmic);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(
                    "Per-channel clamp on summed light (ambient +\n"
                    "point lights) before albedo multiply. Caps\n"
                    "burn-out from stacked point lights. > 1.0 allows\n"
                    "overbright; the linear-space GPU path needs this\n"
                    "to match retail's non-linear palette boost.");

            // ---- Time of day --------------------------------------------
            // Drives the day/night ambient blend in TArea::Pulse. The
            // current minute, daylight scalar (0..255 sun curve), and
            // coarse DayTimeFlag bucket are all derived from
            // PlayScreen.gametime; SetGameTime jumps that clock forward
            // or backward, after which the next TArea::Pulse picks up
            // the new value and FadeAmbient interpolates accordingly.
            ImGui::Separator();
            const int32_t day_minutes = 24 * 60;
            int32_t tod_min = PlayScreen.TimeOfDay() % day_minutes;
            if (tod_min < 0) tod_min += day_minutes;
            const int32_t hh = tod_min / 60;
            const int32_t mm = tod_min % 60;
            const int32_t daylight = PlayScreen.Daylight();
            static const char* kDayNames[6] = {
                "Midnight", "Morning", "Daytime", "Noon", "Evening", "Night"
            };
            const int32_t df = PlayScreen.DayTimeFlag();
            const char* day_name = (df >= 0 && df < 6) ? kDayNames[df] : "?";

            ImGui::Text("time: %02d:%02d  daylight=%d/255  flag=%s",
                        hh, mm, daylight, day_name);

            int set_min = tod_min;
            if (ImGui::SliderInt("time-of-day (minutes)", &set_min,
                                 0, day_minutes - 1, "%d min"))
            {
                // gametime is centi-seconds; 1 minute = 6000.
                PlayScreen.SetGameTime(set_min * 6000);
            }

            auto set_minute = [](int32_t m) {
                PlayScreen.SetGameTime(m * 6000);
            };
            if (ImGui::Button("Midnight (00:00)")) set_minute(0);
            ImGui::SameLine();
            if (ImGui::Button("Sunrise (06:00)")) set_minute(6 * 60);
            ImGui::SameLine();
            if (ImGui::Button("Noon (12:00)"))    set_minute(12 * 60);
            ImGui::SameLine();
            if (ImGui::Button("Sunset (19:00)"))  set_minute(19 * 60);
            ImGui::SameLine();
            if (ImGui::Button("Night (22:00)"))   set_minute(22 * 60);

            // ---- Point lights -------------------------------------------
            // Per-instance lights placed by level designers. Multipliers
            // affect all point lights uniformly; per-light list lets you
            // toggle individual ones for debugging.
            ImGui::Separator();
            ImGui::Text("point lights: active=%zu resident=%d submitted=%d",
                        s.sectorLights.size(),
                        s.residentPointLightCount,
                        s.last_draw_counts.point_lights_submitted);
            ImGui::Checkbox("lights enabled",    &s.lights_on);
            ImGui::SliderFloat("radius x",       &s.radius_mul,    0.1f, 8.0f,   "%.2f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("intensity x",    &s.intensity_mul, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            if (ImGui::TreeNode("per-light list")) {
                for (size_t i = 0; i < s.sectorLights.size(); ++i) {
                    SSectorLight& L = s.sectorLights[i];
                    float rgb[3]; lightColor(L, rgb);
                    const char* cn = lightClassName(L);
                    ImGui::PushID(int(i));
                    ImGui::Checkbox("##on", &L.enabled);
                    ImGui::SameLine();
                    ImGui::Text("%zu %s  r=%.0f  (%.2f,%.2f,%.2f) int=%.2f",
                                i, cn ? cn : "?", lightRadius(L),
                                rgb[0], rgb[1], rgb[2], lightIntensity(L));
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }

        // -------- Effects ------------------------------------------------
        // Modern-mode-only post stages: sun shadows, screen-space AO,
        // normal reconstruction. All gated on lighting_mode==1 because
        // the retail 1998 path doesn't use them.
        if (ImGui::BeginTabItem("Effects")) {
            ImGui::BeginDisabled(s.lighting_mode == 0);
            if (s.lighting_mode == 0)
                ImGui::TextDisabled("(disabled in retail-1998 lighting mode)");

            if (ImGui::CollapsingHeader("Sun shadows", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("sun shadows", &s.sun_shadow);
                ImGui::SliderFloat("step wu",  &s.sun_shadow_step, 1.0f, 256.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
                ImGui::SliderInt("max steps",  &s.sun_shadow_max, 4, 512);
                ImGui::SliderFloat("soft edge radius px", &s.sun_shadow_soft, 0.0f, 48.0f, "%.1f");
                ImGui::SliderFloat("depth cutoff wu", &s.sun_shadow_depth_cutoff, 1.0f, 128.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
                ImGui::SliderFloat("bias wu",  &s.sun_shadow_bias, -10.0f, 10.0f, "%.2f");
                ImGui::SliderFloat("wz mul",   &s.sdir_wz_mul, 0.05f, 8.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
                ImGui::Text("reach %.0f wu", s.sun_shadow_step * float(s.sun_shadow_max));
                ImGui::Text("approx full-res ray reads/pixel: %.1f", 0.25f * float(s.sun_shadow_max));
                if (ImGui::Button("soft default")) {
                    s.sun_shadow_step = 32.0f; s.sun_shadow_soft = 3.0f;
                    s.sun_shadow_max = 64; s.sun_shadow_samples = 1;
                    s.sun_shadow_depth_cutoff = 16.0f; s.sun_shadow_bias = 2.0f;
                    s.sdir_wz_mul = 1.0f;
                }
                ImGui::SameLine();
                if (ImGui::Button("hard baseline")) {
                    s.sun_shadow_step = 16.0f; s.sun_shadow_soft = 0.0f;
                    s.sun_shadow_max = 128; s.sun_shadow_samples = 1;
                    s.sun_shadow_depth_cutoff = 16.0f; s.sun_shadow_bias = 2.0f;
                    s.sdir_wz_mul = 1.0f;
                }
                ImGui::SameLine();
                if (ImGui::Button("slow diagnostic")) {
                    s.sun_shadow_step = 8.0f; s.sun_shadow_soft = 2.0f;
                    s.sun_shadow_max = 512; s.sun_shadow_samples = 1;
                    s.sun_shadow_depth_cutoff = 16.0f; s.sun_shadow_bias = 2.0f;
                    s.sdir_wz_mul = 1.0f;
                }
            }

            if (ImGui::CollapsingHeader("Ambient occlusion")) {
                ImGui::Checkbox("ambient occlusion", &s.ao_enable);
                ImGui::SliderFloat("ao radius px", &s.ao_radius_px, 1.0f, 48.0f);
                ImGui::SliderFloat("ao strength",  &s.ao_strength,  0.0f, 12.0f);
                ImGui::SliderFloat("ao bias",      &s.ao_bias,      0.0f, 0.5f);
                ImGui::SliderFloat("ao max wu",    &s.ao_max_dist,  8.0f, 320.0f, "%.0f");
            }

            if (ImGui::CollapsingHeader("Normal reconstruction")) {
                ImGui::SliderFloat("depth_mul",     &s.depth_mul,       0.0f, 128.0f);
                ImGui::SliderFloat("hardness",      &s.normal_hardness, 0.0f, 1.0f);
                ImGui::SliderFloat("normal_radius", &s.normal_radius,   0.5f, 8.0f);
                ImGui::SliderFloat("edge_thr",      &s.edge_thr,        1.0f, 2048.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
            }
            ImGui::EndDisabled();
            ImGui::EndTabItem();
        }

        // -------- Map ----------------------------------------------------
        // Sector / character / locator visibility, world-space probes,
        // z-range clipping. Used during level inspection.
        if (ImGui::BeginTabItem("Map")) {
            std::vector<TObjectInstance*> character_list;
            character_list.reserve(64);
            TGameMap* dbg_map = s.currentMap.Get();
            if (dbg_map)
            for (TSector* sec : dbg_map->Sectors())
            {
                if (!sec)
                    continue;
                for (int32_t i = 0; i < sec->NumItems(); ++i)
                {
                    TObjectInstance* oi = sec->GetInstance(i);
                    if (!oi || !oi->IsCharacter())
                        continue;
                    if ((std::find)(character_list.begin(), character_list.end(), oi) == character_list.end())
                        character_list.push_back(oi);
                }
            }
            if (s.sectorCharacterFocusIdx >= int32_t(character_list.size()))
                s.sectorCharacterFocusIdx = -1;
            if (ImGui::Button("center on next character"))
            {
                if (!character_list.empty())
                {
                    s.sectorCharacterFocusIdx = (s.sectorCharacterFocusIdx + 1) % int32_t(character_list.size());
                    if (TObjectInstance* focus = character_list[s.sectorCharacterFocusIdx])
                    {
                        const S3DPoint p = focus->Pos();
                        s.sectorCameraWorld.x = p.x;
                        s.sectorCameraWorld.y = p.y;
                        s.sectorCameraWorld.z = 0;
                        s.sectorDragging = false;
                    }
                }
            }
            ImGui::SameLine();
            if (!character_list.empty() && s.sectorCharacterFocusIdx >= 0)
            {
                TObjectInstance* focus = character_list[s.sectorCharacterFocusIdx];
                bool has_drawable = false;
                for (const auto& inst : s.sectorDrawInst)
                {
                    if (inst.src.Get() == focus)
                    {
                        has_drawable = true;
                        break;
                    }
                }
                ImGui::Text("focus: %s (%d/%zu) drawable=%s",
                    focus && focus->GetTypeName() ? focus->GetTypeName() : "character",
                    s.sectorCharacterFocusIdx + 1, character_list.size(),
                    has_drawable ? "yes" : "no");
            }
            else
            {
                ImGui::Text("focus: none (%zu chars)", character_list.size());
            }
            ImGui::Separator();
            ImGui::Checkbox("show light vector", &s.show_light_vector);
            ImGui::Checkbox("show normal under mouse", &s.show_mouse_normal);
            ImGui::SliderFloat("arrow len", &s.debug_arrow_len_wu, 16.0f, 320.0f, "%.0f wu");
            ImGui::Separator();
            const float span = s.debugSceneZMinFit - s.debugSceneZMaxFit;
            const float margin = std::fabs(span) > 0.0f ? std::fabs(span) : 2500.0f;
            const float slo = fminf(-1024.0f, s.debugSceneZMinFit - margin);
            const float shi = fmaxf(16384.0f, s.debugSceneZMaxFit + margin);
            if (ImGui::Button("reset z-range")) { s.z_near = -1024.0f; s.z_far = 16384.0f; }
            ImGui::SameLine();
            ImGui::Text("clip span=%.0f wu  fit tiles=%d", s.z_far - s.z_near, s.debugFitTiles);
            ImGui::SliderFloat("z_near", &s.z_near, slo, shi);
            ImGui::SliderFloat("z_far",  &s.z_far,  slo, shi);
            ImGui::Checkbox("show tiles", &s.sectorShowTiles);
            ImGui::Checkbox("show meshes", &s.sectorShowMeshes);
            ImGui::Checkbox("show mesh locators", &s.sectorShowMeshLocators);
            ImGui::Checkbox("force mesh preview pose", &s.sectorForceMeshPreviewPose);
            ImGui::SliderFloat("mesh scale x", &s.sectorMeshScaleX, 0.1f, 4.0f);
            ImGui::SliderFloat("mesh scale y", &s.sectorMeshScaleY, 0.1f, 4.0f);
            ImGui::SliderFloat("mesh scale z", &s.sectorMeshScaleZ, 0.1f, 4.0f);
            ImGui::Checkbox("show tile bboxes", &s.sectorShowTileBboxes);
            ImGui::Checkbox("show tile locators", &s.sectorShowTileLocators);
            if (s.sectorShowTileLocators)
            {
                ImGui::SameLine();
                ImGui::Checkbox("labels", &s.sectorShowTileLabels);
            }
            ImGui::Checkbox("show all object locators", &s.sectorShowObjectLocators);
            if (s.sectorShowObjectLocators)
            {
                ImGui::SameLine();
                ImGui::Checkbox("object labels", &s.sectorShowObjectLabels);
            }
            ImGui::Checkbox("show gizmos",      &s.sectorShowGizmos);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if (s.sectorShowMeshLocators && EditorOverlaysEnabled())
    {
        TMapRenderer* mr = PlayScreen.MapRenderer();

        // Ask the renderer for the single world->pixel matrix for the
        // rect where the rendered scene actually lands. Editor mode:
        // the lit_target ImGui::Image inside the Game View panel. Game
        // mode (fallback): the whole swapchain. The matrix bakes in
        // the iso camera + ortho + viewport in one go -- gizmos just
        // do a 4x4 multiply, no piecewise math.
        SEditorRect dst{};
        if (EditorOverlaysEnabled()) {
            dst = EditorGameViewRect();
        } else {
            dst = { 0, 0, int32_t(io.DisplaySize.x), int32_t(io.DisplaySize.y) };
        }
        float mat[16];
        if (mr) mr->GetWorldToPixel(dst.x, dst.y, dst.w, dst.h, mat);
        else    { for (int i = 0; i < 16; ++i) mat[i] = (i % 5 == 0) ? 1.0f : 0.0f; }
        const hmm_mat4& M = *reinterpret_cast<const hmm_mat4*>(mat);
        auto Project = [&](const S3DPoint& w) {
            hmm_vec4 p = HMM_MultiplyMat4ByVec4(
                M, HMM_Vec4(float(w.x), float(w.y), float(w.z), 1.0f));
            return ImVec2(p.X, p.Y);
        };

        ImDrawList* dl = ImGui::GetForegroundDrawList();
        int shown = 0;
        for (const auto& inst : s.sectorDrawInst)
        {
            if (inst.kind != ESectorDrawableKind::Mesh)
                continue;
            TObjectInstance* oi = inst.src.Get();
            if (!oi) continue;
            if (!s.sectorShowGizmos && (oi->IsLight() || oi->ObjClass() == OBJCLASS_HELPER))
                continue;

            const int32_t len = int32_t(s.debug_arrow_len_wu);
            const S3DPoint root = inst.world_pos;
            const ImVec2 a  = Project(root);
            const ImVec2 ax = Project({ root.x + len, root.y,       root.z       });
            const ImVec2 ay = Project({ root.x,       root.y + len, root.z       });
            const ImVec2 az = Project({ root.x,       root.y,       root.z + len });

            if ((a.x < -64 || a.x > io.DisplaySize.x + 64 ||
                 a.y < -64 || a.y > io.DisplaySize.y + 64) && shown >= 32)
                continue;

            const bool is_character = oi->IsCharacter();
            const ImU32 label_color = is_character
                ? IM_COL32(255, 230, 96, 255)
                : IM_COL32(255, 255, 255, 255);
            const float root_radius = is_character ? 5.0f : 3.0f;
            dl->AddCircleFilled(a, root_radius, label_color, 12);
            DrawArrow2D(dl, a, ax, IM_COL32(255,64,64,255), 2.0f);
            DrawArrow2D(dl, a, ay, IM_COL32(64,255,64,255), 2.0f);
            DrawArrow2D(dl, a, az, IM_COL32(64,128,255,255), 2.0f);

            const char* label = nullptr;
            if (is_character)
            {
                label = oi->GetTypeName();
                if ((!label || !*label) && oi->GetName() && *oi->GetName())
                    label = oi->GetName();
            }
            else
            {
                label = oi->GetClassName();
            }
            if (label && *label)
                dl->AddText(ImVec2(a.x + 8.0f, a.y - 7.0f), label_color, label);
            {
                char posbuf[128];
                const S3DPoint p = oi->Pos();
                if (inst.kind == ESectorDrawableKind::Mesh)
                {
                    const float render_z = FIX_Z_VALUE(p.z);
                    std::snprintf(posbuf, sizeof(posbuf), "(%d,%d,%d) rz=%.2f", p.x, p.y, p.z, render_z);
                }
                else
                {
                    std::snprintf(posbuf, sizeof(posbuf), "(%d,%d,%d)", p.x, p.y, p.z);
                }
                dl->AddText(ImVec2(a.x + 8.0f, a.y + 7.0f), label_color, posbuf);
            }

            ++shown;
            if (shown >= 64) break;
        }
        ImGui::Text("mesh locators shown: %d", shown);
    }

    if (s.sectorShowTileLocators && EditorOverlaysEnabled())
    {
        int32_t cam_ox = 0, cam_oy = 0;
        s.sectorCameraOriginScreen(cam_ox, cam_oy);
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        int shown = 0;
        for (size_t i = 0; i < s.sectorDrawInst.size(); ++i)
        {
            const auto& inst = s.sectorDrawInst[i];
            if (inst.kind != ESectorDrawableKind::Tile)
                continue;
            if ((uint32_t)inst.asset_idx >= (uint32_t)s.sectorTileTex.size())
                continue;
            TObjectInstance* oi = inst.src.Get();
            if (!oi) continue;
            if (!s.sectorShowGizmos && (oi->IsLight() || oi->ObjClass() == OBJCLASS_HELPER))
                continue;

            const SSectorTileTex& tex = s.sectorTileTex[size_t(inst.asset_idx)];
            S3DPoint sp;
            s.sectorProjectWorld(inst.world_pos, sp);
            const int32_t dx = sp.x - inst.regx + cam_ox;
            const int32_t dy = sp.y - inst.regy + cam_oy;
            const int32_t x1 = dx + tex.w;
            const int32_t y1 = dy + tex.h;
            if (x1 <= 0 || y1 <= 0 || dx >= io.DisplaySize.x || dy >= io.DisplaySize.y)
                continue;

            const ImVec2 top_left{float(dx), float(dy)};
            const ImVec2 bottom_right{float(x1), float(y1)};
            dl->AddRect(top_left, bottom_right, IM_COL32(64, 224, 255, 160), 0.0f, 0, 1.0f);

            const int32_t len = int32_t(s.debug_arrow_len_wu);
            S3DPoint root = inst.world_pos;
            S3DPoint px = root; px.x += len;
            S3DPoint py = root; py.y += len;
            S3DPoint pz = root; pz.z += len;
            S3DPoint sr, sx, sy, sz;
            s.sectorProjectWorld(root, sr);
            s.sectorProjectWorld(px, sx);
            s.sectorProjectWorld(py, sy);
            s.sectorProjectWorld(pz, sz);

            const ImVec2 a (float(sr.x + cam_ox), float(sr.y + cam_oy));
            const ImVec2 ax(float(sx.x + cam_ox), float(sx.y + cam_oy));
            const ImVec2 ay(float(sy.x + cam_ox), float(sy.y + cam_oy));
            const ImVec2 az(float(sz.x + cam_ox), float(sz.y + cam_oy));
            dl->AddCircleFilled(a, 3.0f, IM_COL32(64, 224, 255, 255), 12);
            DrawArrow2D(dl, a, ax, IM_COL32(255,64,64,255), 1.5f);
            DrawArrow2D(dl, a, ay, IM_COL32(64,255,64,255), 1.5f);
            DrawArrow2D(dl, a, az, IM_COL32(64,128,255,255), 1.5f);

            if (s.sectorShowTileLabels)
            {
                const int32_t secx = MapRendererFloorDiv(inst.world_pos.x, SECTORWIDTH);
                const int32_t secy = MapRendererFloorDiv(inst.world_pos.y, SECTORHEIGHT);
                char label[256];
                std::snprintf(label, sizeof(label), "#%zu %s:%s sec=%d_%d pos=(%d,%d,%d)",
                              i,
                              oi->GetClassName() ? oi->GetClassName() : "?",
                              oi->GetTypeName() ? oi->GetTypeName() : "?",
                              secx, secy,
                              inst.world_pos.x, inst.world_pos.y, inst.world_pos.z);
                dl->AddText(ImVec2(a.x + 7.0f, a.y - 7.0f), IM_COL32(64, 224, 255, 255), label);
            }

            ++shown;
            if (shown >= 512) break;
        }
        ImGui::Text("tile locators shown: %d", shown);
    }

    if (s.sectorShowObjectLocators && EditorOverlaysEnabled())
    {
        int32_t cam_ox = 0, cam_oy = 0;
        s.sectorCameraOriginScreen(cam_ox, cam_oy);
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        int shown = 0;

        TGameMap* dbg_map2 = s.currentMap.Get();
        if (dbg_map2)
        for (TSector* sec : dbg_map2->Sectors())
        {
            if (!sec) continue;
            const int32_t secx = sec->SectorX();
            const int32_t secy = sec->SectorY();
            for (int32_t slot = 0; slot < sec->NumItems(); ++slot)
            {
                TObjectInstance* oi = sec->GetInstance(slot);
                if (!oi) continue;
                if (!s.sectorShowGizmos && (oi->IsLight() || oi->ObjClass() == OBJCLASS_HELPER))
                    continue;

                const S3DPoint root = oi->Pos();
                S3DPoint sr;
                s.sectorProjectWorld(root, sr);
                const ImVec2 a(float(sr.x + cam_ox), float(sr.y + cam_oy));
                if (a.x < -128.0f || a.x > io.DisplaySize.x + 128.0f ||
                    a.y < -128.0f || a.y > io.DisplaySize.y + 128.0f)
                    continue;

                const bool is_tile = oi->ObjClass() == OBJCLASS_TILE;
                const bool is_helper = oi->ObjClass() == OBJCLASS_HELPER;
                const bool is_effect = oi->ObjClass() == OBJCLASS_EFFECT;
                const bool is_trace_target =
                    sec->SectorLevel() == 0 && secx == 7 && secy == 22 &&
                    (slot == 34 || slot == 36 || slot == 37 || slot == 44 || slot == 122);
                int drawable_idx = -1;
                for (size_t di = 0; di < s.sectorDrawInst.size(); ++di)
                {
                    const auto& drawable = s.sectorDrawInst[di];
                    if (drawable.src.Get() == oi)
                    {
                        drawable_idx = int(di);
                        break;
                    }
                }
                ImU32 color = IM_COL32(255, 255, 255, 255);
                if (is_tile) color = IM_COL32(80, 255, 255, 255);
                else if (is_effect) color = IM_COL32(255, 128, 255, 255);
                else if (is_helper) color = IM_COL32(255, 210, 64, 255);
                if (is_trace_target) color = IM_COL32(255, 0, 255, 255);

                dl->AddCircleFilled(a, is_trace_target ? 7.0f : (is_tile ? 3.0f : 4.0f), color, 12);
                if (is_trace_target)
                    dl->AddCircle(a, 13.0f, IM_COL32(255, 255, 255, 255), 16, 2.0f);

                if (is_trace_target && drawable_idx >= 0)
                {
                    const SSectorDrawableInst& drawable = s.sectorDrawInst[size_t(drawable_idx)];
                    if (drawable.kind == ESectorDrawableKind::Tile &&
                        (uint32_t)drawable.asset_idx < (uint32_t)s.sectorTileTex.size())
                    {
                        const SSectorTileTex& tex = s.sectorTileTex[size_t(drawable.asset_idx)];
                        S3DPoint dsp;
                        s.sectorProjectWorld(drawable.world_pos, dsp);
                        const int32_t dx = dsp.x - drawable.regx + cam_ox;
                        const int32_t dy = dsp.y - drawable.regy + cam_oy;
                        const ImVec2 r0{float(dx), float(dy)};
                        const ImVec2 r1{float(dx + tex.w), float(dy + tex.h)};
                        dl->AddRect(r0, r1, IM_COL32(255, 0, 255, 255), 0.0f, 0, 3.0f);
                        const ImVec2 rc((r0.x + r1.x) * 0.5f, (r0.y + r1.y) * 0.5f);
                        dl->AddLine(a, rc, IM_COL32(255, 255, 255, 220), 2.0f);
                        const S3DPoint draw_pos = drawable.world_pos;
                        const int32_t draw_sec_x = MapRendererFloorDiv(draw_pos.x, SECTORWIDTH);
                        const int32_t draw_sec_y = MapRendererFloorDiv(draw_pos.y, SECTORHEIGHT);
                        const int32_t raw_lx = root.x - secx * SECTORWIDTH;
                        const int32_t raw_ly = root.y - secy * SECTORHEIGHT;
                        const int32_t draw_lx = draw_pos.x - draw_sec_x * SECTORWIDTH;
                        const int32_t draw_ly = draw_pos.y - draw_sec_y * SECTORHEIGHT;
                        const bool pos_mismatch = root.x != draw_pos.x || root.y != draw_pos.y || root.z != draw_pos.z;
                        char rlabel[256];
                        std::snprintf(rlabel, sizeof(rlabel),
                                      "draw#%d asset=%d%s\nrect=(%d,%d)-(%d,%d)\nraw %d_%d L(%d,%d) Z%d\ndraw %d_%d L(%d,%d) Z%d\nreg=(%d,%d,%d)",
                                      drawable_idx, drawable.asset_idx,
                                      pos_mismatch ? " POS MISMATCH" : "",
                                      dx, dy, dx + tex.w, dy + tex.h,
                                      secx, secy, raw_lx, raw_ly, root.z,
                                      draw_sec_x, draw_sec_y, draw_lx, draw_ly, draw_pos.z,
                                      drawable.regx, drawable.regy, drawable.regz);
                        const ImU32 trace_text_color = pos_mismatch ? IM_COL32(255, 255, 0, 255)
                                                                    : IM_COL32(255, 0, 255, 255);
                        dl->AddText(ImVec2(a.x + 16.0f, a.y + 12.0f), trace_text_color, rlabel);
                        dl->AddText(ImVec2(r0.x + 4.0f, r0.y + 4.0f), trace_text_color, rlabel);
                    }
                }

                const int32_t len = int32_t(s.debug_arrow_len_wu);
                S3DPoint px = root; px.x += len;
                S3DPoint py = root; py.y += len;
                S3DPoint pz = root; pz.z += len;
                S3DPoint sxp, syp, szp;
                s.sectorProjectWorld(px, sxp);
                s.sectorProjectWorld(py, syp);
                s.sectorProjectWorld(pz, szp);
                DrawArrow2D(dl, a, ImVec2(float(sxp.x + cam_ox), float(sxp.y + cam_oy)), IM_COL32(255,64,64,255), 1.25f);
                DrawArrow2D(dl, a, ImVec2(float(syp.x + cam_ox), float(syp.y + cam_oy)), IM_COL32(64,255,64,255), 1.25f);
                DrawArrow2D(dl, a, ImVec2(float(szp.x + cam_ox), float(szp.y + cam_oy)), IM_COL32(64,128,255,255), 1.25f);

                if (s.sectorShowObjectLabels)
                {
                    char label[256];
                    std::snprintf(label, sizeof(label), "%d_%d[%03d] %s\n%s:%s\nL(%d,%d) Z%d",
                                  secx, secy, slot,
                                  drawable_idx >= 0 ? "D" : "noD",
                                  oi->GetClassName() ? oi->GetClassName() : "?",
                                  oi->GetTypeName() ? oi->GetTypeName() : "?",
                                  root.x - secx * SECTORWIDTH,
                                  root.y - secy * SECTORHEIGHT,
                                  root.z);
                    dl->AddText(ImVec2(a.x + 6.0f, a.y - 18.0f), color, label);
                }

                ++shown;
                if (shown >= 1024) break;
            }
            if (shown >= 1024) break;
        }

        ImGui::Text("object locators shown: %d", shown);
    }
}
