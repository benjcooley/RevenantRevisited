# Frame Pipeline

Status: **partial — see "Migration State" below for what's wired and what's still on the legacy path.**

## Motivation

The 1998 engine routed everything through one function — `TScreen::TimerTick(bool draw)` — which catches up pending simulation pulses, then runs `Animate(draw)` to draw the frame. The pre-port codebase further conflated this with a CPU 2D backbuffer that screens blitted into directly via `Display.Put()`.

When the GPU compositor came in, the 2D backbuffer was kept around (as a `TSurface` wrapping a sokol image) but **nothing in the production path opens a sokol pass on it.** The result: `Display.Put` calls — mouse cursor, HUD overlays, status bars — silently no-op in `PlayScreen` because they're trying to render into an inactive pass. The presentation step in `TDisplay::FlipPage` made it worse by gating "composite 2D backbuffer" on `!presented_3d`, so even if 2D drew successfully, it never reached the screen when 3D was active.

This document defines the intended frame pipeline. The implementation is being migrated in stages — the entry points and layering are in place, individual screens move over as they're touched.

## Concepts

A frame has two independent driver paths:

- **Tick**  — advances the simulation by a fixed number of steps. Owns gameplay state, AI, physics, area transitions, animation logic. Does not draw.
- **Draw**  — renders one frame. May skip simulation steps (or run several since the last draw) without affecting state.

These can run at different cadences. A pause freezes Tick but not Draw. A headless test can run Tick without Draw. A locked 60 Hz Draw can pull from a 24 Hz Tick with interpolation.

The frame is composed from **layers** stacked back-to-front:

1. **Scene3D** — the world (`TMapRenderer::RenderFrame` → `TRenderer::PresentToSwapchain`). Opaque.
2. **Overlay2D** — HUD, cursor, debug widgets, anything legacy-`Display.Put` writes. Transparent over Scene3D.
3. **DebugUI** — ImGui debug panels (`simgui_render`). Transparent over Overlay2D.

`TDisplay::FlipPage` composites these in order onto the swapchain.

## Surfaces

### `TScreen` — screen lifecycle

```cpp
class TScreen {
public:
    virtual void Tick();       // advance simulation (catch up missed legacy frames)
    virtual void DrawFrame();  // open Overlay2D pass, run screen's Animate, end pass
    virtual void Animate(bool draw);  // legacy hook — concrete screens override for their draw work
    virtual void Pulse();             // legacy hook — concrete screens override for sim work

    // Deprecated. Calls Tick() + (draw ? DrawFrame() : nothing).
    // Removed once every call site is migrated.
    [[deprecated]] virtual bool TimerTick(bool draw);
};
```

`Tick()` runs the existing per-legacy-frame `Pulse()` catch-up loop. `DrawFrame()` is **two-phase**:

1. **Animate phase** — no sokol pass open. `Animate(true)` runs; screens that own 3D rendering (e.g. `TPlayScreen` calling `TMapRenderer::RenderFrame`) open and close their own offscreen sokol passes here. Required because sokol enforces "one active pass at a time," and the 3D scene needs multiple sequential offscreen passes (G-buffer, light, etc.).
2. **Overlay phase** — `BeginOverlay()` opens a sokol pass on the backbuffer; `DrawBackground()` (pane backdrops, drawn on top of 3D) and `DrawMouseCursor()` run; `EndOverlay()` closes the pass.

Screens customize behavior through `Pulse()` (sim) and `Animate()` (3D + screen-owned draw), exactly as before — the new pipeline doesn't force any change at the screen level.

### `TDisplay` — pass management + final composite

```cpp
class TDisplay : public TSurface {
public:
    void BeginOverlay();  // sg_begin_pass on the backbuffer surface, transparent clear
    void EndOverlay();    // sg_end_pass

    bool FlipPage(bool wait = false);  // composite Scene3D → Overlay2D → DebugUI → present
};
```

`BeginOverlay` / `EndOverlay` wrap a sokol render pass that targets the 2D backbuffer surface. While the pass is open, every legacy `Display.Put` / `Display.Blit` / etc. routes into the backbuffer texture. `FlipPage` then composites that texture on top of the 3D scene.

The crucial guarantee: **`Display.Put` calls outside a `BeginOverlay/EndOverlay` pair are a programmer error** (they will silently no-op). Screens are expected to do all 2D drawing from within `DrawFrame()`, which opens the pass for them.

### `AppFrame` — driver

```cpp
static void AppFrame()
{
    ...
    CurrentScreen->Tick();             // sim catch-up
    TObjectComponent::RunUpdateList(); // post-sim component pass

    CurrentScreen->DrawFrame();        // 2D overlay pass on backbuffer
    DebugUI::DrawFrame();              // ImGui frame build

    if (Display.IsActive())
        Display.FlipPage();            // composite + present
    ...
}
```

The `bool draw` parameter that used to gate the legacy `TimerTick` is gone — separation is explicit at the driver.

## Composition order in `FlipPage`

```cpp
sg_begin_default_pass(...);   // clear to black

// Scene3D layer
const bool presented_3d = Renderer->PresentToSwapchain();

// Overlay2D layer — backbuffer texture, alpha-blended on top.
// Always composited; even when 3D wasn't presented (logo screen,
// menus), this is where their content reaches the swapchain.
Renderer->Composite(backbuffer);

// DebugUI layer — ImGui draws directly into the active default pass.
simgui_render();

sg_end_pass();
sg_commit();
```

`Renderer->Composite(TSurface*)` uses an alpha-blended swap pipeline; transparent backbuffer pixels leave the 3D scene visible. (See [renderer.cpp](src/renderer.cpp) `composite_pip_swap`.)

## Migration State

- ✅ `TScreen::Tick` / `TScreen::DrawFrame` exist; default impls bridge into legacy `Pulse()` / `Animate()` so existing screens keep working.
- ✅ `TDisplay::BeginOverlay` / `EndOverlay` wrap sokol pass on backbuffer.
- ✅ `FlipPage` composites Scene3D + Overlay2D + DebugUI in defined order with alpha blend.
- ✅ `AppFrame` calls `Tick()` then `DrawFrame()` separately.
- ⚠️ `TScreen::TimerTick(bool)` retained as deprecated wrapper. Callers in test modes still use it; migrate as touched.
- ⚠️ `TScreen::Animate(bool draw)` keeps its `draw` parameter for now. The parameter is always `true` under the new pipeline. Drop the parameter when convenient.
- ⚠️ Individual screens (`TPlayScreen`, `TLogoScreen`, …) override `Animate` and `Pulse`. They do **not** need to override `Tick` / `DrawFrame` unless they want fully custom pipeline behavior.

## Rules for new code

- 2D draws (`Display.Put`, `Display.Blit`, `Display.Line`, …) belong inside a screen's `Animate()` (or anything reachable from `DrawFrame()`). Anywhere else is a bug.
- Game logic and state mutation belongs in `Pulse()` (reached from `Tick`). Don't read input or mutate state in `Animate()`.
- 3D rendering is owned by `TMapRenderer` / `TRenderer`; screens submit by calling those, not by issuing sokol calls directly.
- Debug UI lives in ImGui via `DebugUI::DrawFrame()` and is the final composite layer — visible over both Scene3D and Overlay2D.
- Future work: a formal render-graph (named passes with declared inputs/outputs) when the layer count starts to hurt. Until then this three-layer model is fine.
