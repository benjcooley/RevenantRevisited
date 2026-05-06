// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               runtimemode.cpp - game / editor mode hooks              *
// *************************************************************************

#include "runtimemode.h"

#include "ctrlmap.h"
#include "logging.h"
#include "mappane.h"
#include "maprenderer.h"
#include "object.h"
#include "player.h"
#include "playscreen.h"
#include "revenant.h"

namespace
{

class TGameModeImpl final : public IRuntimeMode
{
  public:
    const char* Name() const override { return "game"; }

    void OnEnter() override { log_info("[runtimemode] enter game mode"); }
    void OnExit()  override { log_info("[runtimemode] exit game mode");  }

    void Tick() override
    {
        // Drive player movement from the latest command-flag state.
        // Mouse-click walk-to is the primary input path; keyboard
        // arrows are the secondary fallback. UpdateMove translates
        // CMDFLAG bits into Player->Go / Stop.
        if (PlayScreen.IsControlOn() && !PlayScreen.IsDemoMode())
            PlayScreen.UpdateMove();

        // Game-logic tick over the active 3x3 window centered on the
        // player. UpdateActiveWindow re-fills MapPane.sectors[][];
        // PulseObjects runs per-instance Pulse() (AI / animator state);
        // MoveObjects applies the movement step from movebits set during
        // Pulse.
        MapPane.UpdateActiveWindow();
        MapPane.PulseObjects();
        MapPane.MoveObjects();

        // Camera follow: anchor the renderer's camera on the player.
        // The camera is a real entity in world space -- it tracks
        // Locke's full position (including Z). Architectural rule:
        // the camera depends on objects (it follows the player), but
        // no object's rendered position is allowed to depend on the
        // camera. Object screen positions are computed from absolute
        // world coords; camera position only shifts the screen-space
        // viewport origin.
        if (Player && PlayScreen.MapRenderer())
        {
            S3DPoint p;
            Player->GetPos(p);
            PlayScreen.MapRenderer()->SetCameraWorld(
                Player->GetLevel(), p.x, p.y, p.z);
        }
    }

    bool HandleKey(int32_t key, bool down) override
    {
        // Pump the global ControlMap so cmdflagstate reflects what's
        // currently held. UpdateMove reads that state on the next tick.
        // GetCommand returns the GAMECMD_* fired (down/up edge); we
        // don't dispatch it here -- direction commands are read off
        // the flag state in UpdateMove. Returning false lets the rest
        // of the pane stack also see the key (for non-movement
        // accelerators).
        ControlMap.GetCommand(key, down);
        return false;
    }

    bool HandleMouseClick(int32_t /*button*/, int32_t /*x*/, int32_t /*y*/) override
    {
        // Game-mode click (walk-to / pick / interact) is not wired yet.
        // Importantly we do NOT forward to TMapRenderer::HandleMouseClick
        // here -- that path is editor gizmo / drag logic.
        return false;
    }
};

class TEditorModeImpl final : public IRuntimeMode
{
  public:
    const char* Name() const override { return "editor"; }

    void OnEnter() override { log_info("[runtimemode] enter editor mode"); }
    void OnExit()  override { log_info("[runtimemode] exit editor mode");  }

    void Tick() override
    {
        // Editor owns its own pulse / camera (Play button + viewport
        // controls). Nothing to drive from the screen layer.
    }

    bool HandleMouseClick(int32_t button, int32_t x, int32_t y) override
    {
        // Editor mouse: forward into the renderer's gizmo / drag picker.
        if (PlayScreen.MapRenderer())
        {
            PlayScreen.MapRenderer()->HandleMouseClick(button, x, y);
            return true;
        }
        return false;
    }
};

TGameModeImpl   g_gameMode;
TEditorModeImpl g_editorMode;
IRuntimeMode*   g_currentMode = &g_gameMode;

} // namespace

IRuntimeMode* CurrentMode() { return g_currentMode; }

void SetCurrentMode(IRuntimeMode* mode)
{
    if (mode == g_currentMode) return;
    if (g_currentMode) g_currentMode->OnExit();
    g_currentMode = mode;
    if (g_currentMode) g_currentMode->OnEnter();
}

IRuntimeMode* GameMode()   { return &g_gameMode;   }
IRuntimeMode* EditorMode() { return &g_editorMode; }

bool EditorOverlaysEnabled() { return g_currentMode == &g_editorMode; }
