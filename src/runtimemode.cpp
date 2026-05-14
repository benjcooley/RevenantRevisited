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
