// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    debugui.cpp - Debug UI overlay                     *
// *************************************************************************

#include "debugui.h"
#include "maprenderer_internal.h"

#include "imgui.h"
#include "time.h"

#include <algorithm>

namespace {
std::vector<IDebugTabContributor*> g_debugContributors;
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

void DrawFrame()
{
    if (g_debugContributors.empty())
        return;

    ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Debug"))
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
    const float dt = float(TTime::DeltaTime());
    dt_ema = dt_ema == 0.0f ? dt : (dt_ema * 0.95f + dt * 0.05f);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("frame: %.2f ms (ema %.2f ms)  tiles=%zu lights=%zu",
                dt * 1000.0f, dt_ema * 1000.0f,
                s.sectorTileInst.size(), s.sectorLights.size());
    ImGui::Text("mouse=(%.0f,%.0f) dragging=%d cam=(%d,%d,%d)",
                io.MousePos.x, io.MousePos.y,
                s.sectorDragging ? 1 : 0,
                s.sectorCameraWorld.x, s.sectorCameraWorld.y, s.sectorCameraWorld.z);
    ImGui::Separator();

    if (ImGui::BeginTabBar("map_debug_tabs"))
    {
        if (ImGui::BeginTabItem("View")) {
            ImGui::TextUnformatted("view:");
            ImGui::SameLine(); if (ImGui::RadioButton("lit",     s.view_mode == 0)) s.view_mode = 0;
            ImGui::SameLine(); if (ImGui::RadioButton("albedo",  s.view_mode == 1)) s.view_mode = 1;
            ImGui::SameLine(); if (ImGui::RadioButton("depth",   s.view_mode == 2)) s.view_mode = 2;
            ImGui::SameLine(); if (ImGui::RadioButton("normals", s.view_mode == 3)) s.view_mode = 3;
            ImGui::SameLine(); if (ImGui::RadioButton("points",  s.view_mode == 4)) s.view_mode = 4;
            ImGui::SameLine(); if (ImGui::RadioButton("shadow",  s.view_mode == 6)) s.view_mode = 6;
            ImGui::SameLine(); if (ImGui::RadioButton("ao",      s.view_mode == 7)) s.view_mode = 7;
            ImGui::Separator();
            ImGui::TextUnformatted("mode:");
            ImGui::SameLine(); if (ImGui::RadioButton("retail 1998", s.lighting_mode == 0)) s.lighting_mode = 0;
            ImGui::SameLine(); if (ImGui::RadioButton("modern",      s.lighting_mode == 1)) s.lighting_mode = 1;
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Light")) {
            ImGui::Checkbox("animate (sweep sun across sky)", &s.animate);
            const float side = 140.0f;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            const ImVec2 p0 = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("sun_puck", ImVec2(side, side));
            const bool active = ImGui::IsItemActive();
            const ImVec2 center = ImVec2(p0.x + side * 0.5f, p0.y + side * 0.5f);
            const float radius = side * 0.5f - 4.0f;
            if (active) {
                const ImVec2 m = ImGui::GetIO().MousePos;
                float u = (m.x - center.x) / radius;
                float v = (center.y - m.y) / radius;
                const float r2 = u*u + v*v;
                if (r2 > 1.0f) { const float k = 1.0f / std::sqrt(r2); u *= k; v *= k; }
                s.puck_u = u; s.puck_v = v;
            }
            dl->AddCircleFilled(center, radius, IM_COL32(30,32,38,255), 48);
            dl->AddCircle(center, radius, IM_COL32(120,120,130,255), 48, 1.5f);
            dl->AddLine(ImVec2(center.x - radius, center.y), ImVec2(center.x + radius, center.y), IM_COL32(70,70,80,255));
            dl->AddLine(ImVec2(center.x, center.y - radius), ImVec2(center.x, center.y + radius), IM_COL32(70,70,80,255));
            const ImVec2 dot = ImVec2(center.x + s.puck_u * radius, center.y - s.puck_v * radius);
            dl->AddCircleFilled(dot, 5.0f, IM_COL32(255,220,80,255), 16);
            dl->AddCircle(dot, 5.0f, IM_COL32(40,30,0,255), 16, 1.5f);
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("sun puck");
            ImGui::Text("u=%.2f  v=%.2f", s.puck_u, s.puck_v);
            ImGui::Text("Puck dir=(%.2f, %.2f, %.2f)", s.dir[0], s.dir[1], s.dir[2]);
            ImGui::EndGroup();
            ImGui::SliderFloat("intensity", &s.intensity, 0.0f, 4.0f);
            ImGui::ColorEdit3("sun color", s.color);
            ImGui::ColorEdit3("ambient color", s.ambient_color);
            ImGui::SliderFloat("ambient", &s.ambient, 0.0f, 10.0f);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Shadows")) {
            ImGui::BeginDisabled(s.lighting_mode == 0);
            ImGui::Checkbox("sun shadows", &s.sun_shadow);
            ImGui::SliderFloat("shadow step", &s.sun_shadow_step, 2.0f, 128.0f);
            ImGui::SliderFloat("shadow soft", &s.sun_shadow_soft, 0.0f, 16.0f);
            ImGui::SliderInt("shadow steps", &s.sun_shadow_max, 4, 128);
            ImGui::EndDisabled();
            ImGui::Separator();
            ImGui::SliderFloat("shadow ox", &s.sdir_off_x, -1.5f, 1.5f);
            ImGui::SliderFloat("shadow oy", &s.sdir_off_y, -1.5f, 1.5f);
            ImGui::SliderFloat("shadow wz mul", &s.sdir_wz_mul, 0.1f, 4.0f);
            if (ImGui::Button("reset shadow variance")) {
                s.sdir_off_x = 0.0f;
                s.sdir_off_y = 0.0f;
                s.sdir_wz_mul = 1.0f;
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("AO")) {
            ImGui::Checkbox("ambient occlusion", &s.ao_enable);
            ImGui::SliderFloat("ao radius px", &s.ao_radius_px, 1.0f, 48.0f);
            ImGui::SliderFloat("ao strength",  &s.ao_strength,  0.0f, 12.0f);
            ImGui::SliderFloat("ao bias",      &s.ao_bias,      0.0f, 0.5f);
            ImGui::SliderFloat("ao max wu",    &s.ao_max_dist,  8.0f, 320.0f, "%.0f");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Normals")) {
            ImGui::SliderFloat("depth_mul",     &s.depth_mul,     0.0f, 128.0f);
            ImGui::SliderFloat("hardness",      &s.normal_hardness, 0.0f, 1.0f);
            ImGui::SliderFloat("normal_radius", &s.normal_radius, 0.5f, 8.0f);
            ImGui::SliderFloat("edge_thr",      &s.edge_thr,      1.0f, 2048.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Debug")) {
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
            ImGui::Checkbox("show tile bboxes", &s.sectorShowTileBboxes);
            ImGui::Checkbox("show gizmos",      &s.sectorShowGizmos);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Point Lights")) {
            ImGui::Text("point lights (%zu from sector)", s.sectorLights.size());
            ImGui::Checkbox("lights enabled",    &s.lights_on);
            ImGui::SliderFloat("radius x",       &s.radius_mul,    0.1f, 8.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("intensity x",    &s.intensity_mul, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            if (ImGui::TreeNode("per-light")) {
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

        ImGui::EndTabBar();
    }
}
