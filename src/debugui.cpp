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

    ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_FirstUseEver);
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
    const float dt = float(TTime::DeltaTime());
    dt_ema = dt_ema == 0.0f ? dt : (dt_ema * 0.95f + dt * 0.05f);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("frame: %.2f ms (ema %.2f ms)  drawables=%zu lights=%zu",
                dt * 1000.0f, dt_ema * 1000.0f,
                s.sectorDrawInst.size(), s.sectorLights.size());
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
            std::vector<TObjectInstance*> character_list;
            character_list.reserve(64);
            for (TSector* sec : s.sectorsKept)
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

    if (s.sectorShowMeshLocators)
    {
        int32_t cam_ox = 0, cam_oy = 0;
        s.sectorCameraOriginScreen(cam_ox, cam_oy);
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
            S3DPoint root = inst.world_pos;
            S3DPoint px = root; px.x += len;
            S3DPoint py = root; py.y += len;
            S3DPoint pz = root; pz.z += len;

            S3DPoint sr, sx, sy, sz;
            if (inst.kind == ESectorDrawableKind::Mesh)
            {
                s.sectorProjectMeshWorld(root, sr);
                s.sectorProjectMeshWorld(px, sx);
                s.sectorProjectMeshWorld(py, sy);
                s.sectorProjectMeshWorld(pz, sz);
            }
            else
            {
                s.sectorProjectWorld(root, sr);
                s.sectorProjectWorld(px, sx);
                s.sectorProjectWorld(py, sy);
                s.sectorProjectWorld(pz, sz);
            }

            const ImVec2 a (float(sr.x + cam_ox), float(sr.y + cam_oy));
            const ImVec2 ax(float(sx.x + cam_ox), float(sx.y + cam_oy));
            const ImVec2 ay(float(sy.x + cam_ox), float(sy.y + cam_oy));
            const ImVec2 az(float(sz.x + cam_ox), float(sz.y + cam_oy));

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

    if (s.sectorShowTileLocators)
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

    if (s.sectorShowObjectLocators)
    {
        int32_t cam_ox = 0, cam_oy = 0;
        s.sectorCameraOriginScreen(cam_ox, cam_oy);
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        int shown = 0;

        for (TSector* sec : s.sectorsKept)
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
