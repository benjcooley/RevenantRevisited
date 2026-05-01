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
#include "editoradapters.h"
#include "editorcommands.h"
#include "editorfonts.h"
#include "editoricons.h"
#include "editorlog.h"
#include "editorserialize.h"
#include "editorundo.h"
#include "renderer_readback.h"
#include "editorplatform.h"
#include "editorprefs.h"
#include "editorstub.h"
#include "iobject.h"
#include "logging.h"
#include "mappane.h"
#include "maprenderer.h"
#include "player.h"
#include "object.h"
#include "playscreen.h"
#include "renderer.h"
#include "revdefs.h"
#include "revtypes.h"
#include "sector.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <unordered_map>
#include <IconsMaterialSymbols.h>
// Revenant's legacy revdefs.h defines `COLOR` as a numeric macro, which
// collides with ImGuizmo's `enum COLOR { ... }`. Drop the legacy def --
// the only remaining uses are inside string literals where macros don't
// expand.
#undef COLOR
#include <ImGuizmo.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_toggle.h>
#include <imgui_toggle_presets.h>
#include <sokol_app.h>

namespace {

constexpr int32_t kGameViewW = 640;
constexpr int32_t kGameViewH = 480;
constexpr const char* kDockSpaceName = "##editor_dockspace";
constexpr const char* kPrefBuiltLayout    = "layout.imgui.built";
constexpr const char* kPrefSceneShowEmpty = "scene.show_empty_sectors";
constexpr const char* kPrefEditorWindowW  = "window.editor.w";
constexpr const char* kPrefEditorWindowH  = "window.editor.h";
constexpr const char* kPrefGameWindowW    = "window.game.w";
constexpr const char* kPrefGameWindowH    = "window.game.h";

constexpr int32_t kDefaultEditorWindowW = 1280;
constexpr int32_t kDefaultEditorWindowH = 800;

bool g_first_chrome_frame = true;
bool g_commands_registered = false;

// Play / pause for game-state advancement. Editor opens paused, but the
// world is still rendered every frame so animated widgets / overlays
// don't freeze. Pause means "don't advance gameplay state", not "stop
// rendering".
bool g_running = false;

// Whether shadows were on before we entered editor mode. We turn them
// off in editor mode by default and restore on exit.
bool g_shadows_were_on = true;

// Game-View toolbar tool. Drives ImGuizmo's operation once wired; for
// now just a UI state toggled by the toolbar buttons / hotkeys.
enum class EEditorTool : uint8_t { Select, Pan, Move, Rotate, Scale, _Count };
EEditorTool g_tool = EEditorTool::Select;

// Per-tool definition table. Anything that varies between tools (icon,
// tooltip, gizmo op, whether a left click in the game view should
// resolve to a pixel-pick selection, ...) lives here so call sites
// don't have to switch on the enum.
struct SEditorToolDef
{
    const char*          icon;
    const char*          tooltip;
    ImGuizmo::OPERATION  gizmo_op;
    bool                 can_click_select;
};

constexpr SEditorToolDef kEditorTools[size_t(EEditorTool::_Count)] = {
    /* Select */ { ICON_MS_ARROW_SELECTOR_TOOL, "Select (Q)",
                   ImGuizmo::TRANSLATE, true  },
    /* Pan    */ { ICON_MS_PAN_TOOL,            "Pan (H, or Alt+drag in any tool)",
                   ImGuizmo::TRANSLATE, false },
    /* Move   */ { ICON_MS_OPEN_WITH,           "Move (W)",
                   ImGuizmo::TRANSLATE, true  },
    /* Rotate */ { ICON_MS_3D_ROTATION,         "Rotate (E)",
                   ImGuizmo::ROTATE,    true  },
    /* Scale  */ { ICON_MS_TRANSFORM,           "Scale (R)",
                   ImGuizmo::SCALE,     true  },
};

inline const SEditorToolDef& ToolDef(EEditorTool t)
{
    return kEditorTools[size_t(t)];
}

bool g_panning = false;       // active drag-to-pan in the game view

// Editor selection. Mapindex-keyed (the universal id every TSafeRef<>
// resolves through). Only TObjectInstance-derived objects are
// selectable; sectors are tree headers, not selectable items.
std::vector<int32_t> g_selection;

// One-shot reveal request. Set when the selection primary changes from
// a non-tree source (pixel pick, Frame command). The tree expands
// ancestors + scrolls to the row, then clears it.
bool    g_selection_reveal      = false;
int32_t g_last_revealed_primary = -1;

// Anchor for shift-range select: the last "primary" pick. Range is
// anchor..clicked in the order rows appear in the tree.
int32_t g_selection_anchor = -1;

// Flat record of which instance rows were drawn this frame, in display
// order. Rebuilt each frame; used by shift-range selection to walk
// from anchor to clicked row.
std::vector<int32_t> g_tree_order_this_frame;

TObjectInstance* SelectionPrimary()
{
    return g_selection.empty() ? nullptr
                               : safe_cast<TObjectInstance>(g_selection.back());
}
// IObject* view of the primary selection, for code paths that still drive
// off IReflection (property panel, schema-based UI). Falls through the
// adapter cache so the underlying TObjectInstance is wrapped as needed.
IObject* SelectionPrimaryAsObject()
{
    TObjectInstance* oi = SelectionPrimary();
    return oi ? EditorAdapters().For(oi) : nullptr;
}
bool IsSelected(int32_t mapindex)
{
    for (int32_t s : g_selection) if (s == mapindex) return true;
    return false;
}

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
    ImGui::DockBuilderDockWindow("Debug",       right);   // tab next to Properties
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
            EditorCommands::DrawMenuItems("File");
            ImGui::Separator();
            ImGui::MenuItem("Exit Editor (F12)", nullptr, false, false);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            // Pick "Cmd" / "Ctrl" based on the platform binding ImGui
            // is currently using -- matches what users actually press.
            const char* mod = ImGui::GetIO().ConfigMacOSXBehaviors ? "Cmd" : "Ctrl";
            char label[96];
            char sc[64];

            // Undo / Redo with the operation label appended -- "Undo move"
            // / "Redo scale" reading. The registered Edit.Undo/Redo
            // commands still drive the hotkeys; this just renders their
            // menu rows manually so we can use a dynamic label.
            std::snprintf(sc, sizeof(sc), "%s+Z", mod);
            if (EditorUndo::CanUndo() && *EditorUndo::TopUndoName())
                std::snprintf(label, sizeof(label), "Undo %s", EditorUndo::TopUndoName());
            else
                std::snprintf(label, sizeof(label), "Undo");
            if (ImGui::MenuItem(label, sc, false, EditorUndo::CanUndo()))
                EditorUndo::Undo();
            std::snprintf(sc, sizeof(sc), "%s+Shift+Z", mod);
            if (EditorUndo::CanRedo() && *EditorUndo::TopRedoName())
                std::snprintf(label, sizeof(label), "Redo %s", EditorUndo::TopRedoName());
            else
                std::snprintf(label, sizeof(label), "Redo");
            if (ImGui::MenuItem(label, sc, false, EditorUndo::CanRedo()))
                EditorUndo::Redo();
            ImGui::Separator();
            // Edit ops registered through the command registry. Render
            // them by id so we can format the shortcut text with the
            // platform-correct primary modifier (Cmd / Ctrl).
            auto DrawEditCmd = [&](const char* id, const char* sc_text) {
                const SEditorCommand* c = EditorCommands::Find(id);
                if (!c) return;
                const bool enabled = !c->is_enabled || c->is_enabled();
                if (ImGui::MenuItem(c->label.c_str(),
                                    sc_text && *sc_text ? sc_text : nullptr,
                                    false, enabled) && c->execute)
                    c->execute();
            };
            std::snprintf(sc, sizeof(sc), "%s+X", mod); DrawEditCmd("Edit.Cut",       sc);
            std::snprintf(sc, sizeof(sc), "%s+C", mod); DrawEditCmd("Edit.Copy",      sc);
            std::snprintf(sc, sizeof(sc), "%s+V", mod); DrawEditCmd("Edit.Paste",     sc);
            std::snprintf(sc, sizeof(sc), "%s+D", mod); DrawEditCmd("Edit.Duplicate", sc);
            DrawEditCmd("Edit.Delete", "Del");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            EditorCommands::DrawMenuItems("View");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            bool dbg = DebugUI::IsVisible();
            if (ImGui::MenuItem("Debug", nullptr, &dbg))
                DebugUI::SetVisible(dbg);
            EditorCommands::DrawMenuItems("Window");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Map"))   { EditorCommands::DrawMenuItems("Map");   ImGui::EndMenu(); }
        if (ImGui::BeginMenu("Tools")) { EditorCommands::DrawMenuItems("Tools"); ImGui::EndMenu(); }
        if (ImGui::BeginMenu("Help"))  { EditorCommands::DrawMenuItems("Help");  ImGui::EndMenu(); }
        ImGui::EndMainMenuBar();
    }
}

// Honest FPS measurement. ImGui's io.Framerate is averaged off
// io.DeltaTime, which sokol filters to "hide spikes" -- meaning the
// reported number stays at 60 even when we're rendering at 20-30. Use
// our own wall-clock counter instead.
struct SFpsCounter {
    using clock = std::chrono::steady_clock;
    clock::time_point window_start = clock::now();
    int32_t           frames_in_window = 0;
    float             fps_smoothed = 0.0f;
    float             ms_smoothed  = 0.0f;
    clock::time_point last_frame   = clock::now();

    void Tick()
    {
        const auto now = clock::now();
        const float dt_ms = std::chrono::duration<float, std::milli>(now - last_frame).count();
        last_frame = now;
        // Per-frame ms exponentially smoothed -- shows real frame time even
        // when fps averages over a wider window.
        ms_smoothed = (ms_smoothed == 0.0f) ? dt_ms : (ms_smoothed * 0.9f + dt_ms * 0.1f);

        ++frames_in_window;
        const float window_ms = std::chrono::duration<float, std::milli>(now - window_start).count();
        if (window_ms >= 500.0f) {
            fps_smoothed   = float(frames_in_window) * 1000.0f / window_ms;
            window_start   = now;
            frames_in_window = 0;
        }
    }
};

SFpsCounter g_fps;

void DrawStatusBar()
{
    g_fps.Tick();

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

            TMapRenderer::SDrawCounts dc{};
            if (mr) dc = mr->GetLastDrawCounts();

            char buf[300];
            std::snprintf(buf, sizeof(buf),
                          "Level: %d   Sector: %d_%d   World: (%d, %d, %d)   "
                          "%.0f fps  (%.2f ms)   "
                          "draw: tiles=%d meshes=%d (of %d, %d off-screen)",
                          lvl, sx, sy, wx, wy, wz,
                          g_fps.fps_smoothed, g_fps.ms_smoothed,
                          dc.tiles_submitted, dc.meshes_submitted,
                          dc.total_drawables, dc.offscreen_culled);
            ImGui::TextUnformatted(buf);
            ImGui::EndMenuBar();
        }
    }
    ImGui::End();
}

// ImGuizmo plumbing -----------------------------------------------------------

// Build a column-major translate-only model matrix at world point W.
void ModelMatrixFromTranslation(const S3DPoint& W, float m[16])
{
    for (int i = 0; i < 16; ++i) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[12] = float(W.x);
    m[13] = float(W.y);
    m[14] = float(W.z);
}

// Map current editor tool to an ImGuizmo OPERATION (sourced from
// kEditorTools so adding a tool is one table edit).
ImGuizmo::OPERATION ToolToGizmoOp(EEditorTool t)
{
    return ToolDef(t).gizmo_op;
}

// Pick the right category icon for an editor IObject. Sectors get the
// Icon for a TObjectInstance: dispatched by ObjClass().
sg_image IconForInstance(TObjectInstance* oi)
{
    if (!oi) return sg_image{ SG_INVALID_ID };
    return EditorIcons::ForObjClass(oi->ObjClass());
}

// Render one selectable instance row. Sectors are non-selectable
// expanders rendered in DrawSectorNode below.
void DrawInstanceRow(TObjectInstance* oi)
{
    if (!oi) return;
    const int32_t mi = oi->GetMapIndex();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_OpenOnDoubleClick |
                               ImGuiTreeNodeFlags_SpanAvailWidth |
                               ImGuiTreeNodeFlags_Leaf |
                               ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (IsSelected(mi)) flags |= ImGuiTreeNodeFlags_Selected;

    const sg_image icon = IconForInstance(oi);
    const float    sz   = ImGui::GetTextLineHeight();
    ImGui::TreeNodeEx((void*)(uintptr_t)mi, flags, "");
    if (g_selection_reveal && IsSelected(mi)) {
        // Manual visibility check vs the scrollable child's clip rect --
        // ImGui::IsItemVisible() lies the frame an ancestor expanded.
        const ImVec2 row_min = ImGui::GetItemRectMin();
        const ImVec2 row_max = ImGui::GetItemRectMax();
        const ImVec2 win_min = ImGui::GetWindowPos();
        const ImVec2 win_max = ImVec2(win_min.x + ImGui::GetWindowWidth(),
                                      win_min.y + ImGui::GetWindowHeight());
        const bool fully_visible = row_min.y >= win_min.y && row_max.y <= win_max.y;
        if (!fully_visible) ImGui::SetScrollHereY(0.5f);
    }
    g_tree_order_this_frame.push_back(mi);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        const ImGuiIO& io = ImGui::GetIO();
        if (io.KeyShift && g_selection_anchor >= 0) {
            const auto& flat = g_tree_order_this_frame;
            int from = -1, to = -1;
            for (size_t i = 0; i < flat.size(); ++i) {
                if (flat[i] == g_selection_anchor) from = int(i);
                if (flat[i] == mi)                  to   = int(i);
            }
            if (from >= 0 && to >= 0) {
                if (from > to) std::swap(from, to);
                g_selection.clear();
                for (int i = from; i <= to; ++i)
                    g_selection.push_back(flat[i]);
            } else {
                g_selection = { mi };
            }
        } else if (io.KeyCtrl) {
            auto it = std::find(g_selection.begin(), g_selection.end(), mi);
            if (it != g_selection.end()) g_selection.erase(it);
            else                          g_selection.push_back(mi);
            g_selection_anchor = mi;
        } else {
            g_selection        = { mi };
            g_selection_anchor = mi;
        }
    }
    ImGui::SameLine(0.0f, 0.0f);
    if (icon.id != SG_INVALID_ID)
        ImGui::Image((ImTextureID)(uintptr_t) icon.id, ImVec2(sz, sz));
    else
        ImGui::Dummy(ImVec2(sz, sz));
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    const char* nm = oi->GetTypeName();
    if (!nm || !*nm) nm = oi->GetClassName();
    if (!nm || !*nm) nm = "(unnamed)";
    ImGui::TextUnformatted(nm);
}

void DrawScenePanel()
{
    // Scene panel itself doesn't scroll; we put the header (filter checkbox
    // + counts) at the top and a scrollable child below for the tree.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    const bool open = ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar |
                                                     ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar();

    if (open) {
        TMapRenderer* mr = PlayScreen.MapRenderer();
        if (!mr) {
            ImGui::TextDisabled("(no map renderer)");
        } else {
            std::vector<TSector*> sectors;
            mr->GetLoadedSectors(sectors);

            // Sort sectors by (Y, X) so the hierarchy lists them in a
            // stable, predictable order regardless of the renderer's
            // internal load order. Objects within a sector are NOT
            // sorted -- their order matches the source data so editor
            // operations stay reproducible against the .DAT file.
            std::sort(sectors.begin(), sectors.end(),
                      [](TSector* a, TSector* b) {
                          if (!a || !b) return a < b;
                          if (a->SectorY() != b->SectorY())
                              return a->SectorY() < b->SectorY();
                          if (a->SectorX() != b->SectorX())
                              return a->SectorX() < b->SectorX();
                          return a->SectorLevel() < b->SectorLevel();
                      });

            // ---- Search + overflow header -----------------------------
            //
            // Magnifying-glass icon (Material Symbols) + InputTextWithHint
            // for filtering, then a vertical-kebab overflow button that
            // opens a popup of "panel option" toggles. Toggles that used
            // to live inline (Show empty sectors, ...) move into the
            // popup so the header stays compact.
            static char s_search[64] = "";

            const ImGuiStyle& st = ImGui::GetStyle();
            const float kebab_w = ImGui::GetFrameHeight();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(ICON_MS_SEARCH);
            ImGui::SameLine(0.0f, st.ItemInnerSpacing.x);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - kebab_w - st.ItemSpacing.x);
            ImGui::InputTextWithHint("##scene_search", "Search...",
                                     s_search, sizeof(s_search));
            ImGui::SameLine();
            if (ImGui::Button(ICON_MS_MORE_VERT "##scene_overflow",
                              ImVec2(kebab_w, kebab_w)))
                ImGui::OpenPopup("##scene_overflow_popup");
            if (ImGui::BeginPopup("##scene_overflow_popup")) {
                bool show_empty = EditorPrefs::GetBool(kPrefSceneShowEmpty, false);
                ImGuiToggleConfig cfg = ImGuiTogglePresets::MaterialStyle();
                cfg.Flags = ImGuiToggleFlags_Animated;
                if (ImGui::Toggle("Show empty sectors", &show_empty, cfg)) {
                    EditorPrefs::SetBool(kPrefSceneShowEmpty, show_empty);
                    EditorPrefs::Save();
                }
                ImGui::EndPopup();
            }

            // Case-insensitive substring search on (name | type | class).
            // Empty filter matches everything.
            const bool has_filter = s_search[0] != '\0';
            auto IcaseContains = [](const char* hay, const char* needle) -> bool {
                if (!hay || !needle) return false;
                if (!*needle) return true;
                for (const char* p = hay; *p; ++p) {
                    const char* a = p;
                    const char* b = needle;
                    while (*a && *b &&
                           std::tolower((uint8_t)*a) == std::tolower((uint8_t)*b))
                    { ++a; ++b; }
                    if (!*b) return true;
                }
                return false;
            };
            auto MatchesInst = [&](TObjectInstance* oi) -> bool {
                if (!has_filter) return true;
                if (!oi) return false;
                if (IcaseContains(oi->GetName(),      s_search)) return true;
                if (IcaseContains(oi->GetTypeName(),  s_search)) return true;
                if (IcaseContains(oi->GetClassName(), s_search)) return true;
                return false;
            };

            const bool show_empty = EditorPrefs::GetBool(kPrefSceneShowEmpty, false);

            size_t non_empty = 0;
            for (TSector* s : sectors)
                if (s && s->NumItems() > 0) ++non_empty;
            ImGui::Text("Sectors: %zu loaded, %zu non-empty", sectors.size(), non_empty);
            ImGui::Separator();

            // Scrollable region for the tree. Sized to fill the rest of
            // the panel after the header above.
            if (ImGui::BeginChild("##scene_tree", ImVec2(0, 0), false,
                                  ImGuiWindowFlags_HorizontalScrollbar))
            {
                g_tree_order_this_frame.clear();
                for (TSector* s : sectors) {
                    if (!s) continue;
                    if (!show_empty && s->NumItems() <= 0) continue;

                    // Filter: count matching instances. If zero and the
                    // filter is non-empty, skip the whole sector. Cache
                    // the count so the inner loop can short-circuit.
                    const int32_t n = s->NumItems();
                    int32_t match_count = 0;
                    if (has_filter) {
                        for (int32_t i = 0; i < n; ++i)
                            if (MatchesInst(s->GetInstance(i))) ++match_count;
                        if (match_count == 0) continue;
                    }

                    // Auto-expand on a match so hits aren't hidden inside
                    // a collapsed sector. Same trick the selection reveal
                    // uses below.
                    if (has_filter && match_count > 0)
                        ImGui::SetNextItemOpen(true);

                    // When revealing the selection, force-open any
                    // sector that contains a selected instance so the
                    // row inside is exposed for SetScrollHereY to find.
                    // Without this, clicking a tile in the game view
                    // marks reveal=true but the sector stays collapsed
                    // and the row is never drawn.
                    if (g_selection_reveal) {
                        for (int32_t i = 0; i < n; ++i) {
                            TObjectInstance* oi = s->GetInstance(i);
                            if (oi && IsSelected(oi->GetMapIndex())) {
                                ImGui::SetNextItemOpen(true);
                                break;
                            }
                        }
                    }

                    char label[80];
                    if (has_filter)
                        std::snprintf(label, sizeof(label),
                                      "Sector %d,%d (%d/%d)##sec_%p",
                                      s->SectorX(), s->SectorY(),
                                      match_count, n, (void*)s);
                    else
                        std::snprintf(label, sizeof(label),
                                      "Sector %d,%d (%d)##sec_%p",
                                      s->SectorX(), s->SectorY(), n, (void*)s);

                    if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_SpanAvailWidth)) {
                        for (int32_t i = 0; i < n; ++i) {
                            TObjectInstance* oi = s->GetInstance(i);
                            if (has_filter && !MatchesInst(oi)) continue;
                            DrawInstanceRow(oi);
                        }
                        ImGui::TreePop();
                    }
                }
                // One-shot: clear after the tree had a chance to expand
                // ancestors and scroll to the selected row.
                g_selection_reveal = false;
            }
            ImGui::EndChild();
        }
    }
    ImGui::End();
}

// Render one IReflection property row. Reads the value via the schema,
// shows a type-appropriate widget, and writes back when edited (unless
// the property is ReadOnly).
void DrawPropertyRow(IObject* obj, const IReflection& r, const SPropertyDesc& p)
{
    if (HasFlag(p.flags, EPropertyFlag::Hidden)) return;

    const bool read_only = HasFlag(p.flags, EPropertyFlag::ReadOnly);
    SPropertyValue v;
    r.GetValue(obj, p, v);

    ImGui::PushID(p.name);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(p.label && *p.label ? p.label : p.name);

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);

    if (read_only) ImGui::BeginDisabled();

    switch (p.type) {
        case EPropertyType::Bool: {
            bool b = std::get_if<bool>(&v) ? std::get<bool>(v) : false;
            if (ImGui::Checkbox("##v", &b) && !read_only) {
                v = b; r.SetValue(obj, p, v); EditorMarkDirty();
            }
            break;
        }
        case EPropertyType::Int: {
            int n = std::get_if<int64_t>(&v) ? int(std::get<int64_t>(v)) : 0;
            if (ImGui::DragInt("##v", &n, 1.0f,
                               int(p.min_hint), int(p.max_hint)) && !read_only) {
                v = int64_t(n); r.SetValue(obj, p, v); EditorMarkDirty();
            }
            break;
        }
        case EPropertyType::Float: {
            float f = std::get_if<double>(&v) ? float(std::get<double>(v)) : 0.0f;
            if (ImGui::DragFloat("##v", &f, 0.1f,
                                 float(p.min_hint), float(p.max_hint)) && !read_only) {
                v = double(f); r.SetValue(obj, p, v); EditorMarkDirty();
            }
            break;
        }
        case EPropertyType::String: {
            std::string s = std::get_if<std::string>(&v) ? std::get<std::string>(v) : std::string();
            char buf[256];
            std::snprintf(buf, sizeof(buf), "%s", s.c_str());
            if (ImGui::InputText("##v", buf, sizeof(buf),
                                 read_only ? ImGuiInputTextFlags_ReadOnly : 0)
                && !read_only)
            {
                v = std::string(buf); r.SetValue(obj, p, v); EditorMarkDirty();
            }
            break;
        }
        case EPropertyType::Vec3Int: {
            SVec3i iv = std::get_if<SVec3i>(&v) ? std::get<SVec3i>(v) : SVec3i{};
            int xyz[3] = { iv.x, iv.y, iv.z };
            if (ImGui::DragInt3("##v", xyz, 1.0f) && !read_only) {
                v = SVec3i{ xyz[0], xyz[1], xyz[2] }; r.SetValue(obj, p, v); EditorMarkDirty();
            }
            break;
        }
        case EPropertyType::Vec3Float: {
            SVec3f fv = std::get_if<SVec3f>(&v) ? std::get<SVec3f>(v) : SVec3f{};
            float xyz[3] = { fv.x, fv.y, fv.z };
            if (ImGui::DragFloat3("##v", xyz, 0.1f) && !read_only) {
                v = SVec3f{ xyz[0], xyz[1], xyz[2] }; r.SetValue(obj, p, v); EditorMarkDirty();
            }
            break;
        }
        case EPropertyType::Color: {
            SColor4f c = std::get_if<SColor4f>(&v) ? std::get<SColor4f>(v) : SColor4f{};
            float rgba[4] = { c.r, c.g, c.b, c.a };
            if (ImGui::ColorEdit4("##v", rgba) && !read_only) {
                v = SColor4f{ rgba[0], rgba[1], rgba[2], rgba[3] }; r.SetValue(obj, p, v); EditorMarkDirty();
            }
            break;
        }
        case EPropertyType::Enum: {
            int cur = std::get_if<int64_t>(&v) ? int(std::get<int64_t>(v)) : 0;
            const char* preview = "(?)";
            for (int i = 0; i < p.enum_count; ++i)
                if (p.enum_options[i].value == cur) { preview = p.enum_options[i].label; break; }
            if (ImGui::BeginCombo("##v", preview)) {
                for (int i = 0; i < p.enum_count; ++i) {
                    const bool sel = (p.enum_options[i].value == cur);
                    if (ImGui::Selectable(p.enum_options[i].label, sel) && !read_only) {
                        v = int64_t(p.enum_options[i].value);
                        r.SetValue(obj, p, v); EditorMarkDirty();
                    }
                    if (sel) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            break;
        }
        case EPropertyType::ObjectRef: {
            IObject* ref = std::get_if<IObject*>(&v) ? std::get<IObject*>(v) : nullptr;
            ImGui::TextUnformatted(ref ? ref->GetName() : "(null)");
            // TODO: drag-target / picker; selection swap on click.
            break;
        }
    }

    if (read_only) ImGui::EndDisabled();

    // Dirty-tracking: lock the resolver while the user is actively
    // dragging the widget so we don't generate one undo entry per
    // pixel. MarkDirty is idempotent -- the first call snapshots the
    // object, the rest are no-ops. Unlock at drag end fires Resolve,
    // which diffs and pushes a single entry per changed object.
    if (!read_only) {
        if (ImGui::IsItemActivated()) {
            EditorUndo::MarkDirty(obj);
            EditorUndo::LockResolver();
            EditorUndo::SetOperationName(
                p.label && *p.label ? p.label : (p.name ? p.name : "edit"));
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            EditorUndo::UnlockResolver();
        }
        // For widgets that don't go through Activated/Deactivated
        // (Checkbox, Combo selection, etc.) make sure a direct value
        // change still gets snapshotted, and also stamp a name for
        // the upcoming end-of-frame resolve.
        if (ImGui::IsItemEdited()) {
            EditorUndo::MarkDirty(obj);
            EditorUndo::SetOperationName(
                p.label && *p.label ? p.label : (p.name ? p.name : "edit"));
        }
    }
    ImGui::PopID();
}

void DrawPropertiesPanel()
{
    if (ImGui::Begin("Properties")) {
        IObject* primary = SelectionPrimaryAsObject();
        if (!primary) {
            ImGui::TextDisabled("(no selection)");
        } else {
            // Large category icon + name/class header.
            const sg_image icon = IconForInstance(SelectionPrimary());
            const float sz = 64.0f;
            if (icon.id != SG_INVALID_ID) {
                ImGui::Image((ImTextureID)(uintptr_t) icon.id, ImVec2(sz, sz));
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.x);
            }
            const IReflection& r = primary->GetRefl();
            ImGui::BeginGroup();
            ImGui::TextUnformatted(primary->GetName());
            ImGui::TextDisabled("[%s]", r.GetClassName());
            ImGui::EndGroup();
            ImGui::Separator();

            const int32_t n = r.NumProperties();
            if (n <= 0) {
                ImGui::TextDisabled("(no reflected properties)");
            } else if (ImGui::BeginTable("##props", 2,
                          ImGuiTableFlags_BordersInnerV |
                          ImGuiTableFlags_Resizable |
                          ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Name",  ImGuiTableColumnFlags_WidthStretch, 0.40f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.60f);
                for (int32_t i = 0; i < n; ++i)
                    DrawPropertyRow(primary, r, r.GetProperty(i));
                ImGui::EndTable();
            }
        }
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

// Floating toolbar drawn inside the Game View panel. Tool selection +
// play/pause + frame button. Tools wire into ImGuizmo once that's bound
// to the renderer's view+proj.
void DrawGameViewToolbar()
{
    constexpr float kBtnSize    = 36.0f;
    constexpr float kBtnRound   = 8.0f;
    constexpr float kGroupGap   = 18.0f;

    // Pad away from the panel edges -- BeginChild's WindowPadding
    // doesn't always reach SameLined items consistently.
    ImGui::SetCursorPos(ImVec2(12.0f, 8.0f));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(6, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  ImVec2(6, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kBtnRound);
    if (ImFont* icon = EditorFonts::GetIconFont()) ImGui::PushFont(icon);

    auto IconButton = [&](const char* glyph, const char* tip, bool active) -> bool {
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.26f, 0.45f, 0.75f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.50f, 0.82f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.34f, 0.55f, 0.90f, 1.0f));
        }
        const bool clicked = ImGui::Button(glyph, ImVec2(kBtnSize, kBtnSize));
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
            ImGui::SetTooltip("%s", tip);
        if (active) ImGui::PopStyleColor(3);
        return clicked;
    };

    auto ToolButton = [&](EEditorTool t) {
        const SEditorToolDef& def = ToolDef(t);
        if (IconButton(def.icon, def.tooltip, g_tool == t)) g_tool = t;
    };

    ToolButton(EEditorTool::Select);
    ImGui::SameLine();
    ToolButton(EEditorTool::Pan);
    ImGui::SameLine();
    ToolButton(EEditorTool::Move);
    ImGui::SameLine();
    ToolButton(EEditorTool::Rotate);
    ImGui::SameLine();
    ToolButton(EEditorTool::Scale);

    ImGui::SameLine(0.0f, kGroupGap);
    if (IconButton(EditorRunning() ? ICON_MS_PAUSE : ICON_MS_PLAY_ARROW,
                   "Play / Pause (Space)", EditorRunning()))
        EditorSetRunning(!EditorRunning());

    ImGui::SameLine();
    if (IconButton(ICON_MS_FIT_SCREEN, "Frame Selection (F)", false)) {
        if (const SEditorCommand* c = EditorCommands::Find("View.Frame"))
            if (c->execute) c->execute();
    }

    if (EditorFonts::GetIconFont()) ImGui::PopFont();
    ImGui::PopStyleVar(3);
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
        // Toolbar lives inside the panel above the game image.
        constexpr float kToolbarBtn = 36.0f;
        constexpr float kToolbarPad = 8.0f;
        ImGui::BeginChild("##gv_toolbar",
                          ImVec2(0, kToolbarBtn + 2.0f * kToolbarPad),
                          false, ImGuiWindowFlags_NoScrollbar);
        DrawGameViewToolbar();
        ImGui::EndChild();

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

        // ----- Selection / gizmo overlay over the game image ---------
        if (TMapRenderer* mr = PlayScreen.MapRenderer()) {
            float view[16], proj[16];
            mr->GetViewProj(view, proj, kGameViewW, kGameViewH);

            ImGuizmo::SetOrthographic(true);
            ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
            ImGuizmo::SetRect(img_min.x, img_min.y, gw, gh);

            // ----- Pan: tool==Pan, OR Alt-drag in any tool ------------
            const ImGuiIO& mio = ImGui::GetIO();
            const ImVec2 mp = ImGui::GetMousePos();
            const bool over_image = mp.x >= img_min.x && mp.x < img_min.x + gw
                                 && mp.y >= img_min.y && mp.y < img_min.y + gh;
            const float img_px = (mp.x - img_min.x) * (float(kGameViewW) / gw);
            const float img_py = (mp.y - img_min.y) * (float(kGameViewH) / gh);
            const bool pan_active = (g_tool == EEditorTool::Pan) || mio.KeyAlt;

            if (g_panning && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                mr->HandleMouseClick(MB_LEFTUP, int32_t(img_px), int32_t(img_py));
                g_panning = false;
            }
            if (!g_panning && pan_active && over_image && !ImGuizmo::IsUsing()
                && ImGui::IsMouseClicked(ImGuiMouseButton_Left)
                && !ImGui::IsAnyItemHovered())
            {
                mr->HandleMouseClick(MB_LEFTDOWN, int32_t(img_px), int32_t(img_py));
                g_panning = true;
            }
            if (g_panning) {
                mr->HandleMouseMove(MB_LEFTDOWN, int32_t(img_px), int32_t(img_py));
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            } else if (pan_active && over_image) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }

            TObjectInstance* oi = SelectionPrimary();

            const bool is_tile = oi && oi->ObjClass() == OBJCLASS_TILE;
            // Tiles can only translate -- they don't carry per-instance
            // rotation or scale yet. Suppress the rotate/scale gizmos
            // when a tile is selected (the AABB + axis triad still draw).
            const bool gizmo_tool = (g_tool == EEditorTool::Move) ||
                                    (!is_tile && (g_tool == EEditorTool::Rotate ||
                                                  g_tool == EEditorTool::Scale));
            if (oi && gizmo_tool) {
                struct SDragEntry { int32_t mapindex; S3DPoint from; };
                static TObjectInstance*        s_drag_primary = nullptr;
                static S3DPoint                s_drag_start   = {0,0,0};
                static std::vector<SDragEntry> s_drag_others;

                ImGuizmo::SetGizmoSizeClipSpace(0.20f);   // 2x default

                float model[16];
                ModelMatrixFromTranslation(oi->Pos(), model);
                const bool was_using = ImGuizmo::IsUsing();
                if (ImGuizmo::Manipulate(view, proj,
                                         ToolToGizmoOp(g_tool),
                                         ImGuizmo::WORLD,
                                         model)) {
                    if (g_tool == EEditorTool::Move) {
                        if (!s_drag_primary) {
                            s_drag_primary = oi;
                            s_drag_start   = oi->Pos();
                            s_drag_others.clear();
                            // Mark every selected adapter dirty before
                            // touching their underlying instances. Lock
                            // the resolver until drag-end so the diff
                            // batches across the whole drag into one
                            // undo entry.
                            EditorUndo::LockResolver();
                            EditorUndo::SetOperationName(
                                g_tool == EEditorTool::Move   ? "move"   :
                                g_tool == EEditorTool::Rotate ? "rotate" :
                                g_tool == EEditorTool::Scale  ? "scale"  : "transform");
                            for (int32_t mi : g_selection) {
                                TObjectInstance* sel = safe_cast<TObjectInstance>(mi);
                                if (!sel) continue;
                                EditorUndo::MarkDirty(EditorAdapters().For(sel));
                                if (sel != oi)
                                    s_drag_others.push_back({ mi, sel->Pos() });
                            }
                        }
                        const S3DPoint np{ int32_t(model[12]),
                                           int32_t(model[13]),
                                           int32_t(model[14]) };
                        const int32_t dx = np.x - s_drag_start.x;
                        const int32_t dy = np.y - s_drag_start.y;
                        const int32_t dz = np.z - s_drag_start.z;
                        oi->SetPos(np, -1, true);
                        for (const auto& e : s_drag_others) {
                            if (auto* other = safe_cast<TObjectInstance>(e.mapindex))
                                other->SetPos({ e.from.x + dx, e.from.y + dy,
                                                e.from.z + dz }, -1, true);
                        }
                        EditorMarkDirty();
                    }
                }
                // Drag ended: unlock the resolver. The diff against the
                // baseline snapshots produces one undo entry per moved
                // instance (or zero entries if the user dragged back to
                // the start -- changebacks naturally collapse).
                if (was_using && !ImGuizmo::IsUsing() && s_drag_primary) {
                    s_drag_primary = nullptr;
                    s_drag_others.clear();
                    EditorUndo::UnlockResolver();
                }
            }
            // Click-pick raycast: available in tools where clicking has
            // no other meaning -- Select + the transform tools (Move /
            // Rotate / Scale) so clicking off the gizmo reselects.
            // Suppressed in Pan (a click is a pan gesture there) and
            // while Alt is held (Alt = pan modifier in any tool).
            // Skipped when the gizmo would consume the click or while
            // the gizmo is dragging.
            // Modifier rules:
            //   no mod    -> replace selection
            //   Ctrl      -> toggle add/remove
            //   Shift     -> extend selection
            //   else      -> click belongs to another gesture (pan, ...)
            {
                const bool no_mod    = !mio.KeyShift && !mio.KeyAlt && !mio.KeySuper && !mio.KeyCtrl;
                const bool ctrl_add  = !mio.KeyShift && !mio.KeyAlt && !mio.KeySuper &&  mio.KeyCtrl;
                const bool shift_add =  mio.KeyShift && !mio.KeyAlt && !mio.KeySuper && !mio.KeyCtrl;
                if (ToolDef(g_tool).can_click_select
                    && over_image && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver()
                    && !ImGui::IsAnyItemHovered()
                    && (no_mod || ctrl_add || shift_add)
                    && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    const int32_t pad = Renderer ? Renderer->GBufPad() : 0;
                    const int32_t px  = int32_t(img_px) + pad;
                    const int32_t py  = int32_t(img_py) + pad;
                    uint8_t rgba[4] = {0,0,0,0};
                    TObjectInstance* hit = nullptr;
                    if (Renderer && RendererReadback::ReadPixel(Renderer->IdTarget(), px, py, rgba)) {
                        const uint32_t obj_id = uint32_t(rgba[0])
                                             | (uint32_t(rgba[1]) << 8)
                                             | (uint32_t(rgba[2]) << 16)
                                             | (uint32_t(rgba[3]) << 24);
                        if (obj_id != 0) hit = mr->InstanceFromDrawId(obj_id);
                    }
                    const int32_t hit_mi = hit ? hit->GetMapIndex() : -1;
                    if (ctrl_add) {
                        if (hit_mi >= 0) {
                            auto it = std::find(g_selection.begin(), g_selection.end(), hit_mi);
                            if (it != g_selection.end()) g_selection.erase(it);
                            else                          g_selection.push_back(hit_mi);
                            g_selection_anchor = hit_mi;
                        }
                    } else if (shift_add) {
                        if (hit_mi >= 0 && std::find(g_selection.begin(), g_selection.end(), hit_mi) == g_selection.end())
                            g_selection.push_back(hit_mi);
                        // Don't move anchor; keeps tree shift-range
                        // working across game-view picks too.
                    } else {
                        if (hit_mi >= 0) { g_selection = { hit_mi }; g_selection_anchor = hit_mi; }
                        else             { g_selection.clear(); }
                    }
                    const int32_t new_primary = g_selection.empty() ? -1 : g_selection.back();
                    if (new_primary >= 0 && new_primary != g_last_revealed_primary) {
                        g_selection_reveal      = true;
                        g_last_revealed_primary = new_primary;
                    }
                }
            }
            // Selection visualisation: project the instance's world-space
            // bounding box (from imagery) to panel pixels and outline it.
            // Same AABB data drives walkmap + render bounds, so this works
            // for tiles, billboards, and 3D meshes.
            if (oi) {
                auto WorldToPanel = [&](const S3DPoint& w) {
                    auto mul = [](const float* M, float x, float y, float z, float w_,
                                  float& ox, float& oy, float& oz, float& ow) {
                        ox = M[0]*x + M[4]*y + M[8 ]*z + M[12]*w_;
                        oy = M[1]*x + M[5]*y + M[9 ]*z + M[13]*w_;
                        oz = M[2]*x + M[6]*y + M[10]*z + M[14]*w_;
                        ow = M[3]*x + M[7]*y + M[11]*z + M[15]*w_;
                    };
                    float vx,vy,vz,vw; mul(view, float(w.x), float(w.y), float(w.z), 1.0f, vx,vy,vz,vw);
                    float cx,cy,cz,cw; mul(proj,  vx,vy,vz,vw, cx,cy,cz,cw);
                    if (cw == 0.0f) cw = 1.0f;
                    cx /= cw; cy /= cw;
                    return ImVec2(img_min.x + (cx * 0.5f + 0.5f) * gw,
                                  img_min.y + (1.0f - (cy * 0.5f + 0.5f)) * gh);
                };

                ImDrawList* dl = ImGui::GetWindowDrawList();
                const ImU32 col = IM_COL32(255, 220, 50, 255);

                int32_t bw = 0, bl = 0, bh = 0, rx = 0, ry = 0, rz = 0;
                int32_t img_w = 0, img_h = 0, img_rx = 0, img_ry = 0;
                if (TObjectImagery* simg = oi->GetImagery()) {
                    const int32_t st = oi->GetState();
                    simg->GetWorldBoundBox(st, bw, bl, bh);
                    rx = simg->GetWorldRegX(st);
                    ry = simg->GetWorldRegY(st);
                    rz = simg->GetWorldRegZ(st);
                    img_w  = simg->GetWidth(st);
                    img_h  = simg->GetHeight(st);
                    img_rx = simg->GetRegX(st);
                    img_ry = simg->GetRegY(st);
                }
                // Log once per selection target so we can see what bounds
                // a given object exposes.
                static const TObjectInstance* s_last_logged = nullptr;
                if (s_last_logged != oi) {
                    s_last_logged = oi;
                    log_info("[editor] sel bounds: class=%s type=%s "
                             "world(w=%d l=%d h=%d reg=%d,%d,%d) "
                             "img(w=%d h=%d reg=%d,%d) state=%d",
                             oi->GetClassName() ? oi->GetClassName() : "?",
                             oi->GetTypeName()  ? oi->GetTypeName()  : "?",
                             bw, bl, bh, rx, ry, rz,
                             img_w, img_h, img_rx, img_ry,
                             oi->GetState());
                }
                if (bw > 0 && bl > 0) {
                    // Tiles legitimately have wheight=0 (flat floor plates).
                    // Draw the floor quad in that case rather than a degenerate
                    // cube. For real 3D bounds, draw the full 12-edge AABB.
                    const S3DPoint& P = oi->Pos();
                    const int32_t x0 = P.x - rx, y0 = P.y - ry, z0 = P.z - rz;
                    const int32_t x1 = x0 + bw, y1 = y0 + bl;
                    if (bh > 0) {
                        const int32_t z1 = z0 + bh;
                        const ImVec2 c[8] = {
                            WorldToPanel({x0,y0,z0}), WorldToPanel({x1,y0,z0}),
                            WorldToPanel({x1,y1,z0}), WorldToPanel({x0,y1,z0}),
                            WorldToPanel({x0,y0,z1}), WorldToPanel({x1,y0,z1}),
                            WorldToPanel({x1,y1,z1}), WorldToPanel({x0,y1,z1}),
                        };
                        constexpr int edges[12][2] = {
                            {0,1},{1,2},{2,3},{3,0},
                            {4,5},{5,6},{6,7},{7,4},
                            {0,4},{1,5},{2,6},{3,7},
                        };
                        for (auto& e : edges) dl->AddLine(c[e[0]], c[e[1]], col, 1.0f);
                    } else {
                        const ImVec2 c[4] = {
                            WorldToPanel({x0,y0,z0}), WorldToPanel({x1,y0,z0}),
                            WorldToPanel({x1,y1,z0}), WorldToPanel({x0,y1,z0}),
                        };
                        for (int i = 0; i < 4; ++i)
                            dl->AddLine(c[i], c[(i+1)&3], col, 1.0f);
                    }
                } else if (img_w > 0 && img_h > 0) {
                    // No walkmap (typical for decorative tiles + billboards).
                    // Fall back to the imagery's screen-space sprite rect:
                    // map image-pixel (regx,regy)->(regx+w, regy+h) onto the
                    // panel relative to the projected anchor.
                    const ImVec2 p = WorldToPanel(oi->Pos());
                    const float sx = gw / float(kGameViewW);   // panel-px per game-px
                    const float sy = gh / float(kGameViewH);
                    const ImVec2 a(p.x - float(img_rx) * sx,
                                   p.y - float(img_ry) * sy);
                    const ImVec2 b(a.x + float(img_w)  * sx,
                                   a.y + float(img_h)  * sy);
                    dl->AddRect(a, b, col, 0.0f, 0, 1.0f);
                } else {
                    // No spatial extents at all -- small cross at anchor
                    // (lights, helpers, abstract effects).
                    const ImVec2 p = WorldToPanel(oi->Pos());
                    dl->AddLine(ImVec2(p.x-6, p.y), ImVec2(p.x+6, p.y), col, 1.0f);
                    dl->AddLine(ImVec2(p.x, p.y-6), ImVec2(p.x, p.y+6), col, 1.0f);
                }

                // Always draw a small filled dot on the anchor too -- gives
                // the eye a stable reference even for tall / wide bboxes.
                const S3DPoint& origin = oi->Pos();
                const ImVec2 anchor = WorldToPanel(origin);
                dl->AddCircleFilled(anchor, 3.0f, col);

                // Local-axis triad at the object's origin. Suppressed
                // when a gizmo tool is active -- the gizmo handles
                // overlap visually and the axis just adds clutter.
                if (!gizmo_tool) {
                const int32_t axis_len = 48;
                const ImVec2 ax = WorldToPanel({origin.x + axis_len, origin.y, origin.z});
                const ImVec2 ay = WorldToPanel({origin.x, origin.y + axis_len, origin.z});
                const ImVec2 az = WorldToPanel({origin.x, origin.y, origin.z + axis_len});
                auto Arrow = [&](const ImVec2& a, const ImVec2& b, ImU32 c_arrow, float thick) {
                    dl->AddLine(a, b, c_arrow, thick);
                    const ImVec2 d = ImVec2(b.x - a.x, b.y - a.y);
                    const float len2 = d.x*d.x + d.y*d.y;
                    if (len2 < 1.0f) return;
                    const float invlen = 1.0f / std::sqrt(len2);
                    const ImVec2 dir(d.x * invlen, d.y * invlen);
                    const ImVec2 perp(-dir.y, dir.x);
                    constexpr float head = 8.0f, wing = 4.0f;
                    const ImVec2 h0(b.x - dir.x * head + perp.x * wing,
                                    b.y - dir.y * head + perp.y * wing);
                    const ImVec2 h1(b.x - dir.x * head - perp.x * wing,
                                    b.y - dir.y * head - perp.y * wing);
                    dl->AddLine(b, h0, c_arrow, thick);
                    dl->AddLine(b, h1, c_arrow, thick);
                };
                Arrow(anchor, ax, IM_COL32(255, 64, 64, 255), 2.0f);   // X red
                Arrow(anchor, ay, IM_COL32(64, 220, 64, 255), 2.0f);   // Y green
                Arrow(anchor, az, IM_COL32(80, 140, 255, 255), 2.0f);  // Z blue
                } // !gizmo_tool
            }
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

// ---------------------------------------------------------------------------
// World position lookup for selection
// ---------------------------------------------------------------------------
//
// IObject doesn't expose a world position directly; only the editor's
// adapters know how to read one out of the underlying game object. The
// Frame command uses dynamic_cast since both adapter types live here.

bool ObjectWorldPos(IObject* obj, int32_t& level, int32_t& wx, int32_t& wy, int32_t& wz)
{
    if (!obj) return false;
    if (auto* ia = dynamic_cast<TInstanceAdapter*>(obj); ia && ia->Instance()) {
        const S3DPoint& p = ia->Instance()->Pos();
        level = ia->Instance()->GetLevel();
        wx = p.x; wy = p.y; wz = p.z;
        return true;
    }
    if (auto* sa = dynamic_cast<TSectorAdapter*>(obj); sa && sa->Sector()) {
        TSector* s = sa->Sector();
        level = s->SectorLevel();
        wx = s->SectorX() * SECTORWIDTH  + SECTORWIDTH  / 2;
        wy = s->SectorY() * SECTORHEIGHT + SECTORHEIGHT / 2;
        wz = 0;
        return true;
    }
    return false;
}

// Editor color tweaks. Applied once -- ImGui's style is global and our
// edits stick across frames. Goal: lighter, opaque dark-gray panels so
// they read distinctly from the (black) play window.
void ApplyEditorStyle()
{
    ImGuiStyle& s = ImGui::GetStyle();
    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]        = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
    c[ImGuiCol_ChildBg]         = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
    c[ImGuiCol_PopupBg]         = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    c[ImGuiCol_MenuBarBg]       = ImVec4(0.22f, 0.22f, 0.24f, 1.00f);
    c[ImGuiCol_TitleBg]         = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
    c[ImGuiCol_TitleBgActive]   = ImVec4(0.24f, 0.24f, 0.28f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]= ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
    c[ImGuiCol_FrameBg]         = ImVec4(0.24f, 0.24f, 0.28f, 1.00f);
    c[ImGuiCol_FrameBgHovered]  = ImVec4(0.30f, 0.30f, 0.34f, 1.00f);
    c[ImGuiCol_FrameBgActive]   = ImVec4(0.34f, 0.34f, 0.40f, 1.00f);
    c[ImGuiCol_TableHeaderBg]   = ImVec4(0.22f, 0.22f, 0.26f, 1.00f);
    c[ImGuiCol_TableRowBg]      = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    c[ImGuiCol_TableRowBgAlt]   = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
    c[ImGuiCol_Tab]             = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    c[ImGuiCol_TabHovered]      = ImVec4(0.34f, 0.34f, 0.40f, 1.00f);
    c[ImGuiCol_TabSelected]     = ImVec4(0.28f, 0.28f, 0.34f, 1.00f);
    c[ImGuiCol_DockingEmptyBg]  = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
}

// Delete-with-undo: capture serialized state (with @refid) for the
// selection up front, then delete on Redo and re-instantiate on Undo.
// Restoring with the original mapindices implicitly heals every TSafeRef
// elsewhere in the world that pointed at the deleted objects (see
// memory/project_object_ref.md). Name is parameterised so Cut shows up
// as "Cut" in the undo menu while still using this command body.
class TDeleteSelectionCommand : public IEditorCommand
{
public:
    TDeleteSelectionCommand(std::vector<int32_t> mapindices, const char* name)
        : mapindices_(std::move(mapindices)), name_(name ? name : "Delete")
    {
        std::vector<TObjectInstance*> live;
        live.reserve(mapindices_.size());
        for (int32_t mi : mapindices_)
            if (auto* oi = safe_cast<TObjectInstance>(mi)) live.push_back(oi);
        text_ = EditorSerialize::ToText(live, /*include_refid=*/true);
    }

    void Redo() override
    {
        // MapPane.DeleteObject scans MapPane.sectors[][] which is empty
        // in editor mode (the renderer manages its own loaded-sector
        // list independently). Go straight through the instance's own
        // sector pointer so we don't depend on MapPane paging state.
        // See memory/project_editor_object_model.md for the longer
        // story on the MapPane/renderer ownership split.
        for (int32_t mi : mapindices_) {
            TObjectInstance* oi = safe_cast<TObjectInstance>(mi);
            if (!oi) continue;
            TSector* sec = oi->GetSector();
            if (!sec) continue;
            const int32_t idx = sec->GetObjIndex(oi);
            if (idx < 0) continue;
            sec->RemoveObject(idx);
            delete oi;            // ~TObjectInstance unregisters from saferef registry
        }
        EditorMarkDirty();
    }

    void Undo() override
    {
        EditorSerialize::FromText(text_, /*preserve_refid=*/true);
        EditorMarkDirty();
    }

    const char* Name() const override { return name_.c_str(); }

private:
    std::vector<int32_t> mapindices_;
    std::string          text_;
    std::string          name_;
};

// Inverse of TDeleteSelectionCommand: instantiates objects from
// serialized text on Redo (used by Paste, Duplicate, future Spawn).
// First Redo allocates fresh mapindices and snapshots the resulting
// state with @refid so subsequent redo cycles bring the SAME instances
// back -- this keeps any external mapindex references (selection,
// inventory pointers, undo stack entries below this one) consistent
// across undo/redo flips.
class TCreateInstancesCommand : public IEditorCommand
{
public:
    TCreateInstancesCommand(std::string text, const char* name)
        : initial_text_(std::move(text)), name_(name ? name : "Create") {}

    void Redo() override
    {
        if (created_text_.empty()) {
            // First execution: fresh ids, then re-serialize with @refid
            // so future redos pin to the same instances.
            created_indices_ = EditorSerialize::FromText(initial_text_, /*preserve_refid=*/false);
            std::vector<TObjectInstance*> live;
            live.reserve(created_indices_.size());
            for (int32_t mi : created_indices_)
                if (auto* oi = safe_cast<TObjectInstance>(mi)) live.push_back(oi);
            created_text_ = EditorSerialize::ToText(live, /*include_refid=*/true);
        } else {
            created_indices_ = EditorSerialize::FromText(created_text_, /*preserve_refid=*/true);
        }
        EditorMarkDirty();
    }

    void Undo() override
    {
        // Same MapPane bypass as TDeleteSelectionCommand::Redo:
        // MapPane.DeleteObject iterates MapPane.sectors[][] (empty in
        // editor mode) and FATALs. Go through oi->GetSector() directly.
        for (int32_t mi : created_indices_) {
            TObjectInstance* oi = safe_cast<TObjectInstance>(mi);
            if (!oi) continue;
            TSector* sec = oi->GetSector();
            if (!sec) continue;
            const int32_t idx = sec->GetObjIndex(oi);
            if (idx < 0) continue;
            sec->RemoveObject(idx);
            delete oi;
        }
        created_indices_.clear();
        EditorMarkDirty();
    }

    const char* Name() const override { return name_.c_str(); }

    // Read-only view of mapindices created by the most recent Redo.
    // Used by the registering command's execute lambda to update
    // g_selection so the freshly-created objects show as selected.
    const std::vector<int32_t>& CreatedIndices() const { return created_indices_; }

private:
    std::string          initial_text_;
    std::string          created_text_;
    std::vector<int32_t> created_indices_;
    std::string          name_;
};

void RegisterBuiltinCommands()
{
    if (g_commands_registered) return;
    g_commands_registered = true;

    // -- View.Frame -- center the camera on the selection's average pos.
    EditorCommands::Register({
        .id        = "View.Frame",
        .label     = "Frame Selection",
        .menu_path = "View",
        .chord     = ImGuiKey_F,
        .shortcut  = "F",
        .is_enabled = []{ return !g_selection.empty() && PlayScreen.MapRenderer() != nullptr; },
        .is_checked = nullptr,
        .execute   = []{
            TMapRenderer* mr = PlayScreen.MapRenderer();
            if (!mr) return;
            int64_t sx = 0, sy = 0, sz = 0;
            int32_t count = 0;
            int32_t level = 0;
            for (int32_t mi : g_selection) {
                TObjectInstance* sel = safe_cast<TObjectInstance>(mi);
                if (!sel) continue;
                if (count == 0) level = sel->GetLevel();
                const S3DPoint& p = sel->Pos();
                sx += p.x; sy += p.y; sz += p.z; ++count;
            }
            if (count == 0) return;
            mr->SetCameraWorld(level,
                               int32_t(sx / count),
                               int32_t(sy / count),
                               int32_t(sz / count));
            EditorMarkDirty();
            log_info("[editor] frame: level=%d  world=(%lld, %lld, %lld)  selection=%d",
                     level, (long long)(sx/count), (long long)(sy/count),
                     (long long)(sz/count), count);
        },
    });

    // -- Tool selection (Q/W/E/R) ------------------------------------------
    EditorCommands::Register({
        .id = "Tool.Select", .label = "Select Tool", .menu_path = "",
        .chord = ImGuiKey_Q, .shortcut = "Q",
        .is_checked = []{ return g_tool == EEditorTool::Select; },
        .execute    = []{ g_tool = EEditorTool::Select; EditorMarkDirty(); },
    });
    EditorCommands::Register({
        .id = "Tool.Pan", .label = "Pan Tool", .menu_path = "",
        .chord = ImGuiKey_H, .shortcut = "H",
        .is_checked = []{ return g_tool == EEditorTool::Pan; },
        .execute    = []{ g_tool = EEditorTool::Pan; EditorMarkDirty(); },
    });
    EditorCommands::Register({
        .id = "Tool.Move", .label = "Move Tool", .menu_path = "",
        .chord = ImGuiKey_W, .shortcut = "W",
        .is_checked = []{ return g_tool == EEditorTool::Move; },
        .execute    = []{ g_tool = EEditorTool::Move; EditorMarkDirty(); },
    });
    EditorCommands::Register({
        .id = "Tool.Rotate", .label = "Rotate Tool", .menu_path = "",
        .chord = ImGuiKey_E, .shortcut = "E",
        .is_checked = []{ return g_tool == EEditorTool::Rotate; },
        .execute    = []{ g_tool = EEditorTool::Rotate; EditorMarkDirty(); },
    });
    EditorCommands::Register({
        .id = "Tool.Scale", .label = "Scale Tool", .menu_path = "",
        .chord = ImGuiKey_R, .shortcut = "R",
        .is_checked = []{ return g_tool == EEditorTool::Scale; },
        .execute    = []{ g_tool = EEditorTool::Scale; EditorMarkDirty(); },
    });

    // -- Player.PlaceHere -- relocate the main player to the world XY
    // currently centered in the Game View, with Z resolved off the
    // walkmap. If no Player exists yet (boot-time spawn was skipped for
    // some reason), spawn one at the anchor sector first. This is
    // primarily a debug/iteration helper while the input + walkmap
    // stack is being wired up.
    EditorCommands::Register({
        .id         = "Player.PlaceHere",
        .label      = "Place Player at View Center",
        .menu_path  = "Edit",
        .chord      = 0,
        .shortcut   = "",
        .is_enabled = []{ return PlayScreen.MapRenderer() != nullptr; },
        .is_checked = nullptr,
        .execute    = []{
            TMapRenderer* mr = PlayScreen.MapRenderer();
            if (!mr) return;

            S3DPoint        dst       = mr->CameraWorld();
            const int32_t   cam_level = mr->CameraLevel();
            const int32_t   cam_sx    = dst.x >> SECTORWSHIFT;
            const int32_t   cam_sy    = dst.y >> SECTORHSHIFT;
            dst.z = MapPane.GetWalkHeight(dst);

            if (!Player)
            {
                // Boot-spawn path; will land at the anchor sector
                // center. We then re-place precisely at dst below.
                PlayScreen.SpawnDefaultPlayer(cam_level, cam_sx, cam_sy);
            }

            if (!Player)
            {
                log_warn("[player] PlaceHere: no Player and spawn failed");
                return;
            }

            // SetMainPlayer's UI side effects (run inside SetPos via
            // the legacy walkmap/transfer path) want CurrentScreen to
            // be set up; clear it briefly so a place in a half-init
            // editor frame doesn't poke an unprepared HealthBar.
            TScreen* saved_screen = CurrentScreen;
            CurrentScreen = nullptr;
            Player->SetPos(dst, cam_level, /*override=*/true);
            CurrentScreen = saved_screen;

            EditorMarkDirty();
            log_info("[player] placed at sector %d_%d_%d world (%d,%d,%d)",
                     cam_level, cam_sx, cam_sy, dst.x, dst.y, dst.z);
        },
    });

    // -- View.PlayPause -- toggle the running flag. While paused the
    // Game View keeps showing the last rendered frame; dirty marks force
    // single-frame re-renders for property edits etc.
    EditorCommands::Register({
        .id        = "View.PlayPause",
        .label     = "Play / Pause",
        .menu_path = "View",
        .chord     = ImGuiKey_Space,
        .shortcut  = "Space",
        .is_enabled = nullptr,
        .is_checked = []{ return g_running; },
        .execute   = []{ EditorSetRunning(!g_running); },
    });

    // -- Edit.Undo / Edit.Redo -------------------------------------------
    // ImGuiMod_Ctrl is the cross-platform "primary shortcut" modifier.
    // With ConfigMacOSXBehaviors=true AND trickle enabled, ImGui auto-
    // swaps incoming Cmd events to ImGuiMod_Ctrl on Mac, so a single
    // chord binding fires on Cmd+Z (macOS) and Ctrl+Z (Win/Linux).
    // (Trickle is required for the swap to land — see
    // memory/project_imgui_mac_modifiers.md.)
    constexpr ImGuiKeyChord kPrimaryMod = ImGuiMod_Ctrl;
    EditorCommands::Register({
        .id        = "Edit.Undo",
        .label     = "Undo",
        .menu_path = "Edit",
        .chord     = kPrimaryMod | ImGuiKey_Z,
        .shortcut  = "",                      // menu renders this dynamically
        .is_enabled = []{ return EditorUndo::CanUndo(); },
        .is_checked = nullptr,
        .execute   = []{ EditorUndo::Undo(); EditorMarkDirty(); },
    });
    EditorCommands::Register({
        .id        = "Edit.Redo",
        .label     = "Redo",
        .menu_path = "Edit",
        .chord     = kPrimaryMod | ImGuiMod_Shift | ImGuiKey_Z,
        .shortcut  = "",
        .is_enabled = []{ return EditorUndo::CanRedo(); },
        .is_checked = nullptr,
        .execute   = []{ EditorUndo::Redo(); EditorMarkDirty(); },
    });

    // -- Edit.Delete -- remove selected instances, restorable via undo.
    // Registered chord is Backspace (Mac convention); the standalone
    // Delete (forward-delete) key is also accepted via an inline check
    // in EditorDrawChrome so both work without two registry entries.
    EditorCommands::Register({
        .id        = "Edit.Delete",
        .label     = "Delete",
        .menu_path = "Edit",
        .chord     = ImGuiKey_Backspace,
        .shortcut  = "Del",
        .is_enabled = []{ return !g_selection.empty(); },
        .is_checked = nullptr,
        .execute   = []{
            if (g_selection.empty()) return;
            EditorUndo::Push(std::make_unique<TDeleteSelectionCommand>(g_selection, "Delete"));
            g_selection.clear();
            g_selection_anchor       = -1;
            g_last_revealed_primary  = -1;
            EditorMarkDirty();
        },
    });

    // -- Edit.Copy -- serialize selection to OS clipboard, no undo entry
    //    (clipboard state isn't part of the world undo timeline).
    EditorCommands::Register({
        .id        = "Edit.Copy",
        .label     = "Copy",
        .menu_path = "Edit",
        .chord     = kPrimaryMod | ImGuiKey_C,
        .shortcut  = "",
        .is_enabled = []{ return !g_selection.empty(); },
        .is_checked = nullptr,
        .execute   = []{
            if (g_selection.empty()) return;
            std::vector<TObjectInstance*> live;
            live.reserve(g_selection.size());
            for (int32_t mi : g_selection)
                if (auto* oi = safe_cast<TObjectInstance>(mi)) live.push_back(oi);
            if (live.empty()) return;
            const std::string text = EditorSerialize::ToText(live, /*include_refid=*/false);
            ImGui::SetClipboardText(text.c_str());
        },
    });

    // -- Edit.Cut -- Copy + Delete in a single undo entry named "Cut".
    EditorCommands::Register({
        .id        = "Edit.Cut",
        .label     = "Cut",
        .menu_path = "Edit",
        .chord     = kPrimaryMod | ImGuiKey_X,
        .shortcut  = "",
        .is_enabled = []{ return !g_selection.empty(); },
        .is_checked = nullptr,
        .execute   = []{
            if (g_selection.empty()) return;
            std::vector<TObjectInstance*> live;
            live.reserve(g_selection.size());
            for (int32_t mi : g_selection)
                if (auto* oi = safe_cast<TObjectInstance>(mi)) live.push_back(oi);
            if (live.empty()) return;
            const std::string text = EditorSerialize::ToText(live, /*include_refid=*/false);
            ImGui::SetClipboardText(text.c_str());
            EditorUndo::Push(std::make_unique<TDeleteSelectionCommand>(g_selection, "Cut"));
            g_selection.clear();
            g_selection_anchor       = -1;
            g_last_revealed_primary  = -1;
            EditorMarkDirty();
        },
    });

    // -- Edit.Paste -- read OS clipboard, instantiate fresh objects,
    //    select them. Header-sniff first so foreign clipboard text
    //    (URLs, code, whatever) is silently ignored.
    EditorCommands::Register({
        .id        = "Edit.Paste",
        .label     = "Paste",
        .menu_path = "Edit",
        .chord     = kPrimaryMod | ImGuiKey_V,
        .shortcut  = "",
        .is_enabled = []{
            const char* t = ImGui::GetClipboardText();
            return t && EditorSerialize::LooksLikeOurs(t);
        },
        .is_checked = nullptr,
        .execute   = []{
            const char* text = ImGui::GetClipboardText();
            if (!text || !EditorSerialize::LooksLikeOurs(text)) return;
            auto cmd = std::make_unique<TCreateInstancesCommand>(text, "Paste");
            auto* raw = cmd.get();
            EditorUndo::Push(std::move(cmd));
            const auto& created = raw->CreatedIndices();
            if (!created.empty()) {
                g_selection             = created;
                g_selection_anchor      = created.back();
                g_last_revealed_primary = -1;        // re-reveal in tree
                g_selection_reveal      = true;
            }
            EditorMarkDirty();
        },
    });

    // -- Edit.Duplicate -- clone selection in place, select the clones.
    //    Same machinery as Paste, just sourced from the live selection
    //    instead of the clipboard.
    EditorCommands::Register({
        .id        = "Edit.Duplicate",
        .label     = "Duplicate",
        .menu_path = "Edit",
        .chord     = kPrimaryMod | ImGuiKey_D,
        .shortcut  = "",
        .is_enabled = []{ return !g_selection.empty(); },
        .is_checked = nullptr,
        .execute   = []{
            if (g_selection.empty()) return;
            std::vector<TObjectInstance*> live;
            live.reserve(g_selection.size());
            for (int32_t mi : g_selection)
                if (auto* oi = safe_cast<TObjectInstance>(mi)) live.push_back(oi);
            if (live.empty()) return;
            const std::string text = EditorSerialize::ToText(live, /*include_refid=*/false);
            auto cmd = std::make_unique<TCreateInstancesCommand>(text, "Duplicate");
            auto* raw = cmd.get();
            EditorUndo::Push(std::move(cmd));
            const auto& created = raw->CreatedIndices();
            if (!created.empty()) {
                g_selection             = created;
                g_selection_anchor      = created.back();
                g_last_revealed_primary = -1;
                g_selection_reveal      = true;
            }
            EditorMarkDirty();
        },
    });
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
    g_running = false;        // editor opens paused; user hits Play to run
    if (Renderer) Renderer->SuppressPresent(true);
    if (TMapRenderer* mr = PlayScreen.MapRenderer()) {
        // Save + disable shadows by default in editor; user can flip via View.
        // Has to be on the MapRenderer because it re-pushes its own shadow
        // params to the core Renderer each frame.
        g_shadows_were_on = mr->SunShadowEnabled();
        mr->SetSunShadowEnabled(false);
    }

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
    g_running = true;          // game runs unconditionally outside editor
    if (Renderer) {
        Renderer->ResetPresentNDCRect();
        Renderer->SuppressPresent(false);
    }
    if (TMapRenderer* mr = PlayScreen.MapRenderer())
        mr->SetSunShadowEnabled(g_shadows_were_on);

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

// Play / pause + dirty marking ------------------------------------------------

bool EditorRunning() { return g_running; }
void EditorSetRunning(bool running) { g_running = running; }
void EditorMarkDirty() {}                        // no-op: we always render

// Render every frame -- ImGui-animated widgets need it. Game state
// already advances at 30Hz internally (kGameFrameRate in playscreen.cpp,
// driven by TTime::LegacyFrameCount), so render-60 / update-30 is
// already what we get.
bool EditorShouldRenderWorld() { return true; }

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

    RegisterBuiltinCommands();
    static bool s_style_applied = false;
    if (!s_style_applied) { ApplyEditorStyle(); s_style_applied = true; }
    EditorCommands::DispatchHotkeys();
    // Forward-delete key as a second binding for Edit.Delete (the
    // registry's chord field is a single key+mods, so we route the
    // second key here to avoid duplicating the command entry).
    if (!ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        if (const SEditorCommand* del = EditorCommands::Find("Edit.Delete"))
            if (del->execute && (!del->is_enabled || del->is_enabled()))
                del->execute();
    }
    ImGuizmo::BeginFrame();

    // Push the current selection (as map indices -- the universal id
    // backing every TSafeRef<T>) to the map renderer. Submit OR's
    // kObjFlagSelected into each affected drawable's obj_id so the
    // lit shader can outline; multi-select gets it for free.
    if (TMapRenderer* mr = PlayScreen.MapRenderer()) {
        mr->SetSelectedMapIndices(g_selection);
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

    // End-of-frame: snapshot any objects edited this frame that didn't
    // pair with an explicit Lock/Unlock cycle (Combo selection, single-
    // click checkboxes, etc.) and push them as one undo entry per
    // changed object. No-op if the resolver is locked (mid-drag).
    EditorUndo::ResolveDirty();
}
