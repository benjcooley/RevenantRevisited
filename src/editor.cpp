// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               editor.cpp - ImGui-based in-game editor                 *
// *************************************************************************
//
// Layout: ImGui docking (vendored docking branch). On a "fresh" launch
// (or whenever the user-saved layout doesn't include our windows yet)
// we build a default split:
//
//   +-----------------------------------------------------+ <- menu bar
//   | Scene |       Game View              | Properties   |
//   |       |  (central dock node;         |              |
//   |       |   renderer composites the    |              |
//   |       |   game image at engine       |              |
//   |       |   resolution, centered)      |              |
//   |-------+------------------------------+--------------+
//   |       Console / Palette                             |
//   +-----------------------------------------------------+ <- status bar
//
// All panels are dockable + resizable + collapsible. Layout (which
// panel is where, splitter positions, etc.) persists via ImGui's own
// .ini system; game-specific state piggybacks on EditorPrefs (which
// writes editor.def).
//
// *************************************************************************

#include "editor.h"

#include "debugui.h"
#include "editorlog.h"
#include "editorplatform.h"
#include "editorprefs.h"
#include "editorstub.h"
#include "logging.h"
#include "maprenderer.h"
#include "playscreen.h"
#include "renderer.h"
#include "revtypes.h"

#include <algorithm>
#include <cstdio>
#include <imgui.h>
#include <imgui_internal.h>
#include <sokol_app.h>

namespace {

constexpr int32_t kGameViewW = 640;
constexpr int32_t kGameViewH = 480;
constexpr const char* kDockSpaceName = "##editor_dockspace";
constexpr const char* kPrefBuiltLayout    = "layout.imgui.built";
constexpr const char* kPrefEditorWindowW  = "window.editor.w";
constexpr const char* kPrefEditorWindowH  = "window.editor.h";
constexpr const char* kPrefGameWindowW    = "window.game.w";
constexpr const char* kPrefGameWindowH    = "window.game.h";

constexpr int32_t kDefaultEditorWindowW = 1280;
constexpr int32_t kDefaultEditorWindowH = 800;

bool g_first_chrome_frame = true;

void PushPresentPixelRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    if (!Renderer) return;
    const int32_t sw = sapp_width();
    const int32_t sh = sapp_height();
    if (sw <= 0 || sh <= 0 || w <= 0 || h <= 0) {
        Renderer->ResetPresentNDCRect();
        return;
    }
    const float ndc_x = -1.0f + 2.0f * float(x) / float(sw);
    const float ndc_w =          2.0f * float(w) / float(sw);
    const float ndc_y =  1.0f - 2.0f * float(y + h) / float(sh);
    const float ndc_h =          2.0f * float(h) / float(sh);
    Renderer->SetPresentNDCRect(ndc_x, ndc_y, ndc_w, ndc_h);
}

// Build the default panel arrangement: Scene left, Properties right,
// Console bottom, Game View central. Called only when the user has no
// saved layout yet (delete editor.def + remove imgui.ini -> fresh).
void BuildDefaultDockLayout(ImGuiID root_id)
{
    ImGui::DockBuilderRemoveNodeChildNodes(root_id);
    ImGui::DockBuilderSetNodeSize(root_id, ImGui::GetMainViewport()->Size);

    ImGuiID center = root_id;
    ImGuiID left   = ImGui::DockBuilderSplitNode(center, ImGuiDir_Left,  0.20f, nullptr, &center);
    ImGuiID right  = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, nullptr, &center);
    ImGuiID bottom = ImGui::DockBuilderSplitNode(center, ImGuiDir_Down,  0.30f, nullptr, &center);

    ImGui::DockBuilderDockWindow("Scene",       left);
    ImGui::DockBuilderDockWindow("Properties",  right);
    // Bottom node hosts Console / Palette / Log as sibling tabs.
    ImGui::DockBuilderDockWindow("Console",     bottom);
    ImGui::DockBuilderDockWindow("Palette",     bottom);
    ImGui::DockBuilderDockWindow("Log",         bottom);
    ImGui::DockBuilderDockWindow("Game View",   center);

    ImGui::DockBuilderFinish(root_id);
}

void DrawMenuBar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Editor Layout")) EditorSaveState();
            ImGui::MenuItem("Save Map", nullptr, false, false);
            ImGui::Separator();
            ImGui::MenuItem("Exit Editor (F12)", nullptr, false, false);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo",      "Ctrl+Z", false, false);
            ImGui::MenuItem("Redo",      "Ctrl+Y", false, false);
            ImGui::Separator();
            ImGui::MenuItem("Cut",       "Ctrl+X", false, false);
            ImGui::MenuItem("Copy",      "Ctrl+C", false, false);
            ImGui::MenuItem("Paste",     "Ctrl+V", false, false);
            ImGui::MenuItem("Duplicate", "Ctrl+D", false, false);
            ImGui::MenuItem("Delete",    "Del",    false, false);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            bool dbg = DebugUI::IsVisible();
            if (ImGui::MenuItem("Debug Panel", nullptr, &dbg))
                DebugUI::SetVisible(dbg);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Map"))   { ImGui::EndMenu(); }
        if (ImGui::BeginMenu("Tools")) { ImGui::EndMenu(); }
        if (ImGui::BeginMenu("Help"))  { ImGui::EndMenu(); }
        ImGui::EndMainMenuBar();
    }
}

void DrawStatusBar()
{
    // Bottom-of-viewport status strip. The docking branch supports
    // BeginViewportSideBar; we use it so the status bar is always
    // anchored regardless of dock layout.
    const float h = ImGui::GetFrameHeight();
    if (ImGui::BeginViewportSideBar("##editor_status_bar",
                                    ImGui::GetMainViewport(),
                                    ImGuiDir_Down, h,
                                    ImGuiWindowFlags_NoSavedSettings |
                                    ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar()) {
            // Pull whatever the map renderer thinks is "current" right now.
            // PlayScreen owns the live TMapRenderer; ask it.
            const TMapRenderer* mr = PlayScreen.MapRenderer();
            int32_t lvl = -1, sx = -1, sy = -1;
            int32_t wx = 0, wy = 0, wz = 0;
            if (mr) mr->GetCameraStatus(lvl, sx, sy, wx, wy, wz);

            const ImGuiIO& io = ImGui::GetIO();
            char buf[200];
            std::snprintf(buf, sizeof(buf),
                          "Level: %d   Sector: %d_%d   World: (%d, %d, %d)   "
                          "%.0f fps  (%.2f ms)   ImGui windows: %d",
                          lvl, sx, sy, wx, wy, wz,
                          io.Framerate, 1000.0f / (io.Framerate <= 0 ? 1.0f : io.Framerate),
                          ImGui::GetCurrentContext()->Windows.Size);
            ImGui::TextUnformatted(buf);
            ImGui::EndMenuBar();
        }
    }
    ImGui::End();
}

void DrawScenePanel()
{
    if (ImGui::Begin("Scene")) {
        ImGui::TextDisabled("(scene tree placeholder)");
        ImGui::TextDisabled("hierarchical IObject view");
    }
    ImGui::End();
}

void DrawPropertiesPanel()
{
    if (ImGui::Begin("Properties")) {
        ImGui::TextDisabled("(property editor placeholder)");
        ImGui::TextDisabled("IReflection-driven inspector");
    }
    ImGui::End();
}

void DrawConsolePanel()
{
    if (ImGui::Begin("Console")) {
        ImGui::TextDisabled("(console placeholder)");
        ImGui::TextDisabled("type editor commands here;");
        ImGui::TextDisabled("output streams above");
    }
    ImGui::End();
}

void DrawPalettePanel()
{
    if (ImGui::Begin("Palette")) {
        ImGui::TextDisabled("(palette placeholder)");
        ImGui::TextDisabled("draggable tiles + monsters");
    }
    ImGui::End();
}

void DrawGameViewPanel()
{
    // The game image is the renderer's lit_target sokol image. We let
    // ImGui composite it as a regular textured quad inside the panel,
    // so docking / overlap / collapse work like any other window.
    // (Previously we tried to blit it onto the swapchain at a sub-rect,
    // but the panel's opaque background drew on top.)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    const bool open = ImGui::Begin("Game View");
    if (open && Renderer) {
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 cur   = ImGui::GetCursorScreenPos();

        // Centered 640x480 (engine resolution) inside the panel.
        const float gw = float(kGameViewW);
        const float gh = float(kGameViewH);
        const float pad_x = (avail.x > gw) ? (avail.x - gw) * 0.5f : 0.0f;
        const float pad_y = (avail.y > gh) ? (avail.y - gh) * 0.5f : 0.0f;
        const ImVec2 img_min(cur.x + pad_x, cur.y + pad_y);
        const ImVec2 img_max(img_min.x + gw, img_min.y + gh);

        // The lit_target is a (width + 2*pad) x (height + 2*pad) RGBA8
        // image; only the centered display-sized sub-rect is the real
        // game view. Compute uv bounds for that sub-rect.
        sg_image lit = Renderer->LitTarget();
        if (lit.id) {
            const int32_t pad = Renderer->GBufPad();
            const float fbw = float(Renderer->Width()  + 2 * pad);
            const float fbh = float(Renderer->Height() + 2 * pad);
            const ImVec2 uv0(float(pad) / fbw, float(pad) / fbh);
            const ImVec2 uv1((float(pad) + float(Renderer->Width())) / fbw,
                             (float(pad) + float(Renderer->Height())) / fbh);
            ImGui::SetCursorScreenPos(img_min);
            ImGui::Image((ImTextureID)(uintptr_t)lit.id,
                         ImVec2(gw, gh), uv0, uv1);
        } else {
            ImGui::Dummy(ImVec2(avail.x, avail.y));
        }

        // The renderer's swap-chain composite isn't used in editor mode --
        // ImGui's pass blits the lit_target into the panel directly.
        Renderer->ResetPresentNDCRect();
    } else {
        if (Renderer) Renderer->ResetPresentNDCRect();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace

// *************************************************************************
// * State + lifecycle                                                     *
// *************************************************************************

void EditorLoadState() {
    EditorPrefs::Load();
    EditorLogInstall();
}
void EditorSaveState() { EditorPrefs::Save(); }

void EditorEnter()
{
    g_first_chrome_frame = true;
    if (Renderer) Renderer->SuppressPresent(true);

    // Save the current (game-only) window size so we can restore it on
    // exit. Read once -- subsequent F12 cycles will preserve whatever
    // user has resized to.
    int32_t cur_w = 0, cur_h = 0;
    EditorPlatform::GetWindowSize(cur_w, cur_h);
    if (cur_w > 0 && cur_h > 0
        && !EditorPrefs::Has(kPrefGameWindowW))
    {
        EditorPrefs::SetInt(kPrefGameWindowW, cur_w);
        EditorPrefs::SetInt(kPrefGameWindowH, cur_h);
    }

    const int32_t ew = EditorPrefs::GetInt(kPrefEditorWindowW, kDefaultEditorWindowW);
    const int32_t eh = EditorPrefs::GetInt(kPrefEditorWindowH, kDefaultEditorWindowH);
    EditorPlatform::SetWindowSizeCenteredOnCurrent(ew, eh);

    log_info("[editor] enter -- ImGui docking layout, window %dx%d", ew, eh);
}

void EditorExit()
{
    if (Renderer) {
        Renderer->ResetPresentNDCRect();
        Renderer->SuppressPresent(false);
    }

    // Save the user's current editor window size so reopening lands at
    // the same dimensions.
    int32_t cur_w = 0, cur_h = 0;
    EditorPlatform::GetWindowSize(cur_w, cur_h);
    if (cur_w > 0 && cur_h > 0) {
        EditorPrefs::SetInt(kPrefEditorWindowW, cur_w);
        EditorPrefs::SetInt(kPrefEditorWindowH, cur_h);
    }
    EditorSaveState();

    // Shrink back to the saved game-only size.
    const int32_t gw = EditorPrefs::GetInt(kPrefGameWindowW, kGameViewW);
    const int32_t gh = EditorPrefs::GetInt(kPrefGameWindowH, kGameViewH);
    EditorPlatform::SetWindowSizeCenteredOnCurrent(gw, gh);

    log_info("[editor] exit -- present rect reset, prefs saved, window %dx%d", gw, gh);
}

SEditorRect EditorGameViewRect()
{
    if (!Editor) return { 0, 0, sapp_width(), sapp_height() };
    // Approximate: the central dock node's rect. ImGui internals expose
    // it but most callers just need a sane non-zero rect; use the full
    // window for now.
    return { 0, 0, sapp_width(), sapp_height() };
}

// *************************************************************************
// * Per-frame UI                                                          *
// *************************************************************************

void EditorDrawChrome()
{
    if (!Editor) return;

    // Make sure docking is enabled. simgui_setup doesn't toggle this for
    // us; we own the flag here so it switches on only when the editor is
    // visible (and stays on across frames once flipped -- ImGui dock
    // state lives in the shared context).
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Diagnostic: log per-toggle viewport vs sapp size to see if the
    // resize propagates.
    static bool s_logged_after_enter = false;
    if (g_first_chrome_frame) s_logged_after_enter = false;
    if (!s_logged_after_enter) {
        const ImGuiViewport* vp = ImGui::GetMainViewport();
        log_info("[editor diag] sapp=%dx%d  io.DisplaySize=%.0fx%.0f  vp=%.0fx%.0f  dpi=%.2f",
                 sapp_width(), sapp_height(),
                 ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y,
                 vp->Size.x, vp->Size.y, sapp_dpi_scale());
        s_logged_after_enter = true;
    }

    DrawMenuBar();
    DrawStatusBar();

    // Full-viewport dockspace under the menu and above the status bar.
    // Pass-through so the game image (drawn before us via the renderer)
    // shows through the Game View central node when its window padding
    // is zero.
    const ImGuiID dock_id =
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID,
                                     ImGui::GetMainViewport(),
                                     ImGuiDockNodeFlags_PassthruCentralNode);

    // On every EditorEnter, force-rebuild the default layout into the
    // current viewport. ImGui's persisted .ini contains pixel positions
    // baked when the window may have been a different size, which leaves
    // panels pinned in a sub-rect of the new (larger) window. Rebuilding
    // pins them to a sensible split for the *current* viewport. The
    // user can still drag splitters during the session; that scales with
    // the live dockspace as they go.
    if (g_first_chrome_frame) {
        BuildDefaultDockLayout(dock_id);
        EditorPrefs::SetBool(kPrefBuiltLayout, true);
        EditorPrefs::Save();
    }
    g_first_chrome_frame = false;

    DrawScenePanel();
    DrawPropertiesPanel();
    DrawConsolePanel();
    DrawPalettePanel();
    if (ImGui::Begin("Log")) EditorLogDrawPanel();
    ImGui::End();
    DrawGameViewPanel();
}
