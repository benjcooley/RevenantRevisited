# UI Resurrection — Burndown

Live status. Update as work progresses. Rationale: [PLAN.md](PLAN.md). Constraints: [README.md](README.md). Architecture: [ARCHITECTURE.md](ARCHITECTURE.md).

**Legend:** `[ ]` pending · `[~]` in progress · `[x]` done · `[!]` blocked

Last updated: 2026-05-16

---

## Inherited from HEAD (`80879c2`) — built before this burndown

The HUD render-path infrastructure was landed in commit `80879c2` ("engine: HUD architecture, world->pixel camera matrix, mode-aware cursor") prior to the burndown being committed. The rest of the burndown builds on top of these — they're not in scope for re-doing:

- **HUD render path:** `THudDrawable` base class + `Renderer->AddHud(d, z)` / `RemoveHud(d)` / `DrawHud()`. Z-ordered draws inside the swapchain pass — replaces the prior surface-cache/blit-to-2D-overlay. See [src/renderer.h:271+](../../src/renderer.h#L271) and [src/renderer.cpp:3535+](../../src/renderer.cpp#L3535).
- **Renderer-side HUD draw primitives:** `Renderer->DrawBitmap(PTBitmap, x, y)` and `Renderer->DrawSurface(TSurface*, x, y)` are the inside-`Draw()` primitives every HUD widget will call. Bitmap → texture cache lives in the renderer.
- **`GameData` (`playscrn.dat`) loaded** by `PlayScreen::Initialize` ([src/playscreen.cpp:206](../../src/playscreen.cpp#L206)). HUD/cursor bitmap resources resolve through it.
- **OS cursor / game cursor swap on ImGui ownership** — editor panels & menus get the OS pointer, the playfield gets the game cursor, never both. Lives in cursor code.

**Implication:** the panes brought up in Phase B do their drawing by subclassing `THudDrawable` (or by being composed inside something that does), registering with `Renderer->AddHud(this, z)`, and issuing `Renderer->DrawBitmap` / `DrawSurface` from `Draw()`. The retained-mode `TPane` evolution in A.2 must integrate with this — not replace it.

## Phase A — Foundational evolutions

- `[x]` **A.1 `Revisited::IsEnabled()` accessor** — landed in `45fe058` as `src/revisited.{h,cpp}`. Reads `[Revisited]` section of `<SavePath>/Revenant.ini` via existing INI API; default per key = false. No call sites yet — wires to first Revisited-gated A.2 feature.
- `[ ]` **A.2 `TPane` retained-mode evolution** (`feedback-ui-retained-mode`, `project-ui-layout-system`). *Invasive — use worktree.* Sub-items, all on existing `TPane` in [src/screen.h](../../src/screen.h) / [src/screen.cpp](../../src/screen.cpp):
  - `[x]` **A.2a Hierarchy** — landed in `810bbb9`. `TPane` gained `parent` + `children` (non-owning, matches TScreen's TPaneArray convention) + `AddChild`/`RemoveChild`/`GetParent`/`Children()`. `SetDirty(true)` propagates up parent chain; `SetDirty(false)` does not. Added missing `virtual ~TPane()` that unlinks from parent and nulls children's back-pointers. Vanilla path unchanged — no existing pane uses AddChild.
  - `[x]` **A.2b 2-pass measure→layout** — landed in `de39125`. `SLayoutKind` (None/Vertical/Horizontal), `SSizePolicy` (Fixed/Greedy), `SSpacing`, `SSize` types on `TPane`. `MeasureSelf` (bottom-up) + `LayoutChildren` (top-down) + `RunLayoutPass` entry point. Vanilla path: `layoutKind == None` panes hit no new code. `--test=ui-layout` exercises 800×600 vertical container with mixed fixed/greedy children + nested horizontal split; verified rects match prediction. Fold-in: const-correct simple inline TPane getters.
  - `[ ]` **A.2c Anchors** — top-left / top-right / bottom-center / center / etc. + native-size pin. Activates only when canvas size differs from explicit rect. `--test=ui-anchors` shows correct positions at 640×480 and 1920×1080.
  - `[ ]` **A.2d 9-slice rendering** — bitmap + 4 insets (L/R/T/B); corners verbatim, edges tile/stretch, center fill. Source atoms from retail widget atlases (`widgetstex.dat`, `medgold.dat`, etc.). `--test=ui-nineslice` renders one source atom at several target sizes.
  - `[ ]` **A.2e `UIStyle` struct** — one struct of style primitives (colors, paddings, spacings, default 9-slice atoms, font role choices). Stack-based push/pop overrides à la `ImGui::PushStyleVar`. No themable-component cascade.
  - `[ ]` **A.2f Clip rect on `TPane` base + scroll/clip evolution** — `TPane` gains an optional clip rect (renderer applies scissor/stencil at composite). [`TScrollPane`](../../src/scroll.h) evolved to viewport+content split (content child larger than viewport, offset on render, viewport rect = clip rect; mouse wheel + drag). `--test=ui-scroll` synthesizes a scrollable list inside a clip rect.
  - `[ ]` **A.2g Alpha clip (soft edge fade)** — clip-rect variant with per-edge fade widths (L/R/T/B), implemented at renderer composite level via a small shader pass. `--test=ui-scroll` includes a soft-clip variant alongside the hard-clip one.
  - `[ ]` **A.2h Resizable + HiDPI canvas** — UI canvas = live window backing resolution (not game-world framebuffer); resize events re-run layout; top-level anchors target window backing. Game-world framebuffer composites under UI as a textured quad. `--test=ui-anchors` exercises 640×480 game/UI, 1920×1080 game/UI, 1920×1080 game with HiDPI UI backing, plus mid-test resize. Reference: `project-resolution-modes` updated.
- `[ ]` **A.3 `TPane::LoadFromDef` + `TButtonPane::LoadFromDef`** — DEF-driven construction. Walks `defdoc::Node`, instantiates panes/buttons. Unknown widget kinds log and skip.
- `[ ]` **A.4 Anchor metadata extension to retail DEF** — define convention inside the existing DEF format (extends `defdoc`). Document in `revisited/resources/README.md` (or extend existing). Vanilla ignores anchor blocks.

## Phase B — HUD bring-up

- `[ ]` **B.1 `TTextBar`** — vertical slice; first pane on the retained-mode tree end-to-end + `--test=ui-textbar`.
- `[ ]` **B.2 `THealthBar`, `TStaminaBar`** (`TStatusBar` subclasses) + `--test=ui-statusbars`.
- `[x]` **B.3 `TCursorHud`** — landed in `80879c2` as a `THudDrawable` subclass registered at z=0 (below other HUD), with OS-pointer / game-cursor swap on ImGui ownership. See [src/cursor.h:32](../../src/cursor.h#L32). Win32-clipping `#if 0` still pending if/when relevant; deferred (cursor works without it).
- `[ ]` **B.4 `TQuickSpellPane`** + `--test=ui-quickspells`.
- `[ ]` **B.5 `TMultiCtrlPane`** — 4-button switcher with 1/2/3/4 keys.
- `[ ]` **B.6 `TInventory`** — drag-and-drop, container traversal + `--test=ui-inventory`.
- `[ ]` **B.7 `TEquipPane`** — reuse B.6 drag infrastructure.
- `[ ]` **B.8 `TAutoMap`** + `--test=ui-automap`.
- `[ ]` **B.9 `TStatPane`** + `--test=ui-statpane`.
- `[ ]` **B.10 `TDialogPane`** + `--test=ui-dialog` — hooks into existing script/dialog system; consumes `TDialogList`.
- `[ ]` **B.11 `GAMECOMMAND` dispatch un-stub** — un-stub `TODO(port)` markers in [src/playscreen.cpp](../../src/playscreen.cpp). Combat / inventory / spells / dialogs. *Tightly coupled to game state; no test mode.*
- `[ ]` **B.12 Panel visibility toggles** — B / V / Space keys per [../HUD.md](../HUD.md).
- `[ ]` **B.13 `--test=ui-hud-mockup`** — full HUD composition test, synthesized data, both resolutions. *Composite mockup.*

## Phase C — OOG bring-up

- `[ ]` **C.1 `TMainMenuScreen`** + `--test=ui-mainmenu`.
- `[ ]` **C.2 Popup mechanism** — `TPopupScreen` or modal helper; reused by everything downstream.
- `[ ]` **C.3 `TExitConfirmScreen`** — trivial pattern shakedown.
- `[ ]` **C.4 `TOptionsScreen`** — settings I/O via existing INI API.
- `[ ]` **C.5 `TLoadGameScreen`** — save format integration; read-only listing first.
- `[ ]` **C.6 `TSaveGameScreen`** — mirror of C.5.
- `[ ]` **C.7 `TCreateCharScreen`** — portrait + attributes + `--test=ui-createchar`.
- `[ ]` **C.8 `TSelStartScreen`** — continues from C.7.
- `[ ]` **C.9 `TInGameMenuScreen`** — pause bridge between HUD and OOG + `--test=ui-ingamemenu`.
- `[ ]` **C.10 Network setup screens** — host / join / connect. *Blocked on network-layer readiness.*
- `[ ]` **C.11 `--test=ui-oog-mockup`** — end-to-end OOG flow with stubbed actions. *Composite mockup.*

## Phase D — Revisited toggles

Each is a key under `[Revisited]` in `Revenant.ini`, default off. Order driven by need.

- `[ ]` **D.1 `Visual.Resolution = "1920x1080"`** — anchored layout fully exercised. Anchor overlays for every HUD + OOG screen ship in `revisited/resources/`.
- `[ ]` **D.2 `Visual.HiDPI` / variable resolution** — beyond 1920×1080.
- `[ ]` **D.3 `UI.ExpandedHUD`** — speculative; only if requested.
- `[ ]` **D.4a Localization v1 (German pack)** — AI pipeline. Stresses anchor layout via +30% length.
- `[ ]` **D.4b Localization v2 (first CJK pack)** — stresses font fallback / glyph coverage.
- `[ ]` **D.5 Settings UI** — only if hand-edited `Revenant.ini` becomes painful.

---

## Cross-cutting (interleaved with phase work, not a separate burndown)

Per memory `feedback-code-style`, `feedback-modern-cpp`, `feedback-const-correctness`, `feedback-field-init`, `feedback-no-P-typedefs`, `feedback-includes`: as each pane file is touched, apply Tier-1 modern C++, default-init members, drop `P*` typedefs, trim umbrella includes, add `const` where appropriate. Not separate tickets — part of "done" for every B / C item.

## Known dependencies on other efforts

- **Streaming layer** (memory `project-playscreen-boot-state`) — B.11 `GAMECOMMAND` dispatch and C.7 character create both benefit from a real Player. Coordinate with whoever owns save-load / new-game.
- **`TScreen` retail mapping uncertainty** (memory `project-tscreen-unconfirmed`) — don't lean on Ghidra TScreen labels as ground truth; verify via decomp source-path fingerprints (memory `reference-revsync-convention`).

## Open decisions

(None currently blocking. The original D1–D8 from earlier drafts are all either resolved or collapsed into existing convention — see Notes log.)

## Notes log (most-recent first)

- **2026-05-16** — A.2b 2-pass measure→layout landed in `de39125`. `--test=ui-layout` validates math. Fold-in: const-correctness on simple TPane getters.
- **2026-05-16** — A.2a TPane parent/children/dirty propagation landed in `810bbb9`. Added missing virtual dtor as fold-in fix.
- **2026-05-16** — A.1 `Revisited::IsEnabled()` shipped as `src/revisited.{h,cpp}` (commit `45fe058`). Awaiting first call site.
- **2026-05-16** — Reconciled with `feature/ui` HEAD `80879c2`. HUD render-path infrastructure (`THudDrawable` + `AddHud`/`RemoveHud`/`DrawHud` + `Renderer->DrawBitmap`/`DrawSurface` + `GameData` load) landed prior to this burndown; documented as "Inherited from HEAD" section. B.3 (`TCursorHud`) marked done — cursor works via `THudDrawable` subclass with ImGui-ownership pointer swap.
- **2026-05-16** — A.2 added **A.2h** for resizable + HiDPI: UI canvas = live window backing, decoupled from game-world framebuffer. Game world composites under UI as a textured quad. Layout re-runs on resize. `project-resolution-modes` rewritten to make game-world resolution and UI canvas resolution two distinct concepts.
- **2026-05-16** — A.2 added clip rect on `TPane` base + `TScrollPane` viewport/content evolution + alpha (soft-edge) clip variant (A.2f, A.2g). `--test=ui-scroll` covers both hard and soft clip. `project-ui-layout-system` memory updated.
- **2026-05-16** — A.2 expanded with concrete sub-items: layout (2-pass V/H + margin/padding + fixed/greedy), anchors, **proper 9-slice rendering** (no existing impl in `src/` to evolve — built fresh from retail widget atlases), and a `UIStyle` struct (ImGui/Unity style-asset model, no themable cascade). New memory: `project-ui-layout-system`.
- **2026-05-16** — Plan rewritten after deeper read of `docs/` and `src/`. Most "new module" proposals collapsed: `rvroverlay` → use existing `rev_fopen` + `rev_resolve_revisited_overlay`; `defpatch` → mutation at consumer (existing pattern); `uiloader` → `LoadFromDef` method on existing `TPane`/`TButtonPane`; `loc::Get` → use existing `TDialogList`/`DialogList.GetLine`; settings (TOML/simpleini) → use existing INI API + `[Revisited]` toggles per [revisited/README.md](../../revisited/README.md); `TOOGScreen` base class → just sibling `TScreen` subclasses per retail screen. Phase A shrank to 4 surgical items; Phase D anchored in the existing toggle model. New memories added: `feedback-evolve-dont-replace`, `reference-revisited-overlay-system`, `feedback-ui-retained-mode`. UI work positioned as a slice of master [../PORT_PLAN.md](../PORT_PLAN.md) Phases 4 + 5.
- **2026-05-16** — Effort kicked off. Initial survey + first-pass docs (now superseded).
