// Revenant Revisited port - the debug panel's "Sprites" tab.
// See spritedebugtab.h, spritetexoverride.h (3D model textures) and
// tiletreload.h (the location's tile art).

#include "spritedebugtab.h"

#include "imgui.h"
#include "logging.h"
#include "revisited_settings.h"
#include "spritetexoverride.h"
#include "tiletexoverride.h"
#include "tiletreload.h"

#include <chrono>
#include <cstring>
#include <string>

namespace {

// The reloads the panel asked for. Done after the widgets, so a mode change
// shows the new art in the same frame.
bool     g_reload_pending = false;
int32_t  g_pending_mode = 0;     // the mode the panel asks for
int32_t  g_last_frames = 0;
int32_t  g_last_images = 0;
double   g_last_ms = 0.0;

// The tile art's reloads (the location's art), same pattern.
bool     g_tiles_pending = false;
int32_t  g_tiles_mode = 0;
int32_t  g_last_tiles_visited = 0;
int32_t  g_last_tiles_updated = 0;
double   g_tiles_ms = 0.0;

// The art root boxes. Initialized from the session's settings once (the
// ini value at boot, later reloads never rewrite them); "Use this folder"
// pushes the typed path into the session and reloads. Nothing is written
// to Revenant.ini until the debug panel's own Save button.
char    g_tile_root_buf[512] = "";
bool    g_tile_root_buf_ready = false;
char    g_sprite_root_buf[512] = "";
bool    g_sprite_root_buf_ready = false;

void CopyRootToBuf(char* buf, size_t size, bool& ready, const std::string& value)
{
    if (ready)
        return;
    ready = true;
    const size_t n = value.size() < size - 1 ? value.size() : size - 1;
    if (n)
        std::memcpy(buf, value.data(), n);
    buf[n] = '\0';
}

// What the user typed -> the setting's std::string: Explorer pastes often
// carry surrounding quotes, and stray spaces around the path are noise.
std::string TrimmedPath(const char* buf)
{
    std::string s = buf ? buf : "";
    while (s.size() >= 2 && s.front() == '"' && s.back() == '"')
    {
        s.erase(s.begin());
        s.pop_back();
    }
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t'))
        s.erase(s.begin());
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t'))
        s.pop_back();
    return s;
}

} // namespace

// One section of the tab: the location's tile art (the SpriteReload-style
// path the "reloadtiles" console command also takes, so the panel and the
// console can not drift apart).
static void DrawTileArtSection()
{
    const bool modern = (RevisitedSettings.high_res_tile_mode != 0);
    if (ImGui::RadioButton("Retail", !modern) && modern)
    {
        g_tiles_mode = 0;
        g_tiles_pending = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Modern", modern) && !modern)
    {
        g_tiles_mode = 1;
        g_tiles_pending = true;
    }
    ImGui::Separator();

    ImGui::Text("tiles the renderer holds: %d", TileArtReload::TileCount());
    if (!TileArtReload::Enabled())
        ImGui::TextDisabled("no tile art tree -- set HighResTileRoot in Revenant.ini");

    // Where the tile art comes from. Empty keeps the configured default
    // (<overlay>/Imagery, or the ini's HighResTileRoot).
    CopyRootToBuf(g_tile_root_buf, sizeof(g_tile_root_buf), g_tile_root_buf_ready,
                  RevisitedSettings.high_res_tile_root);
    ImGui::TextUnformatted("art root:");
    ImGui::SameLine();
    ImGui::TextDisabled("%s", TileTexOverride::Root()[0] ? TileTexOverride::Root() : "<default>");
    ImGui::PushItemWidth(-120.0f);
    ImGui::InputText("##tileroot", g_tile_root_buf, sizeof(g_tile_root_buf));
    ImGui::PopItemWidth();
    if (ImGui::Button("Use this folder##tileapply"))
    {
        RevisitedSettings.high_res_tile_root = TrimmedPath(g_tile_root_buf);
        log_info("[tiletex] panel art root: %s",
                 RevisitedSettings.high_res_tile_root.empty() ? "<default>"
                                                              : RevisitedSettings.high_res_tile_root.c_str());
        g_tiles_mode = RevisitedSettings.high_res_tile_mode;
        g_tiles_pending = true;   // the reload re-indexes the new root first
    }
    ImGui::Separator();

    if (ImGui::Button("Reload tiles"))
    {
        g_tiles_mode = RevisitedSettings.high_res_tile_mode;
        g_tiles_pending = true;
    }

    if (g_last_tiles_visited > 0 || g_last_tiles_updated > 0)
    {
        ImGui::Text("last reload: %d of %d tile(s), %.1f ms",
                    g_last_tiles_updated, g_last_tiles_visited, g_tiles_ms);
    }

    if (!g_tiles_pending)
        return;
    g_tiles_pending = false;
    const auto started = std::chrono::steady_clock::now();
    int32_t visited = 0;
    g_last_tiles_updated = TileArtReload::ReloadAll(g_tiles_mode, &visited);
    g_last_tiles_visited = visited;
    g_tiles_ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count();
    log_info("[tiletex] panel reloaded %d of %d tile(s) in %.1f ms",
             g_last_tiles_updated, visited, g_tiles_ms);
}

// One section of the tab: the 3D models' textures (characters, monsters,
// props from .I3D).
static void DrawModelArtSection()
{
    const bool modern = (RevisitedSettings.sprite_mode != 0);
    // Scoped ID: the tiles section already uses the bare labels, and two
    // groups must not share IDs in one window.
    ImGui::PushID("modelart");
    if (ImGui::RadioButton("Retail", !modern) && modern)
    {
        g_pending_mode = 0;
        g_reload_pending = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Modern", modern) && !modern)
    {
        g_pending_mode = 1;
        g_reload_pending = true;
    }
    ImGui::PopID();
    ImGui::Separator();

    ImGui::Text("art tree: %s", SpriteTexOverride::Root());
    ImGui::SameLine();
    ImGui::TextDisabled("(%d)", SpriteTexOverride::Count());
    if (!SpriteTexOverride::Enabled())
    {
        ImGui::TextDisabled("no sprite art tree -- set SpriteRoot in Revenant.ini");
        ImGui::TextDisabled("layout: <root>/<subdir>/<name>/texture_<NN>_frame_<NN>.png");
    }

    // Where the model textures come from. Empty keeps the configured
    // default (<overlay>/Sprites, or <tile root>/sprites).
    CopyRootToBuf(g_sprite_root_buf, sizeof(g_sprite_root_buf), g_sprite_root_buf_ready,
                  RevisitedSettings.sprite_root);
    ImGui::TextUnformatted("art root:");
    ImGui::PushItemWidth(-120.0f);
    ImGui::InputText("##spriteroot", g_sprite_root_buf, sizeof(g_sprite_root_buf));
    ImGui::PopItemWidth();
    if (ImGui::Button("Use this folder##spriteapply"))
    {
        RevisitedSettings.sprite_root = TrimmedPath(g_sprite_root_buf);
        log_info("[spriteart] panel art root: %s",
                 RevisitedSettings.sprite_root.empty() ? "<default>"
                                                       : RevisitedSettings.sprite_root.c_str());
        g_pending_mode = RevisitedSettings.sprite_mode;
        g_reload_pending = true;  // the reload re-indexes the new root first
    }

    if (ImGui::Button("Reload sprites"))
    {
        g_pending_mode = RevisitedSettings.sprite_mode;
        g_reload_pending = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reindex tree"))
    {
        SpriteTexOverride::Init();
        g_pending_mode = RevisitedSettings.sprite_mode;
        g_reload_pending = true;
    }

    if (g_last_frames > 0)
    {
        ImGui::Text("last reload: %d frame(s) in %d image(s), %.1f ms",
                    g_last_frames, g_last_images, g_last_ms);
    }

    if (!g_reload_pending)
        return;

    g_reload_pending = false;
    // One path for the mode switch and the reload: the same one the console
    // command "reloadsprites [retail|modern]" takes, so the panel and the
    // console can not drift apart. It re-indexes the tree (files may have
    // been added since the last index) and then puts the wanted art into
    // the textures the game already uses.
    const auto started = std::chrono::steady_clock::now();
    int32_t images = 0;
    g_last_frames = SpriteReload::ApplyMode(g_pending_mode, &images);
    g_last_images = images;
    g_last_ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count();
    log_info("[spriteart] reloaded %d sprite texture frame(s) in %d image(s) in %.1f ms",
             g_last_frames, images, g_last_ms);
}

void DrawSpritesTab()
{
    // The location's tile art first: that is the picture the player edits
    // and wants to see change in the game.
    if (ImGui::CollapsingHeader("Location art (tiles)", ImGuiTreeNodeFlags_DefaultOpen))
        DrawTileArtSection();
    if (ImGui::CollapsingHeader("Model textures (3D)", ImGuiTreeNodeFlags_DefaultOpen))
        DrawModelArtSection();
}
