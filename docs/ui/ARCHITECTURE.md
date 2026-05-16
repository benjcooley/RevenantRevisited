# UI Architecture — What We Evolve

Read [README.md](README.md) first. This document is short on purpose: most of the architecture is **already built**, and the UI effort's job is to use it and evolve a handful of existing classes.

## 1. Position in the engine

The UI lives between the existing pane framework and the renderer/resource layers below it. None of those lower layers need rewriting for this work — they need consuming correctly.

```
┌──────────────────────────────────────────────────────────────────────┐
│  Screens — TPlayScreen (HUD)  +  per-OOG-screen TScreen subclasses   │
├──────────────────────────────────────────────────────────────────────┤
│  Pane tree — TScreen → TPane → TPane (retained-mode hierarchy)       │
│              Concrete panes: TInventory, TStatusBar, TStatPane,      │
│              TAutoMap, TQuickSpellPane, TDialogPane, TMultiCtrlPane, │
│              TEquipPane, TSpellPane, TTextBar, TCursorHud, ...       │
├──────────────────────────────────────────────────────────────────────┤
│  Existing facilities (use as-is — do NOT re-create):                 │
│   • rev_fopen / MountArchive / rev_resolve_revisited_overlay         │
│   • LoadResource* / TMulti                                           │
│   • defdoc::Document                                                 │
│   • TDialogList + Language (localized strings)                       │
│   • INI API (game settings + [Revisited] toggles in Revenant.ini)    │
│   • EditorPrefs (.def-backed editor state — model for any other      │
│                  .def-backed pref store we might add)                │
│   • TFontTable / TFont / TTF atlas (bitmap + TT already coexist)     │
│   • TRenderer::Composite / Submit* (the HUD composites here)         │
│   • src/testmodes.cpp + src/testscreen.cpp (test mode harness)       │
└──────────────────────────────────────────────────────────────────────┘
```

## 2. What we evolve (and why)

### 2.1 `TPane` → retained-mode hierarchy + layout + 9-slice + style
The pane system is primitive even by 1998 standards (flat lists per `TScreen`, no real layout, immediate-mode draw). Evolution targets — applied to existing `TPane` in [src/screen.h](../../src/screen.h). Reference points: Unity's IMGUI vertical/horizontal layouts, ImGui's `ImGuiStyle` push/pop. **Rudimentary on purpose** — see memory `project-ui-layout-system` for the "what NOT to build" list.

**Hierarchy:**
- **Explicit parent + children:** `TPane* parent`, an owning children array (form consistent with existing T/S/P conventions). Parents own children's lifetime.
- **Dirty propagation:** children mark dirty up the tree; layout/draw runs top-down on dirty subtrees. Use the monotonic counter pattern from memory `feedback-versions-over-flags` if dirty tracking benefits from version comparison rather than a boolean.

**Layout — 2-pass measure → layout:**
- **Pass 1 (measure, bottom-up):** each node reports preferred (and optionally minimum) size given a parent constraint.
- **Pass 2 (layout, top-down):** parent assigns final rects to children based on container kind + per-child sizing policy.
- **Containers:** vertical and horizontal layouts (like Unity's `VerticalLayoutGroup`/`HorizontalLayoutGroup`; or a single-axis flex). Most retail HUD is just fixed-positioned children of a screen, so containers are opt-in per parent, not mandatory.
- **Per-child sizing policy:** `fixed` (use measured preferred size) and `greedy` (consume remaining space, weighted if multiple greedy siblings).
- **Margins + padding:** outer margin around child, inner padding inside parent. Standard CSS naming.
- **Anchors:** absolute-positioned panes anchor to parent edges (`top-left` / `top-right` / `bottom-center` / `center` / etc.) and pin native size. Anchored layout coexists with flex containers — retail HUD uses anchors against canvas edges; nested toolbars / lists inside an OOG screen use containers.

**Vanilla path is a no-op:** at 640×480 the retail DEF coordinates pass through unchanged; layout passes degenerate to "use the explicit rect." Anchored / flex layout activates only when a non-retail canvas size is in use or a parent declares a container kind.

**9-slice rendering:** built proper as part of this work (no game-side 9-slice exists in `src/` today — only `ImGuiStyle`-based styling in the editor). Components that use a stretchable background (buttons, panels, dialog frames) reference a 9-slice resource: bitmap + 4 inset values (left / right / top / bottom). Corners render verbatim, edges tile or stretch, center tiles or fills. The retail widget atlases (`widgetstex.dat`, `medgold.dat`, etc.) are the source 9-slice atoms.

**Scrolling + clip boxes:**
- **Clip rect lives on `TPane` base** — any pane can declare a clip rectangle; children render within that bound. The renderer applies scissor / stencil at composite time. Useful outside scroll contexts too (clipping a text label to a fixed-width slot).
- **Scrolling evolves the existing [`TScrollPane`](../../src/scroll.h)** (per `feedback-evolve-dont-replace` — no new `ScrollView`). Viewport-vs-content split: `TScrollPane` owns a content child larger than its viewport rect, offsets it on render, and uses its viewport rect as the clip rect. One- or two-axis. Mouse wheel + drag scrollbar via the existing pane event path.
- **Alpha clip (soft edge fade):** a clip variant where content alpha fades to 0 over a configurable distance from each clip edge instead of a hard cutoff. Used for scrollable lists so partially-scrolled items don't chop visually. Renderer composite-level — a small shader pass over the clipped region. Specified per clip rect as a 4-tuple of fade widths (L/R/T/B), 0 = hard clip on that edge.

**Style — `UIStyle` struct (ImGui / Unity style-asset model):**
- One `UIStyle` struct with primitive members: colors, paddings, spacings, default 9-slice atom references, font role choices (label / title / body), etc.
- **No themable components, no cascading.** Components read the active `UIStyle` directly. One global active style at a time; stack-based push/pop overrides (`PushStyleVar` / `PopStyleVar`) like ImGui.
- Components are **reused with different 9-slices** (pass a different background atom reference), not different themes.

**DEF construction:**
- `TPane::LoadFromDef(const defdoc::Node&)` (and overrides on subclasses) so retail OOG `.def` files instantiate the right pane tree.
- Container kind, margin, padding, anchor, and 9-slice references are expressed as keys in the DEF block (or in the anchor overlay block for Revisited-only metadata).

This is sanctioned per memory `feedback-ui-retained-mode` and shaped per `project-ui-layout-system`. Class identity stays — it's still `TPane`, just structurally richer. (Per `feedback-evolve-dont-replace`: no `UIWidget` / `Layout` / `Container` parallel hierarchy.)

### 2.2 `TButtonPane` → DEF-driven construction
Already has `NewButton(...)`. Add `LoadFromDef(const defdoc::Node&)` that walks button definitions from a parsed retail UI DEF (`ingamemenu.def`, `selstart.def`, etc.) and calls the existing `NewButton`. Same class, additional constructor path.

### 2.3 `TPlayScreen` — un-stub the HUD wiring
The `TODO(port)` markers in [src/playscreen.cpp](../../src/playscreen.cpp) are the live blockers: `GAMECOMMAND` dispatch (combat / inventory / spells / dialogs), pane init lifecycle, post-character animation overlays, floating dialog text. These un-stubs ARE the HUD bring-up — there's no new screen class to invent.

### 2.4 OOG screens — new `TScreen` subclasses per retail screen
Each retail OOG screen (main menu / load / save / character create / select start / options / exit confirm / in-game menu / network setup) becomes a small `TScreen` subclass that loads its panel tree from the corresponding DEF (`ingamemenu.def`, `selstart.def`, `createchar.def`, `loadgame.def`, `savegame.def`, `popup.def`, `widgets.def`, `connect.def`, `hostgame.def`, `joingame.def`, etc.). These are not "new infra" — they're concrete sibling subclasses to `TPlayScreen`, matching retail's shape, using the (now retained-mode) `TPane` tree underneath. Naming follows retail / decomp where the Ghidra label is confirmed; otherwise descriptive (`TMainMenuScreen`, `TLoadGameScreen`, etc.).

### 2.5 `Revisited::IsEnabled()` — the planned toggle accessor
[revisited/README.md](../../revisited/README.md) documents this and notes "TBD — add when the first behavioral toggle ships." The UI work is the natural first customer (the first Revisited toggle is `Visual.Resolution` or `UI.AnchoredLayout` or similar). Implement it now:
- Read `[Revisited]` section of `<SavePath>/Revenant.ini` via the existing INI API.
- Single public accessor: `bool Revisited::IsEnabled(const char* key)`. Default = false (vanilla).
- Toggle list grows as features land; values registered with defaults in one place.

### 2.6 Concrete pane modernization (interleaved with bring-up)
As each retail pane is brought online — `TInventory`, `TStatusBar`, `THealthBar`, `TStaminaBar`, `TQuickSpellPane`, `TMultiCtrlPane`, `TStatPane`, `TAutoMap`, `TEquipPane`, `TSpellPane`, `TDialogPane`, `TTextBar`, `TCursorHud`, `TScrollPane`/`TBookPane`, `TDeathPane`, `TButton` — apply the Tier-1 modern C++ sweep per memory (`feedback-modern-cpp`, `feedback-const-correctness`, `feedback-field-init`, `feedback-no-P-typedefs`, `feedback-includes`) plus the retained-mode integration. Per `feedback-code-style`: bundled with work in flight, not as a sweep.

## 3. Resolution + layout (two decoupled resolutions)

The engine treats **game-world render resolution** and **UI / display resolution** as **independent** concepts (memory: `project-resolution-modes`). Don't conflate them.

- **Game-world resolution** — the internal framebuffer the 3D scene renders to. Tied to the `[Revisited] Visual.Resolution` toggle: vanilla 640×480 (retail-faithful), Revisited 1920×1080 default. The game-world framebuffer composites under the UI as a textured quad — scaled / letterboxed depending on aspect ratio between game-world and UI canvas.
- **UI / display resolution** — the live window backing resolution, including HiDPI scale (Retina window at logical 1920×1080 has 3840×2160 backing). The UI lays out and renders at this resolution for crisp text and icons. Window is **resizable live** — layout re-runs on resize events; anchored panes reposition; clip / scroll rects invalidate.

The two combinations that matter most:
- **Vanilla, modest window:** 640×480 game-world + 640×480 (or 1280×960) UI canvas. Retail look, retail HUD coords pass through.
- **Revisited, HiDPI window:** 1920×1080 game-world + native window backing UI (e.g., 3840×2160 on Retina). Game world stays at 1920×1080 internal; UI text and icons render at the higher backing resolution.

Per-resolution choices live as toggles under `[Revisited]` in `Revenant.ini`:
- `Visual.Resolution = "1920x1080"` — game-world internal resolution.
- `Visual.HiDPI = true` — UI renders at backing resolution (vs logical). Speculative key name; finalize when implementing.
- (Future) per-axis letterbox vs fill policy when game-world and UI canvas aspects differ.

**Anchor layout is the resolution-adaptation mechanism** — already covered in §2.1. Top-level anchors target the **UI canvas (window backing)**, not the game-world framebuffer. That's how resizable + HiDPI flow naturally through to HUD positioning.

**On HiDPI, TrueType fonts (already available via [src/ttfatlas.cpp](../../src/ttfatlas.cpp)) become much more valuable than the retail bitmap fonts** — bitmap fonts will scale via nearest/bilinear and pixelate. Once the existing `TFontTable` → TTF wiring is finished (memory: `project-font-retail-compat` notes the load side is retail-compat; the wiring TODO is at [src/fonttable.cpp:217](../../src/fonttable.cpp#L217)), UI text can render TTF on HiDPI Revisited and bitmap on vanilla.

## 4. Localization

`TDialogList` already loads `ENGLISH.DEF`. UI text goes through `DialogList.GetLine("tag")`. The tag set in the existing retail `english.def` will need extending for UI labels not already there — that extension lives as a Revisited overlay file at `revisited/resources/<path>/english.def` (or whichever retail file carries UI strings). Actual non-English packs are deferred to last; the API is what we use now.

## 5. Resource overlay (mostly free)

`rev_fopen()` already walks SavePath → RunPath → active module → base. The Revisited overlay slots in via `rev_resolve_revisited_overlay()` + `MountArchive` per [revisited/README.md](../../revisited/README.md). UI work uses `LoadResource` / `TMulti::LoadMulti` / `rev_fopen` as-is and gets overlay precedence for free. Dev mode = `revisited/resources/` loose folder; deploy = `RevenantRevisited.rvr` ZIP.

## 6. Render path for UI

Unchanged in shape — composite on top of the 3D scene at end of frame via `TRenderer::Composite(...)`. UI panes hold draw data; the renderer owns submission (memory: `feedback-centralized-rendering`, `project-renderer-layering`). The retained-mode tree walk gathers what's visible; the renderer composites.

## 7. Test modes

Follow the established pattern in [src/testmodes.cpp](../../src/testmodes.cpp) — add cases to `Initialize`/`Close`/`Render` (and `HandleKeyPress`/`HandleMouseClick` as needed). Per-test implementation lives in `src/<name>test.{h,cpp}`. Data-driven registry where applicable (see `VfxTestRegistry` in [src/vfxtest.cpp](../../src/vfxtest.cpp)). Each pane class participating in a test catalog gains a `static SpawnForTest()` convention method that constructs whatever stub data it needs.

Three categories of UI test mode:
- **Subsystem modes** — e.g. `--test=ui-anchors` (anchor mapping rendered at multiple canvas sizes), `--test=ui-loadfromdef` (load a retail DEF, inspect pane tree).
- **Game panel mockup modes** — one `TPane` subclass with synthesized minimal input. `--test=ui-inventory`, `--test=ui-statpane`, `--test=ui-automap`, etc.
- **Game screen mockup modes** — full `TScreen` with all panels composed and behavior stubs at the action boundary. `--test=ui-hud-mockup`, `--test=ui-mainmenu`, `--test=ui-oog-mockup`. Highest value for dual-resolution layout verification.

## 8. Out of scope

- **Editor UI changes** beyond what UI work touches incidentally — per [../PORT_PLAN.md](../PORT_PLAN.md) §6.
- **Save/load file format changes** — load/save OOG screens drive the existing serialization.
- **Network protocol** — OOG network screens wire to whatever network layer exists; the layer itself is out of scope.
- **Aesthetic redesign of retail HUD** — modernization is structural / perf / specific requested features only. (Memory: `feedback-modernization-stance`.)
