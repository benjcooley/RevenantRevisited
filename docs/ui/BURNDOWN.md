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
  - `[x]` **A.2c Anchors** — landed in `f3e9dbd`. `SAnchor` enum (9-cell grid + None); `TPane.anchor` + `SetAnchor/GetAnchor`. Anchored children inside `layoutKind == None` parents get positioned against the parent's content rect; child margin acts as inset from the anchored edge. `--test=ui-anchors` verifies correct scaling across 640×480, 1920×1080, 3840×2160 canvases. Vanilla path unchanged.
  - `[x]` **A.2d 9-slice rendering** — landed in `d40a7c8`. `TRenderer::DrawNineSlice(PTBitmap, l, t, r, b, dx, dy, dw, dh)` built on existing `BitmapAsTexture` + `CompositeSwapchain` primitives. Corners verbatim, edges stretch, center fills. Degenerate dest clamps cleanly. `--test=ui-nineslice` math-dumps several breakdowns; visual blit verifies when first B-phase pane wires a real widget atlas (source atoms from `widgetstex.dat`, `medgold.dat`, etc.).
  - `[x]` **A.2e `UIStyle` struct** — landed in `311dcb8`. `src/uistyle.{h,cpp}` with primitives struct (colors, paddings, spacings, default 9-slice atoms `TBitmap*`, font role choices `TFont*`) + `UI::Style()` / `PushStyle(...)` / `PopStyle()` / `StyleDepth()`. Function-local-static base + vector stack. No themable cascade. `--test=ui-style` verifies depth 1→2→3→2→1 with per-field override/restore.
  - `[x]` **A.2f.i `TPane` clip-rect API** — landed in `69446de`. `HasClipRect` / `GetClipRect` / `SetClipRect` / `ClearClipRect` on TPane base. `--test=ui-clip` verifies storage contract.
  - `[ ]` **A.2f.ii Renderer-side scissor honoring `TPane::clipRect`** — wires through `DrawHud` so a registered drawable with a clip rect actually gets scissored. Deferred to first B-phase consumer that needs real clipping (likely inventory list).
  - `[ ]` **A.2f.iii General-purpose viewport+content ScrollView** — note: `TScrollPane` (src/scroll.h) is the parchment-text reader, not a general scrollable container. A real ScrollView is a NEW class; deferred until the first B-phase consumer (inventory list, save-game list, etc.) requires it.
  - `[x]` **A.2g Alpha clip (soft edge fade) API** — landed in `c9e60da`. `SSpacing clipFade` on TPane + `GetClipFade` / `SetClipFade`. Renderer-side fade shader is TODO (lands with the first soft-clip consumer). `--test=ui-clip` extended to verify the fade setter contract.
  - `[x]` **A.2h Resizable canvas hook** — landed in `5ddce9c`. `OnCanvasResize(int w, int h)` virtual on TPane (default = Resize+PaneResized+RunLayoutPass) and TScreen (default = broadcast to all panes). sokol_app `SAPP_EVENTTYPE_RESIZED` integration lives in the consumer (display.cpp / mainwnd.cpp) when a screen first needs live-resize. `--test=ui-anchors` exercises the hook on the 4K canvas verification.
- `[ ]` **A.3 `TPane::LoadFromDef` + `TButtonPane::LoadFromDef`** — DEF-driven construction. Walks `defdoc::Node`, instantiates panes/buttons. Unknown widget kinds log and skip.
- `[ ]` **A.4 Anchor metadata extension to retail DEF** — define convention inside the existing DEF format (extends `defdoc`). Document in `revisited/resources/README.md` (or extend existing). Vanilla ignores anchor blocks.

## Phase B — HUD bring-up (**RETRACTION — see note below**)

> **Phase B reset 2026-05-16.** The class/rect set this section originally
> enumerated (`THealthBar`/`TStaminaBar` as standalone vertical fluid tubes
> at HEALTHBARX/HEALTHBARY etc, `TInventory` at 16×386 382×85, etc.) does
> **not match the shipped retail HUD**. It was derived from pre-release
> `src/` defines + an AI-generated `docs/HUD.md` that the user (the
> original developer of Revenant) flagged as wrong. The retail HUD is:
>
> 1. **Two combined health/mana/character panels** in the upper-left
>    and upper-right corners of the playfield — not vertical edge tubes.
> 2. **Right sidebar area** — a full multifunction panel (character /
>    spell list / automap / inventory).
> 3. **Bottom quickspell + shelf area** — equipped spells + potion slots
>    (not a 7×2 inventory grid).
> 4. **Game log panel** — transparent overlay on the bottom area
>    (TTextBar is likely this; its retail decomp at
>    ~~`recon/classes_converted/cls_0x5a4358_likely_TTextBar.cpp`~~ (mislabel — that's `TConsolePane`; real TTextBar at FUN_0054bf70 per Wave-1C) is ~13 KB,
>    4× the pre-release size, suggesting significant scrolling/multi-line
>    features beyond the pre-release Print() API).
> 5. **Dialog overlays** — conditional top-area + bottom-area overlays.
> 6. **Books / scrolls** — main-game-area overlays, conditional.
> 7. **Button bar** — small tab area, bottom-right playfield overlay.
>
> Reconstruction is in flight; see "Phase B — actual" below.

### Phase B — retracted items

- `[!]` **B.1 `TTextBar`** — data-API contract test (`4df1bcf`) still
  useful for the pre-release class as a snapshot reference. The
  visualizer commit (`7873ac1`) painted a yellow fixed-position strip,
  which is the wrong rendering for what is actually a transparent
  game-log overlay. The retail decomp shows TTextBar evolved
  significantly past the pre-release shape — needs a fresh port plan.
- `[!]` **B.2 `THealthBar`, `TStaminaBar`** — standalone vertical
  fluid-tube classes have **no basis in the shipped retail UI**.
  Per user. **The classes are slated for removal**; their work is
  absorbed by the combined upper-corner character-info panels.
  - The `HealthBar` / `StaminaBar` *globals* (`src/revmain.cpp:125-126`)
    are load-bearing: `player.cpp`, `food.cpp`, `spell.cpp`,
    `effect_old.cpp` all call `HealthBar.ChangeLevel(...)` on gameplay
    state changes. Removal needs to either re-target those calls to
    the new character-panel model or refactor `TStatusBar` into a
    pure value-with-animation data model (no TPane).
  - Test modes `uistatusbartest.{h,cpp}` and the composite
    `uihudmockuptest.{h,cpp}` moved to `attic/src/` — they encoded
    the wrong rects/labels.
- `[!]` **B.13 `--test=ui-hud-mockup`** — composite wireframe was
  built on the same wrong layout. Retracted; replaced by a future
  composite that uses the verified retail panel set.

### Phase B — actual (forensic reconstruction)

**Procedure:** follow the established retail-sync method per
[`../../recon/docs/RETAIL_SYNC_PLAN.md`](../../recon/docs/RETAIL_SYNC_PLAN.md)
and the 3-stage decomp pipeline per [`../../recon/docs/PIPELINE.md`](../../recon/docs/PIPELINE.md).
This is the same forensic class of work as gameplay/combat/vfx
reconstructions in the same recon tree.

**Per-character bars:** the user clarified retail has **three** bars
per character: **health, stamina, AND mana**. The combined character
panel (upper-left + upper-right) hosts all three plus character info.
This corrects the "two bars" framing throughout earlier docs.

**Plan document:** [RETAIL_UI_RECOVERY_PLAN.md](RETAIL_UI_RECOVERY_PLAN.md)
is the master plan — procedure, per-class roster (Tier 0–8), Ghidra
CLI fallback path, sub-agent delegation pattern, session-survival
checklist.

**Inventory artifacts:**
- [SRC_UI_INVENTORY.md](SRC_UI_INVENTORY.md) — 607-line catalog of
  every UI-related file in `src/`, classified by retail-correspondence
  confidence (Very High / High / Medium / Low / port-specific).
- [RECON_UI_COVERAGE.md](RECON_UI_COVERAGE.md) — 320-line catalog of
  recon coverage: 15 confirmed UI classes in `classes_readable/`,
  5 "likely" in `classes_converted/`, YAML mapping cross-refs,
  string-based discovery results, PlayScreen decomp survey,
  retail-HUD-element → candidate-recon-class coverage map, gaps.

**Roster status** (per RETAIL_UI_RECOVERY_PLAN.md Tier system — the plan doc has the full per-class enumeration; BURNDOWN tracks status only):

Tier 0 substrate done. Tier 1–8 panes mostly identified at the container layer; content classes per tab in flight. Tier 9 DEF engine identified, port pending (B.r14). Tier 10 OOG: TLogoScreen partial; TDeathPane / TBookPane / TScrollPane / credits / main-menu placeholders open (B.r16–B.r20). Tier 11 sidebar tab content classes (B.r21–B.r25) — spell list done; character / stats / automap / inventory placeholders open.

Highlights (full status in [RETAIL_UI_RECOVERY_PLAN.md](RETAIL_UI_RECOVERY_PLAN.md)):

- `[~]` **B.r1 Forensic inventory + recovery plan** — landed `e8c4f3c`
  (inventories) + this commit (plan). Tier roster populated; per-class
  reconstruction now traceable.
- `[ ]` **B.r2 Tier 1 — Combined character panel (TCharacterPane, real
  name TBD)** — two instances, each hosts three bars (health, stamina,
  mana) + character info; identity hunt: investigate
  `recon/classes_readable/TStatusBar.{h,cpp}` (may be the multi-bar
  grouping not a single bar), string hunt in `class_index.tsv`,
  PlayScreen decomp `meth_0x*` callers.
- `[ ]` **B.r3 Tier 2 — Game-log overlay (TTextBar retail port)** —
  ~~recon `cls_0x5a4358_likely_TTextBar.cpp` is 4× pre-release size;~~ **MISLABEL RETRACTED 2026-05-16** — cls_0x5a4358 is actually TConsolePane (Wave-1C golden-path identification). Real retail TTextBar is at FUN_0054bf70 (init wrapper) with three TMosaicSurface buffers; leaf class vtable still TBD;
  port the missing scrolling/multi-line/history features.
- `[ ]` **B.r4 Tier 3 — Right sidebar (TMultiCtrlPane + TEquipPane +
  TSpellPane + TAutoMap + TStatPane)** — verify each against recon.
- `[ ]` **B.r5 Tier 4 — Bottom quickspell + shelf (TQuickSpellPane +
  potion shelf, possibly distinct classes)**.
- `[ ]` **B.r6 Tier 5 — Conditional overlays (TDialogPane + TBookPane +
  TScrollPane + TDeathPane + TPopupPane)**.
- `[ ]` **B.r7 Tier 6 — Button bar (class identity unknown — recon hunt)**.
- `[ ]` **B.r8 Tier 7 — Cursor + asset infrastructure (font catalog,
  playscrn.dat / intrface.dat asset enumeration)**.
- `[ ]` **B.r9 Tier 8 — TPlayScreen retail sync** (drives every pane's
  AddPane sequence; capstone of Phase B).
- `[ ]` **B.r10 Statusbar disposition** — `src/statusbar.{h,cpp}` and
  the `HealthBar`/`StaminaBar` globals: re-shape into a pure
  value-with-animation model the character panel reads, OR remove
  entirely after re-targeting every gameplay-side `HealthBar.*` /
  `StaminaBar.*` call (per `SRC_UI_INVENTORY.md` global usage list).
  Also: add a third global / channel for mana if not already in
  TStatusBar pattern.
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
- `[x]` **B.13 `--test=ui-hud-mockup`** — landed in `43e49fb`. Composite wireframe at retail 640×480: TextBar, HealthBar, StaminaBar, Inventory, QuickSpells, MultiCtrl, StatPane (representative multipane) all constructed at retail rects from revdefs.h and rendered via a single visualizer registered with `Renderer->AddHud`. Status bars animate (sine-cycle SetLevel). No `Initialize()` called -- mockup verifies layout topology, not per-pane content. Per-pane content rendering is the follow-up per-pane work.

## Phase C — OOG bring-up

- `[ ]` **C.0 Smacker (.SMK) video playback support** — required by C.1 main menu intro + credits roll + inter-mission FMVs. Pre-release `src/` has NO Smacker decoder or movie player (no SMK references found). Assets shipped with retail: `data/Disk2/MIX_CREDITS.SMK`, `data/Disk2/MIX_FMV1.SMK`, `data/Disk2/MIX_FMV2.SMK`, `data/Disk2/Mix_fmv3english.smk` (4 total). Scope:
  - **Decoder selection — GATED ON LICENSE AUDIT (TODO).** RAD Game Tools' official Smacker SDK is proprietary (no go). Known open-source decoders all need license verification before vendoring:
    - `libsmacker` (Greg Kennedy) — believed LGPL-2.1+; if so, requires either dynamic linking OR providing object files / replaceable lib so users can relink. Doable for our binary-into-GOG-install model but adds packaging burden.
    - FFmpeg's Smacker decoder — LGPL/GPL depending on build config; same dynamic-link/replace constraints, much heavier dep than we want for one codec.
    - ScummVM's Smacker decoder — GPL-2+; viral, almost certainly off-spec for our distribution.
    - Clean-room implementation from public format documentation (MultimediaWiki) — most permissive but largest implementation cost; would need a careful "no GPL source read" workflow.
  - **Action:** before any port work, pick the decoder + confirm license fit with the distribution model. Vendor location: `thirdparty/<chosen>/` (e.g. `thirdparty/libsmacker/`) once cleared.
  - **Video frame upload pipeline (UI worktree scope):** decoded YV12 (or RGB-converted) frame uploaded into a `sokol_gfx` texture per frame; cinematic skippable on Esc / click (generic input dismissal).
  - **Audio decode + playback: COORDINATE WITH AUDIO SUBSYSTEM (sibling worktree).** Not in this worktree's scope. The decoder will yield PCM; whoever owns the audio subsystem provides the playback channel.
  - **Screen wrapper:** `TCinematicScreen` (new) — `TScreen` subclass that owns the decoder stream + per-frame `Update()` that advances/uploads/draws. Composes underneath the existing OOG screen stack so a menu can transition into it.
  - **Test mode:** `--test=ui-cinematic <path>` — load one SMK and play it through the new pipeline to verify decode + frame upload before wiring to game flow. Audio verification depends on the sibling-worktree audio subsystem.
  - **Used by:** C.1 (intro on first New Game), C.? (inter-mission), C.? (credits trigger from main menu).
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

- **2026-05-16** — **Wave-3 forensic findings (3A/3B/3C) committed.** Three parallel agents; 9 conservative renames; 11 new decompilations; 3 briefs (B_r8/B_r9/B_r10).
  - **3A (B.r8):** TPlyrStatusBar single-instance two-pass-draw CONFIRMED from extracted slot 23 body. TButtonPane base = `cls_0x5a45c8` (4 lines of evidence). B.r5's FUN_0041c7f0 corrected to `TPointerArray<TButton>::ctor`.
  - **3B (B.r9):** `cls_0x5a5ae8` identified as spell-list sidebar pane (iterates `DAT_00667fcc+0x2ec`, `"SPELL %s"`); REJECTED all four candidate src class names (TBookPane/TScrollPane/TJournalPane/TSpellPane) — likely a new retail class. Conservative role descriptor `SpellbookSidebarPane` in use. Cross-corroborated TButtonPane = cls_0x5a45c8.
  - **3C (B.r10):** widget registry layout pinned (DefWidgetClassRecord** with 8 records, all register thunks located). SCROLLBAR widget ctor (`FUN_0042de00`, vtable `0x5a3cd8`) is the exemplar. CORRECTION to Wave-2C: prior "widget ctor" labels are actually STYLE-ATTRIBUTE PARSERS — true ctors pending in Wave-4. selstart activator pinned; userinfo routes through `FUN_00463149_MPLobby_ButtonDispatch` (7-way button dispatcher).
- **2026-05-16** — **Forensic recovery plan + inventories.** SRC_UI_INVENTORY.md + RECON_UI_COVERAGE.md committed `e8c4f3c`; RETAIL_UI_RECOVERY_PLAN.md drafted this commit. Procedure follows `recon/docs/RETAIL_SYNC_PLAN.md` (same shape as gameplay/combat/vfx reconstructions). Tier 0 (substrate) done; Tier 1–8 enumerated. **Three bars per character (health/stamina/mana)** per user clarification. Next: B.r2 character-panel class identity hunt via recon.
- **2026-05-16** — **Phase B reset.** User (original Revenant developer) flagged that the `docs/HUD.md` layout + the pre-release `src/` rect defines (`HEALTHBARX` etc) do not match the shipped retail HUD. Standalone health/stamina vertical-tube panes are not in the game. `uihudmockuptest.{h,cpp}` and `uistatusbartest.{h,cpp}` moved to `attic/src/`; corresponding dispatch removed from `testmodes.cpp`. Build clean. `statusbar.{h,cpp}` left in `src/` for now because `HealthBar`/`StaminaBar` globals are load-bearing in gameplay; disposition pending. New phase items B.r1+ to reconstruct from `recon/` decomp.
- **2026-05-16** — Phase B.13 `--test=ui-hud-mockup` (`43e49fb`): **all 7 visible retail HUD panes rendered at retail rects in the new HUD pipeline.** Animated status bars + color-coded outlines for the rest. Verifies layout topology matches `docs/HUD.md`. No per-pane Initialize required (constructors set rects). **RETRACTED — see Phase B reset note.**
- **2026-05-16** — Phase B step 2 for B.1 + B.2 (`7873ac1`): first visible panes on the new HUD pipeline. Each test mode registers a `THudDrawable` visualizer that paints `DrawSolidRect` at the pane's retail rect. Status bars animate level via sine cycle. Pattern is the template for remaining pane bring-ups.
- **2026-05-16** — `TRenderer::DrawSolidRect` (`a8e36f3`): per-color 1x1 texture cache + existing composite pipeline. Unblocks visual fallback for any pane that hasn't ported its retail draw path.
- **2026-05-16** — Phase B started. B.1 step 1 (`4df1bcf`, TTextBar data-API contract) and B.2 step 1 (`495a28c`, TStatusBar/THealthBar/TStaminaBar data-API + retail-asset load verification).
- **2026-05-16** — A.2h `OnCanvasResize` hook landed in `5ddce9c`. Phase A.2 complete except for A.2f.ii (renderer scissor) and A.2f.iii (ScrollView), both deferred to first B-phase consumer.
- **2026-05-16** — A.2g alpha (soft-edge) clip API landed in `c9e60da`. Renderer fade shader TODO.
- **2026-05-16** — A.2f scope split: A.2f.i TPane clip-rect API landed in `69446de` (`--test=ui-clip` verified). A.2f.ii renderer-side scissor deferred to first B-phase consumer. A.2f.iii general ScrollView carved out as separate item (TScrollPane is parchment-text, not a viewport container).
- **2026-05-16** — A.2e UIStyle + push/pop stack landed in `311dcb8`. `--test=ui-style` verified.
- **2026-05-16** — A.2d 9-slice renderer primitive landed in `d40a7c8`. `--test=ui-nineslice` math-dumps verified.
- **2026-05-16** — A.2c anchors landed in `f3e9dbd`. `SAnchor` 3×3 grid; `--test=ui-anchors` verifies across 640×480 / 1920×1080 / 3840×2160 canvases.
- **2026-05-16** — A.2b 2-pass measure→layout landed in `de39125`. `--test=ui-layout` validates math. Fold-in: const-correctness on simple TPane getters.
- **2026-05-16** — A.2a TPane parent/children/dirty propagation landed in `810bbb9`. Added missing virtual dtor as fold-in fix.
- **2026-05-16** — A.1 `Revisited::IsEnabled()` shipped as `src/revisited.{h,cpp}` (commit `45fe058`). Awaiting first call site.
- **2026-05-16** — Reconciled with `feature/ui` HEAD `80879c2`. HUD render-path infrastructure (`THudDrawable` + `AddHud`/`RemoveHud`/`DrawHud` + `Renderer->DrawBitmap`/`DrawSurface` + `GameData` load) landed prior to this burndown; documented as "Inherited from HEAD" section. B.3 (`TCursorHud`) marked done — cursor works via `THudDrawable` subclass with ImGui-ownership pointer swap.
- **2026-05-16** — A.2 added **A.2h** for resizable + HiDPI: UI canvas = live window backing, decoupled from game-world framebuffer. Game world composites under UI as a textured quad. Layout re-runs on resize. `project-resolution-modes` rewritten to make game-world resolution and UI canvas resolution two distinct concepts.
- **2026-05-16** — A.2 added clip rect on `TPane` base + `TScrollPane` viewport/content evolution + alpha (soft-edge) clip variant (A.2f, A.2g). `--test=ui-scroll` covers both hard and soft clip. `project-ui-layout-system` memory updated.
- **2026-05-16** — A.2 expanded with concrete sub-items: layout (2-pass V/H + margin/padding + fixed/greedy), anchors, **proper 9-slice rendering** (no existing impl in `src/` to evolve — built fresh from retail widget atlases), and a `UIStyle` struct (ImGui/Unity style-asset model, no themable cascade). New memory: `project-ui-layout-system`.
- **2026-05-16** — Plan rewritten after deeper read of `docs/` and `src/`. Most "new module" proposals collapsed: `rvroverlay` → use existing `rev_fopen` + `rev_resolve_revisited_overlay`; `defpatch` → mutation at consumer (existing pattern); `uiloader` → `LoadFromDef` method on existing `TPane`/`TButtonPane`; `loc::Get` → use existing `TDialogList`/`DialogList.GetLine`; settings (TOML/simpleini) → use existing INI API + `[Revisited]` toggles per [revisited/README.md](../../revisited/README.md); `TOOGScreen` base class → just sibling `TScreen` subclasses per retail screen. Phase A shrank to 4 surgical items; Phase D anchored in the existing toggle model. New memories added: `feedback-evolve-dont-replace`, `reference-revisited-overlay-system`, `feedback-ui-retained-mode`. UI work positioned as a slice of master [../PORT_PLAN.md](../PORT_PLAN.md) Phases 4 + 5.
- **2026-05-16** — Effort kicked off. Initial survey + first-pass docs (now superseded).
