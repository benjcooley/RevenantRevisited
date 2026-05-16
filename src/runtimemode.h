// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               runtimemode.h - top-level runtime mode hook             *
// *************************************************************************
//
// Encapsulates the per-frame + input behavior that differs between
// "playing the game" and "running the editor". TPlayScreen forwards its
// per-frame work and raw input through CurrentMode(), so swapping the
// mode pointer (in StartEditor / ShutDownEditor) is what flips behavior.
//
// Game mode: ticks the active sector window, runs Pulse/Move on objects,
// drives camera follow on the Player.
// Editor mode: does none of that -- the ImGui editor controls camera +
// selection. World pulse/move only runs when the editor's Play button is
// pressed (handled inside the editor itself, not here).
//
// *************************************************************************

#pragma once

#include <cstdint>

class IRuntimeMode
{
  public:
    virtual ~IRuntimeMode() = default;

    virtual const char* Name() const = 0;

    // Enter / exit transitions. Called by SetCurrentMode().
    virtual void OnEnter() {}
    virtual void OnExit()  {}

    // Per-frame logic. Camera follow, world pulse/move, etc. Editor mode
    // is a no-op here.
    virtual void Tick() {}

    // Raw input. Return true if the event was consumed by the mode and
    // should NOT fall through to the screen's default handling.
    virtual bool HandleKey       (int32_t /*key*/, bool /*down*/)                  { return false; }
    virtual bool HandleMouseClick(int32_t /*button*/, int32_t /*x*/, int32_t /*y*/) { return false; }
    virtual bool HandleMouseMove (int32_t /*button*/, int32_t /*x*/, int32_t /*y*/) { return false; }

    // 2D drawing for the mode happens via Renderer->AddHudItem in
    // OnEnter and Renderer->RemoveHudItem in OnExit. No per-frame
    // drawing hook on the mode -- the renderer iterates registered
    // HUD items each frame in z-order. See docs/FRAME_PIPELINE.md.
};

// Returns the currently active mode. Never null after engine init -- the
// game mode is installed at startup.
IRuntimeMode* CurrentMode();

// Swap the active mode. Calls OnExit() on the outgoing mode and
// OnEnter() on the incoming one. Pass nullptr to clear (rare).
void SetCurrentMode(IRuntimeMode* mode);

// Borrowed pointers to the two built-in modes.
IRuntimeMode* GameMode();
IRuntimeMode* EditorMode();

// Convenience: true when the editor mode is active and editor-only
// visuals (selection highlights, light/helper gizmos, debug labels,
// etc.) should be drawn. Renderer + debug overlay code gate decoration
// passes on this so game mode never shows editor decorations even when
// the underlying toggles (sectorShowGizmos, sectorShowObjectLabels, ...)
// happen to be left enabled.
bool EditorOverlaysEnabled();
