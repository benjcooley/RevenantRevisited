# Retail UI Recovery Plan

**Status:** active — started 2026-05-16
**Scope:** reconstruct the shipped Revenant retail UI (HUD + overlays + OOG) from the Ghidra decomp so the **Classic** mode rendering is precisely faithful to retail. Revisited modernization layers on top via overlay toggles (see [`../../revisited/README.md`](../../revisited/README.md)).

If this plan picks up cold in a fresh session, **read in this order**:
1. [README.md](README.md) (entry point + invariants)
2. [ARCHITECTURE.md](ARCHITECTURE.md) (substrate built in Phase A)
3. This document (recovery procedure + per-class roster)
4. [BURNDOWN.md](BURNDOWN.md) (live status — "what's next?" answer lives there)
5. [SRC_UI_INVENTORY.md](SRC_UI_INVENTORY.md) (what's in `src/` today, what's off-spec)
6. [RECON_UI_COVERAGE.md](RECON_UI_COVERAGE.md) (what retail provides via the decomp)

---

## Why this plan exists

The pre-release UI in `src/` does **not** match the shipped retail HUD. The user (original developer of Revenant) flagged that the layout Phase B tried to bring up — standalone vertical health/stamina tube bars on the screen edges, an inventory grid at the bottom, a fixed-position text bar — is wrong. Reality (per user):

1. **Two combined character panels** upper-left + upper-right of the playfield, each showing **three bars per character: health, stamina, mana** (plus character info / portrait).
2. **Right sidebar** — multifunction panel for character / spell list / automap / inventory (tab-switched).
3. **Bottom quickspell + shelf** — equipped spells + potion slots.
4. **Game log panel** — transparent overlay over the bottom area.
5. **Dialog overlays** — conditional, top and/or bottom game-area overlays.
6. **Books and scrolls** — conditional main-game-area overlay.
7. **Button bar** — small tab area, bottom-right playfield overlay.

`docs/HUD.md` is AI-generated and does not match retail; treat it as a non-authoritative reference. The pre-release `src/` define values (`HEALTHBARX`, `INVENTORYPANEX`, etc.) describe an earlier UI iteration that was extensively reworked before release.

This is the same forensic class of work as the existing reconstructions for gameplay, combat, vfx, etc. We follow the same procedure.

## Hard invariant

> **Classic mode UI must precisely match the shipped retail game.** Pixel-faithful layout, retail asset rendering, retail interaction behavior. Modernizations (anchored layout for 16:9, HiDPI, localization, alternate fonts/textures) are toggled by `Revisited::IsEnabled("...")` and ship in the `revisited/` overlay. **Classic = vanilla retail.**

## Inputs

- **`SRC_UI_INVENTORY.md`** — every UI-related file in `src/`, classified by retail-correspondence confidence. Identifies which classes carry forward, which are off-spec, which need significant porting (TPlayScreen 14× size delta, etc.).
- **`RECON_UI_COVERAGE.md`** — recon's coverage of retail UI classes. 15 confirmed in `recon/classes_readable/`, 5 in `recon/classes_converted/`, address+confidence mapping, gaps requiring deeper Ghidra dives.
- **Pre-existing recon infrastructure** (read before doing per-class work):
  - [`recon/docs/PIPELINE.md`](../../recon/docs/PIPELINE.md) — 3-stage decomp pipeline
  - [`recon/docs/RETAIL_SYNC_PLAN.md`](../../recon/docs/RETAIL_SYNC_PLAN.md) — established per-class sync method + class-ID recipe
  - [`recon/docs/GHIDRA_CLASS_IDENTIFICATIONS.md`](../../recon/docs/GHIDRA_CLASS_IDENTIFICATIONS.md) — identified retail class addresses
  - [`recon/docs/CLASS_MAPPING.md`](../../recon/docs/CLASS_MAPPING.md) — initial size-based mapping
  - [`recon/analysis/class_index.tsv`](../../recon/analysis/class_index.tsv) — fingerprint index (grep-friendly)
  - [`recon/analysis/ghidra_fingerprints.txt`](../../recon/analysis/ghidra_fingerprints.txt) — struct shape fingerprints

## Established procedure (per-class)

Adapted from [`recon/docs/RETAIL_SYNC_PLAN.md §"Method per class"`](../../recon/docs/RETAIL_SYNC_PLAN.md). For each UI class:

1. **Identify retail address.** Use the class-ID recipe (yaml mapping → readable decomp → unique log string → API signature → struct shape → numeric-literal density → method signature shape). Record in this doc and in the class file's `// REVSYNC:` header per `feedback-revsync-convention`.
2. **Read the recon decomp.** Prefer `recon/classes_readable/<Name>.{h,cpp}` if present; fall back to `recon/classes_converted/cls_0x*_likely_<Name>.cpp` or raw `cls_0x*.cpp`.
3. **Diff against `src/<file>.{h,cpp}`.** Per `SRC_UI_INVENTORY.md` the current state is mostly pre-release; the diff drives the port.
4. **Port retail layout + semantics:**
   - Member offsets / struct layout match retail exactly (required for serialization + inheritance to work)
   - Method signatures match retail unless trivially equivalent
   - Keep modern C++ conventions on the surface (override, =default, nullptr, const-correctness) per existing convention memories
5. **If decomp is missing or incomplete:** drop to **Ghidra CLI fallback** (see below) to extract the function. Output goes into `recon/classes_converted/` first; if extensive cleanup is warranted, promote to `recon/classes_readable/`.
6. **Visual test mode.** Each class gets a `--test=ui-<name>` mode following `feedback-test-modes-for-ui`:
   - **Subsystem mode** if data-only (loader / parser / state model)
   - **Game panel mockup** with synthesized input (single TPane subclass rendered via the HUD pipeline)
   - **Screen mockup** for full screens (composite)
7. **Visual self-verification.** Per `reference-visual-verification`: launch the test in background, screencapture, Read PNG inline, confirm against retail reference (screenshot of original game, recon strings, user confirmation). **Do not declare a class done without visual sign-off.**
8. **Commit.** One class per commit, REVSYNC header in source, BURNDOWN updated, classifier in PR-style commit message.

## Ghidra CLI fallback

When the decomp is missing or insufficient (raw `cls_0x*.cpp` doesn't compile, or method bodies aren't recovered), use the Ghidra headless CLI from `recon/scripts/` (or via the `ghidra` directory) to extract the missing function. Verify the binary path (`data/Revenant.exe`) and follow the existing recon pipeline pattern: dump function → drop into `recon/classes_converted/` → minimal cleanup → promote when readable.

The recon pipeline scripts at `recon/scripts/` are:
- `build_class_index.py` — rebuilds `recon/analysis/class_index.tsv`
- Other extraction / conversion utilities (read the directory contents at recovery time; the scripts evolve)

If a class identity is uncertain, follow the class-ID recipe (RETAIL_SYNC_PLAN.md §"Class ID recipe").

## Sub-agent delegation

For parallel forensic work, spawn `Explore` agents with focused tasks:
- One per class for the read-recon-and-diff-vs-src phase (cheap, parallel)
- One per recovery gap (deeper Ghidra extraction, slower)

Each sub-agent's output must be saved as a file in `docs/ui/` (sub-task brief + findings) so it survives session loss. Brief template in [CONVENTIONS.md](CONVENTIONS.md) "Sub-agent brief template" section.

## Per-class reconstruction roster

Derived from `RECON_UI_COVERAGE.md` §6 (retail HUD element → candidate recon class). Ordered by **dependency** (substrate first) and **forensic confidence** (confirmed yaml mappings before "likely" guesses). Status legend: `[ ]` not started · `[~]` in progress · `[x]` done · `[!]` blocked.

### Tier 0 — substrate (already done)

The TPane/TScreen/TButtonPane/TButton core framework is in `src/` and was confirmed in `SRC_UI_INVENTORY.md` as carrying forward. Phase A.2 extended TPane with retained-mode hierarchy, layout, anchors, 9-slice, style, clip, OnCanvasResize. This is the foundation; no retail-sync needed here.

### Tier 1 — combined character panel (upper-left + upper-right)

The single biggest gap. Two instances of one pane class, each showing 3 bars (health, stamina, mana) + character info / portrait for the corresponding player. **`HealthBar` / `StaminaBar` globals in `revmain.cpp` are read by `player.cpp` / `food.cpp` / `spell.cpp` / `effect_old.cpp`** — the character panel needs to consume those (or replace them with a richer state model).

- `[ ]` **TCharacterPane (working name, real name TBD via recon)** — class identity unknown. Investigate:
  - `recon/classes_readable/TStatusBar.{h,cpp}` — `RECON_UI_COVERAGE.md` notes this may be the **combined** character panel (not a single bar). The "bar" in the name may refer to the multi-bar grouping, not a single tube.
  - String hunt in `class_index.tsv` for "health" / "stamina" / "mana" / "portrait" / strings that are likely to appear in a character-panel implementation.
  - PlayScreen decomp survey (`RECON_UI_COVERAGE.md` §5) — find which `meth_0x*` constructs/registers this pane.

### Tier 2 — game-log overlay (transparent bottom area)

- `[ ]` **TTextBar (retail)** — **mislabel retracted**: `cls_0x5a4358` is NOT TTextBar; it's `TConsolePane` (editor console — pre-existing CLASS_MAPPING.md size-heuristic error, caught by Wave-1C 2026-05-16 per `docs/ui/briefs/B_r4_textbar_assessment.md`).
  - **Real retail TTextBar init wrapper:** `FUN_0054bf70` (golden-path confirmed via "text bar" string anchor + cross-ref to pre-release `src/textbar.cpp` features). Body allocates a ~0x450-byte instance with **three `TMosaicSurface` children** — the multi-buffer transparent overlay infrastructure that explains the user's "transparent multi-line overlay" description.
  - **TTextBar leaf class vtable address: still TBD** (Wave-2 to extract the `new <size>` allocation pattern inside FUN_0054bf70 + find the vtable wire site).
  - `SetHealthDisplay` (pre-release opponent name+health overlay path) IS retained in retail bit-for-bit (FUN_0054cb00 — same 155/176/16/186/4-increment constants as pre-release).
  - Read recon decomp; diff vs pre-release; document the missing features.
  - Recon string evidence (RECON_UI_COVERAGE.md §4): "Trouble_initializing_Death_pane" — confirms a TDeathPane exists; similar diagnostic strings around TextBar may exist.

### Tier 3 — right sidebar (multifunction panel)

- `[ ]` **TMultiCtrlPane** — pre-release present in `src/multictrl.h`. Confidence: medium. The pre-release **uses a global `MultiPanes[]` array** that the multifunction button strip switches between; that pattern likely holds in retail. Verify against recon: is the sidebar a single big pane or is each tab a separate pane? What's the tab-switching mechanism?
- `[ ]` **TEquipPane** — pre-release present in `src/equip.h`. Verify retail.
- `[ ]` **TSpellPane** — pre-release in `src/spellpane.h` (note: not `TQuickSpellPane`).
- `[ ]` **TAutoMap** — pre-release in `src/automap.h`. `RECON_UI_COVERAGE.md` notes PERFECT yaml mapping for retail.
- `[ ]` **TStatPane** — pre-release in `src/statpane.h`. Likely the "character/skills" tab content.

### Tier 4 — bottom quickspell + shelf

- `[ ]` **TQuickSpellPane** — pre-release in `src/spellpane.h`. Per user: "shows currently equipped spells + has some slots for potions". The "shelf" terminology might be retail's name; check for distinct retail classes for quickspells vs potion-slots.
- `[ ]` **TInventory** — pre-release in `src/inventory.h`. User flagged the bottom area I labeled "inventory" is actually quickspells + potion shelf, so TInventory may NOT live in the bottom area at all in retail. It might be one of the right-sidebar tabs, or its own thing. Verify against recon.

### Tier 5 — conditional overlays (dialog, books, scrolls, death, popup)

- `[ ]` **TDialogPane** — pre-release in `src/dialog.h`. `RECON_UI_COVERAGE.md` confirms this exists in retail.
- `[ ]` **TScrollPane** / **TBookPane** — pre-release in `src/scroll.h` (where `TScrollPane` is the parchment-text reader, not a viewport scroll; `TBookPane` is the book reader). Retail confirms TBookPane exists.
- `[ ]` **TDeathPane** — pre-release in `src/death.h`. Recon string evidence: "Trouble_initializing_Death_pane".
- `[ ]` **TPopupPane** — popup framework (used by save / load / option confirms). Not yet identified in inventory.

### Tier 6 — button bar (small tab, bottom-right playfield overlay)

- `[ ]` **TButtonBar (working name)** — class identity unknown. Not yet identified in `SRC_UI_INVENTORY.md`. Recon hunt required. Hints to chase: distinct from `TMultiCtrlPane` (sidebar buttons); positioned bottom-right; probably tabular.

### Tier 7 — cursor + asset infrastructure

- `[ ]` **TCursorHud** — already a `THudDrawable` in HEAD (B.3 done per `BURNDOWN.md`). Verify cursor bitmap selection logic matches retail (CURSOR_NONE, CURSOR_EYE, etc.) — pre-release defines these in `src/cursor.h`.
- `[ ]` **TFontTable / TFont / TFontData** — `project-font-retail-compat` memory says the loader is verified retail-compat at the data layer. Render-side wiring through new HUD primitives is per-pane work; the **font catalog** (which fonts under which roles) should be documented as a retail reference.
- `[ ]` **playscrn.dat / intrface.dat asset catalog** — enumerate every bitmap name actually used by the retail HUD code (the pre-release names like "texthealthbar" / "silverfont" / "equipdownup" might differ from retail). Recon string discovery is the right tool.

### Tier 8 — TPlayScreen pane construction

- `[ ]` **TPlayScreen retail sync** — the decomp is where every HUD pane gets constructed and `AddPane`'d. Most of the work above feeds back into this: once we know each pane class, port `TPlayScreen::Initialize` to construct them all with the retail addresses + AddPane sequence. The full Initialize body is already extracted at `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp` (per Wave-1A).
  - Note: the recon mapping originally tagged `cls_0x5b4f30` as TPlayScreen but per `recon/discovered/README.md` that's TPlayer; TPlayScreen is `cls_0x5a5320`.

### Tier 9 — DEF-driven widget engine (NEW)

Per user, **settings / multiplayer / save / load / character-create / select-start / in-game menu / exit-confirm / popups / user-info** are NOT hard-coded TScreen subclasses. They're rendered by a **widget engine** that walks per-screen `.def` files. The engine + the existing `.def` files in `data/resources_unzipped/` (22 files) = the whole secondary-screen layer.

Don't hunt per-screen classes for these. Hunt the engine.

- `[ ]` **B.r11 — Identify the widget engine entry point**. The parser reads `widgets.def` for primitive widget definitions and per-screen files for layout + behavior. Search the retail decomp for unique strings from `widgets.def` (widget type names like `BUTTON`, `LABEL`, `LIST`, `CHECKBOX`, etc.; common attribute names like `POS`, `SIZE`, `TEXT`, `STYLE`). The function emitting / parsing those strings is the engine's parse/build entry. Trace inward.
- `[ ]` **B.r12 — Identify the widget renderer**. Once a widget tree is built, something walks it and renders each widget. Look for the draw-walk function — likely takes a widget-list pointer + a draw context. Cross-reference against `Display.Box` / `Display.WriteText` calls (the legacy CPU-surface primitives used by HUD code; the widget engine probably uses the same primitives).
- `[ ]` **B.r13 — Identify the input dispatcher**. Walks the widget tree for hit-testing + routes input to the active widget. Probably entry point on each key/mouse event from the active screen.
- `[ ]` **B.r14 — Port the engine** so the existing 22 `.def` files render correctly. Test mode: `--test=ui-defwidget-engine` driven against a known-simple `.def` (e.g. `exit.def` — just an OK/Cancel popup).
- `[ ]` **B.r15 — Per-screen verification**. Once the engine renders, each DEF-driven screen should "just work" from the `.def` content; verify each screen against retail screenshots (`--test=ui-options`, `--test=ui-savegame`, etc.).

### Tier 10 — Special-cased non-HUD screens (NEW)

These are neither pure-HUD panes nor DEF-driven; they have their own special-case classes:

- `[ ]` **B.r16 — TLogoScreen** (`cls_0x5a5d18`) — main menu / splash. Partly identified in `recon/discovered/cls_0x5a5d18_TLogoScreen_{Animate,Initialize}.cpp`. May actually BE the main menu (screenshot 5 / `main_menu_ui.jpg`) or be the pre-menu splash with the menu as a separate screen.
- `[ ]` **B.r17 — TDeathPane** (`src/death.h`) — death screen. Has src/ impl; cross-check retail.
- `[ ]` **B.r18 — TBookPane / TScrollPane** (`src/scroll.h`) — book / scroll reader. Renders IN the right sidebar per `CLASSIC_HUD_REFERENCE.md §3c`. Has src/ impl; cross-check retail.
- `[ ]` **B.r19 — Credits screen** — TBD whether DEF-driven or special-cased.
- `[ ]` **B.r20 — Main menu identification** — confirm whether `main_menu_ui.jpg` is TLogoScreen, a separate TMainMenuScreen, or a DEF-driven screen wrapped in special chrome.

## Progress tracking (BURNDOWN integration)

[BURNDOWN.md](BURNDOWN.md) has the live status. Each tier above gets a section in the burndown when work starts; per-class items get checked off there with the commit hash. This document is the procedure / roster reference; BURNDOWN is the live state.

## Session-survival checklist

When pausing or handing off:
1. Every commit on `feature/ui` includes a message that describes WHAT and WHY (not just diff summary).
2. New findings go into one of: this doc / `SRC_UI_INVENTORY.md` / `RECON_UI_COVERAGE.md` / a per-class brief in `docs/ui/briefs/`.
3. Memory files in `~/.claude/projects/.../memory/` get updated for any cross-session convention (e.g. retail HUD layout note, identified class addresses).
4. BURNDOWN updated with current tier + item + status.
5. Build clean at the last commit (`cmake --build build --target Revenant -j` → 100%).
6. Note the next action in the BURNDOWN "Notes log" so the next agent has an immediate starting point.

## Out of scope for this plan

- Revisited-mode UI redesign (anchored 16:9 layout, HiDPI, localization, alternate fonts/textures). Those layer on top of the verified Classic implementation via `Revisited::IsEnabled` toggles per `revisited/README.md`.
- Editor UI (per `../PORT_PLAN.md` §6 — "Level editor UI modernization beyond what it takes to not crash" is explicitly not in scope).
- Multiplayer-specific UI (network setup screens, chat) — deferred until base HUD is faithful.

## Open questions for the user

These don't block plan execution but improve confidence:

1. **3 bars / character panel layout** — health, stamina, mana — what's the visual grouping per panel? (Three side-by-side gauges? One main gauge + two smaller? Bottom-fill vs top-fill?)
2. **Single-player or 2-player on screen** — the "two character panels upper-left + upper-right" — is the right one always the player's pet/companion? Or only present in coop? Or always shown for both? Affects pane lifecycle / visibility logic.
3. **Right sidebar tab switching** — is the multifunction panel always visible with tabs the player switches between, or do tabs hide/show the entire sidebar?
4. **Quickspell vs potion shelf** — same pane or distinct adjacent panes in retail?
5. **TInventory** — does this pane exist in retail at all, or has its function been absorbed into the right sidebar (e.g. inventory tab)?

A quick "yes / no / depends" pass on these dramatically narrows the recon hunt for Tier 1–4.
