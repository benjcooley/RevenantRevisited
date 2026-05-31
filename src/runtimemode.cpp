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
        const uint32_t modemask = CurrentModeMask();
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
        if (!Player) return false;
        switch (button)
        {
          case MB_RIGHTDOWN:
            // Begin walk-to. ApplyWalkCursor computes the world point
            // under the cursor, picks an angle, swaps in the matching
            // wedge bitmap, and asks the Player to walk that way.
            ApplyWalkCursor(x, y);
            walking = true;
            return true;

          case MB_RIGHTUP:
            // Stop walking: release the synthesized joystick direction so
            // UpdateMove sees no direction next tick and stops the player.
            // Revert to the default cursor.
            if (walking)
            {
                ReleaseMouseWalkKey();
                walking = false;
                if (GameData)
                    if (PTBitmap cursor = GameData->Bitmap("cursor"))
                        SetMouseBitmap(cursor);
            }
            return true;

          case MB_LEFTDOWN:
            // Combat mode: random swing. Other modes (bow, hover-to-
            // interact, inventory drag) are Phase 2 of mouse-loop work
            // -- they need object pick via the OBJID buffer and an
            // inventory pane to drop into. The combat-only path is
            // enough to verify the dispatch chain end-to-end now.
            if (Player->IsCombat())
                Player->ButtonAttack(random(1, 3));
            return true;
        }
        return false;
    }

    bool HandleMouseMove(int32_t button, int32_t x, int32_t y) override
    {
        // While the right button is held, follow the cursor with a
        // fresh angle each move. cursorx/cursory are also updated by
        // the sokol MOUSE_MOVE handler, but the click handler stores
        // the right-down state here so we know not to chase the cursor
        // when the player wasn't asking to walk.
        if (walking && Player)
            ApplyWalkCursor(x, y);
        (void)button;
        return false;
    }

  private:
    // Control-mode mask for ControlMap lookups (mirrors the keybind
    // table's CTRL_* in playscreen.cpp). Same value HandleKey and the
    // mouse-walk joystick synthesis both need.
    static uint32_t CurrentModeMask()
    {
        constexpr uint32_t CTRL_NORMALMODE = 1, CTRL_COMBATMODE = 2,
                           CTRL_BOWMODE = 4, CTRL_SNEAKMODE = 8;
        uint32_t m = CTRL_NORMALMODE;
        if (Player)
        {
            if (Player->IsCombat())         m = CTRL_COMBATMODE;
            else if (Player->IsBowMode())   m = CTRL_BOWMODE;
            else if (Player->IsSneakMode()) m = CTRL_SNEAKMODE;
        }
        return m;
    }

    // Release the currently-synthesized mouse-walk joystick direction (if
    // any) through ControlMap, clearing its CMDFLAG so UpdateMove stops
    // driving that direction.
    void ReleaseMouseWalkKey()
    {
        if (mouse_walk_key >= 0)
        {
            ControlMap.GetCommand(mouse_walk_key, false, CurrentModeMask());
            mouse_walk_key = -1;
        }
    }

    // Drive walk-to by synthesizing the matching joystick direction key,
    // exactly as retail's TMapPane::UpdateMouseMovement did. This feeds
    // the SAME ControlMap → cmdflag → UpdateMove path the keyboard uses,
    // so run (R / CMDFLAG_RUN), sneak, block, etc. compose for free and
    // there's a single movement driver. (The previous direct Player->Go
    // bypassed UpdateMove, so R-while-mouse-walking didn't run and the
    // two drivers fought.) Called on right-down and right-button drag.
    void ApplyWalkCursor(int32_t screen_x, int32_t screen_y)
    {
        S3DPoint curpos;
        Player->GetPos(curpos);

        TMapRenderer *mr = PlayScreen.MapRenderer();
        if (!mr) return;

        // Project the cursor pixel onto the player's z-plane so we get
        // a world point at walkable height. zoffset=50 matches the
        // pre-port GetMouseMapPos default.
        S3DPoint target;
        mr->ScreenToWorld(screen_x, screen_y, curpos.z + 50, target);

        // Dead-zone: cursor inside the player's own footprint releases the
        // walk key (stop) and shows the normal cursor.
        if (absval(target.x - curpos.x) < 16 && absval(target.y - curpos.y) < 16)
        {
            ReleaseMouseWalkKey();
            if (GameData)
                if (PTBitmap cursor = GameData->Bitmap("cursor"))
                    SetMouseBitmap(cursor);
            return;
        }

        const int32_t angle = ConvertToFacing(curpos, target);

        // Snap to the nearest 45deg slot (Revenant angle space is 0..255;
        // +0x10 rounds before the mask). dir_idx maps to both the wedge
        // bitmap suffix and the joystick key. Index order matches the
        // legacy Directions[] = { ne, e, se, s, sw, w, nw, n }.
        static const char *kDirections[] =
            { "ne", "e", "se", "s", "sw", "w", "nw", "n" };
        static const int32_t kJoyKeys[] =
            { VK_JOYUPRIGHT, VK_JOYRIGHT, VK_JOYDOWNRIGHT, VK_JOYDOWN,
              VK_JOYDOWNLEFT, VK_JOYLEFT, VK_JOYUPLEFT, VK_JOYUP };
        const int32_t dir_idx = ((angle + 0x10) & 0xe0) >> 5;

        if (GameData)
        {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "wedge-%s", kDirections[dir_idx]);
            if (PTBitmap wedge = GameData->Bitmap(buf))
                SetMouseBitmap(wedge);

            std::snprintf(buf, sizeof(buf), "wedge-%sshadow", kDirections[dir_idx]);
            if (PTBitmap shadow = GameData->Bitmap(buf))
                SetMouseShadow(shadow, 0, 43);
        }

        // Swap the synthesized joystick direction if it changed. UpdateMove
        // reads the resulting CMDFLAG every tick and issues Player->Go with
        // whatever mode (walk/run/sneak) is active.
        const int32_t joykey = kJoyKeys[dir_idx];
        if (joykey != mouse_walk_key)
        {
            const uint32_t mode = CurrentModeMask();
            if (mouse_walk_key >= 0)
                ControlMap.GetCommand(mouse_walk_key, false, mode);
            ControlMap.GetCommand(joykey, true, mode);
            mouse_walk_key = joykey;
        }
    }

    bool    walking = false;
    int32_t mouse_walk_key = -1;   // synthesized joystick dir key, -1 = none
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
