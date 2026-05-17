// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               runtimemode.cpp - game / editor mode hooks              *
// *************************************************************************

#include "runtimemode.h"

#include "ctrlmap.h"
#include "cursor.h"
#include "platform/cursor.h"
#include "imgui.h"
#include "logging.h"
#include "mappane.h"
#include "maprenderer.h"
#include "multi.h"
#include "object.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "revenant.h"
#include "revtypes.h"

#include <sokol_app.h>

namespace
{

class TGameModeImpl final : public IRuntimeMode
{
  public:
    const char* Name() const override { return "game"; }

    void OnEnter() override
    {
        log_info("[runtimemode] enter game mode");
        // Seed the default game cursor. In windowed mode SetMouseBitmap
        // pushes the pixels to AppKit's NSCursor so the OS handles
        // visibility for us (in/out of window, focus changes, ...).
        // In fullscreen there's no "outside" for the OS cursor, and
        // AppKit hides the cursor over fullscreen apps -- so we hide
        // sokol_app's cursor and let TCursorHud draw the cursor in-game
        // like the pre-OS-cursor path. SetMouseBitmap itself decides
        // (Windowed gate) whether to take the OS path; we just suppress
        // the OS arrow when it won't.
        if (!Windowed)
            sapp_show_mouse(false);
        if (GameData)
        {
            if (PTBitmap cursor = GameData->Bitmap("cursor"))
                SetMouseBitmap(cursor);
            else
                log_warn("[runtimemode] GameData->Bitmap(\"cursor\") returned null");
        }

        // Register the cursor HUD drawable. It no longer renders the
        // main cursor pixel (OS owns that); it still composites the
        // drag bitmap + corner-bitmap overlay on top.
        if (Renderer)
            Renderer->AddHud(&cursor_hud, /*z=*/0.0f);
    }
    void OnExit() override
    {
        log_info("[runtimemode] exit game mode");
        if (Renderer)
            Renderer->RemoveHud(&cursor_hud);
        // Hand the cursor back to the system default. In windowed mode
        // that's the OS arrow via NSCursor; in fullscreen we re-show
        // sokol_app's cursor (we hid it in OnEnter) so the next mode
        // isn't left with an invisible pointer.
        if (Windowed)
            rev_platform::ResetOSCursor();
        else
            sapp_show_mouse(true);
    }

  private:
    TCursorHud cursor_hud;
    bool       imgui_had_mouse_last_tick = false;  // edge-trigger for OS cursor swap

    void Tick() override
    {
        // While ImGui wants the mouse (debug panels, menus, modal
        // popups) reset the OS cursor to the platform arrow so panel
        // hovers / resizes feel native. When focus returns to the
        // playfield, re-push the current game cursor. TCursorHud::Draw
        // mirrors the same predicate to suppress drag/corner overlays
        // during ImGui ownership.
        const bool imgui_owns_mouse = ImGui::GetIO().WantCaptureMouse;
        if (imgui_owns_mouse != imgui_had_mouse_last_tick)
        {
            if (imgui_owns_mouse)
                rev_platform::ResetOSCursor();
            else
                RefreshOSCursor();
            imgui_had_mouse_last_tick = imgui_owns_mouse;
        }
        // Re-assertion to defeat AppKit's auto-revert across tracking-
        // area / view / title-bar crossings is driven from the
        // SAPP_EVENTTYPE_MOUSE_MOVE / MOUSE_ENTER handlers in revmain
        // -- those only fire while the cursor is INSIDE our window, so
        // we don't fight AppKit when the mouse legitimately belongs to
        // another app. Don't re-assert from the tick path -- that
        // forces our cursor on top of the system arrow over the
        // desktop / other apps.

        // Drive player movement from the latest command-flag state.
        // Mouse-click walk-to is the primary input path; keyboard
        // arrows are the secondary fallback. UpdateMove translates
        // CMDFLAG bits into Player->Go / Stop.
        if (PlayScreen.IsControlOn() && !PlayScreen.IsDemoMode())
            PlayScreen.UpdateMove();

        // Game-logic tick over the active 3x3 window centered on the
        // player. UpdateActiveWindow re-fills MapPane.sectors[][];
        // NextFrameObjects owns the authoritative 24 Hz animation frame
        // advance; PulseObjects runs per-instance AI/state; MoveObjects
        // applies movement from movebits set during Pulse.
        MapPane.UpdateActiveWindow();
        MapPane.NextFrameObjects();
        MapPane.PulseObjects();
        MapPane.MoveObjects();
    }

    bool HandleKey(int32_t key, bool down) override
    {
      // Pump the global ControlMap so cmdflagstate reflects what's
      // currently held. UpdateMove polls that state every tick to drive
      // directional movement (forward/back/strafe). For edge-triggered
      // commands (attacks, combos, mode toggles, dodge, jump, block) we
      // dispatch the returned GAMECMD_* into the player here.
      //
      // Same-key bindings disambiguate by the current control mode:
      // 'S' is "sneak-back" in normal/sneak mode and "thrust" in combat
      // mode; 'D' is "inventory-drop" in inventory mode and "chop" in
      // combat mode. ControlMap.GetCommand returns the *first* binding
      // whose mode mask intersects modemask, so we have to pass exactly
      // the active mode — ALLMODES would always pick the earlier entry.
      // CTRL_* constants mirror those in playscreen.cpp's keybind table.
        constexpr uint32_t CTRL_NORMALMODE    = 1;
        constexpr uint32_t CTRL_COMBATMODE    = 2;
        constexpr uint32_t CTRL_BOWMODE       = 4;
        constexpr uint32_t CTRL_SNEAKMODE     = 8;
        constexpr uint32_t CTRL_INVENTORYMODE = 16;
        uint32_t modemask = CTRL_NORMALMODE;
        if (Player)
        {
            if (Player->IsCombat())        modemask = CTRL_COMBATMODE;
            else if (Player->IsBowMode())  modemask = CTRL_BOWMODE;
            else if (Player->IsSneakMode())modemask = CTRL_SNEAKMODE;
            // Inventory-mode is UI-driven (not a TCharacter mode); leave it
            // unset for now — combat demos don't open the inventory pane.
        }
        int32_t cmd = ControlMap.GetCommand(key, down, modemask);

        if (Player && PlayScreen.IsControlOn() && !PlayScreen.IsDemoMode() &&
            cmd != GAMECMD_NONE)
        {
            if (down)
            {
                switch (cmd)
                {
                  // Combat mode toggle — required before swings register
                  // (IsValidAttack rejects unless IsCombat()).
                    case GAMECMD_COMBAT:
                        if (Player->IsCombat()) Player->EndCombat();
                        else                    Player->BeginCombat();
                        break;

                  // Three primary attack buttons.
                    case GAMECMD_SWING:  {
                        bool r = Player->Swing();
                        log_info("[input] SWING -> ButtonAttack(1) returned %d", r ? 1 : 0);
                        break;
                    }
                    case GAMECMD_THRUST: {
                        bool r = Player->Thrust();
                        log_info("[input] THRUST -> ButtonAttack(2) returned %d", r ? 1 : 0);
                        break;
                    }
                    case GAMECMD_CHOP:   {
                        bool r = Player->Chop();
                        log_info("[input] CHOP -> ButtonAttack(3) returned %d", r ? 1 : 0);
                        break;
                    }

                  // 12 combo slots (Ctrl+A-H, Shift+A-H).
                    case GAMECMD_COMBO1: case GAMECMD_COMBO2: case GAMECMD_COMBO3:
                    case GAMECMD_COMBO4: case GAMECMD_COMBO5: case GAMECMD_COMBO6:
                    case GAMECMD_COMBO7: case GAMECMD_COMBO8: case GAMECMD_COMBO9:
                    case GAMECMD_COMBO10: case GAMECMD_COMBO11: case GAMECMD_COMBO12:
                        Player->Combo((cmd - GAMECMD_COMBO1) + 1);
                        break;

                    case GAMECMD_DODGE:    Player->Dodge();   break;
                    case GAMECMD_JUMP:     Player->Jump();    break;
                    case GAMECMD_BLOCKDOWN: Player->Block();   break;

                    default: break;
                }
            }
            else
            {
                switch (cmd)
                {
                    case GAMECMD_BLOCKUP: Player->StopBlock(); break;
                    default: break;
                }
            }
        }

      // Returning false lets the rest of the pane stack also see the
      // key (for non-game accelerators like F-keys, dev overlays).
        return false;
    }

    bool HandleMouseClick(int32_t button, int32_t x, int32_t y) override
    {
        // TODO(input): retail MapPane.MouseClick is unsafe under the new
        // renderer-owned-sectors architecture -- it touches pane state
        // (posx/posy/scroll), Notify-iterates sectors that MapPane no
        // longer owns, and synthesizes fake joystick keys. Calling it
        // directly crashes on right-click.
        //
        // Right path: extract the gameplay-only subset (right-down ->
        // start move, right-up -> stop move, left-down -> attack
        // request) into a thin GameInput layer that touches Player +
        // ControlMap without needing pane state. For now just log so
        // we can confirm events arrive.
        log_info("[gameinput] click button=%d at (%d,%d) -- not wired", button, x, y);
        (void)button; (void)x; (void)y;
        return false;
    }

    bool HandleMouseMove(int32_t button, int32_t x, int32_t y) override
    {
        // Same TODO as HandleMouseClick. The wedge-cursor / bow-aim
        // logic in MapPane.MouseMove depends on uninitialized pane
        // state; revisit when we have a GameInput layer.
        (void)button; (void)x; (void)y;
        return false;
    }
};

class TEditorModeImpl final : public IRuntimeMode
{
  public:
    const char* Name() const override { return "editor"; }

    void OnEnter() override
    {
        log_info("[runtimemode] enter editor mode");
        // Editor uses the platform arrow (with editor tool cursors
        // swapped in via SetMouseBitmap when a specific tool wants
        // one). Reset to the default arrow explicitly so the editor's
        // entry state is independent of whatever game mode left.
        rev_platform::ResetOSCursor();

        // Restore the editor's own camera. Otherwise we'd inherit
        // whatever camera position game mode last set (= Locke's last
        // location), which is jarring -- the editor's view should stay
        // wherever the user last left it. First entry of a session
        // skips the restore so the renderer's natural startup centering
        // (BuildAndCenter -> startup_sector pick) is preserved.
        if (cam_saved)
        {
            if (TMapRenderer* mr = PlayScreen.MapRenderer())
                mr->SetCameraWorld(cam_level, cam_world.x, cam_world.y, cam_world.z);
        }
    }
    void OnExit() override
    {
        log_info("[runtimemode] exit editor mode");
        // Snapshot the editor camera so it survives the round-trip
        // through game mode (which retargets the camera to Locke every
        // frame in TPlayScreen::RenderFrame).
        if (TMapRenderer* mr = PlayScreen.MapRenderer())
        {
            cam_world = mr->CameraWorld();
            cam_level = mr->CameraLevel();
            cam_saved = true;
        }
    }

  private:
    S3DPoint cam_world  = {0, 0, 0};
    int32_t  cam_level  = 0;
    bool     cam_saved  = false;

  public:

    // Editor mode owns no game-mode HUD overlay. Editor-specific
    // overlays (gizmos, selection outlines) draw via debugui / the
    // renderer's editor decoration passes, gated on
    // EditorOverlaysEnabled().

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
