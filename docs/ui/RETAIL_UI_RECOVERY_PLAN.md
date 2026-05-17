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

**Retail base-class identifications:**
- `TPane` base = `cls_0x5a4494` (Wave-1A; Init `FUN_00491900`, Close `FUN_00491970`).
- `TButtonPane` intermediate = `cls_0x5a45c8` (Wave-3A, 4 lines: vtable inheritance vs known-derived TQuickSpellPane + slot-0 body matches src::TButtonPane::Initialize + canonical MSVC 3-stage ctor pattern + multiple derived classes use it as base; corroborated independently by Wave-3B). Init `FUN_00434e40`, Close `FUN_00434f30`. `FUN_0041c7f0` is `TPointerArray<TButton>::ctor`, NOT TButtonPane ctor (B.r5 correction).

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

The sidebar is a single container (`TSidePane` = `cls_0x5a53ec`, Wave-1A) whose top region holds a vertical mode-switcher strip (`TSideTabsPane` = `cls_0x5a5750`, Wave-1A, 6 mode-switcher buttons) and whose remaining area shows the **content pane** for the current mode. Per `CLASSIC_HUD_REFERENCE.md` §3 and user's explicit grouping:

**Upper sidebar region (three content modes):**
- `[ ]` **Character content pane (paper-doll + equipment slots; CLASSIC_HUD §3b)** — pre-release `src/equip.h` `TEquipPane` (paper-doll + equipment around character body) + `src/inventory.h` `TInventory` (item grid below). Retail vtable not yet pinned. Container relationship unknown — could be one pane that owns both halves, or two stacked panes in this mode slot.
- `[ ]` **Stats content pane (CLASSIC_HUD §3a)** — pre-release `src/statpane.h` `TStatPane`. Retail vtable not yet pinned; this is a strong candidate for `cls_0x5a5ba0` (vtable[0] = `0x546b50`, the "second stat-pane init" per Wave-2B — adjacent to `cls_0x5a5ae8` in .rdata).
- `[x]` **Spell list content pane (CLASSIC_HUD §3d)** — identified as `cls_0x5a5ae8` by Wave-3B (role descriptor `SpellbookSidebarPane`; iterates player's known-spells list at `DAT_00667fcc+0x2ec/+0x2fc`, formatted `"SPELL %s"`). **NOT** the pre-release `src/spellpane.h` `TSpellPane` (which is the talisman composer). Likely a new retail class with no src name.

**Lower sidebar region (two combined content modes, per user):**
- `[ ]` **Automap content pane (CLASSIC_HUD §3e)** — pre-release `src/automap.h` `TAutoMap`. `RECON_UI_COVERAGE.md` notes PERFECT yaml mapping for retail.
- `[ ]` **Inventory content pane (lower sidebar — combined with automap per user grouping)** — `src/inventory.h` `TInventory`. Container relationship with the upper-region "character" mode TBD: is this the same `TInventory` class instantiated twice, or are paper-doll-grid and lower-sidebar inventory distinct content panes?

**Sidebar-content-host pattern note:** the pre-release shell `TMultiCtrlPane` (`src/multictrl.h`) used a global `MultiPanes[]` array that the button strip switched between. Retail evolved this into `TSideTabsPane` (cls_0x5a5750) — verify whether the switching pattern (one pane visible at a time vs simultaneous upper+lower) survives or got rebuilt.

### Tier 4 — bottom quickspell + shelf

- `[ ]` **TQuickSpellPane** — pre-release in `src/spellpane.h`. Per user: "shows currently equipped spells + has some slots for potions". The "shelf" terminology might be retail's name; check for distinct retail classes for quickspells vs potion-slots.
- `[ ]` **TInventory** — pre-release in `src/inventory.h`. User flagged the bottom area I labeled "inventory" is actually quickspells + potion shelf, so TInventory may NOT live in the bottom area at all in retail. It might be one of the right-sidebar tabs, or its own thing. Verify against recon.

### Tier 5 — conditional overlays (dialog, books, scrolls, death, popup)

- `[ ]` **TDialogPane** — pre-release in `src/dialog.h`. `RECON_UI_COVERAGE.md` confirms this exists in retail.
- `[ ]` **TScrollPane** / **TBookPane** — pre-release in `src/scroll.h` (where `TScrollPane` is the parchment-text reader, not a viewport scroll; `TBookPane` is the book reader). Retail confirms TBookPane exists.
  - **Note (Wave-3B 2026-05-16):** `cls_0x5a5ae8` (parchment-styled in-game pane with up/down arrows) is **NOT** TBookPane / TScrollPane / TJournalPane / TSpellPane. It's the **spell-list sidebar pane** that iterates the player's known-spells list (`DAT_00667fcc+0x2ec/+0x2fc`, formatted `"SPELL %s"`); likely a new class added between pre-release src/ and final retail (no src name available). Conservative role descriptor in use: `SpellbookSidebarPane`. See [briefs/B_r9_parchment_viewer.md](briefs/B_r9_parchment_viewer.md). Separate TBookPane / TScrollPane (the parchment-text reader) class vtable not located near cls_0x5a5ae8 — may have been cut from retail OR live in a different .rdata region. Wave-4 work.
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

- `[x]` **B.r11 — Identify the widget engine entry point** — DONE (Wave-2C, commit `42ff390`). Engine entry at `0x4377c0` (load); parse kickoff at `0x437620`; PANEL parser at `0x437000`; control dispatcher at `0x436ec0`; widget vocabulary fully extracted from `.rdata` at `0x005ccfb8-0x005cdc64`. See `docs/ui/briefs/B_r7_def_widget_engine.md`.
- `[x]` **B.r12 — Identify the widget renderer** — DONE in two waves. Wave-2C identified the per-widget vtable+0xb4 dispatch shape. Wave-3C clarified the registry: `DAT_00655510` is `DefWidgetClassRecord**` (count `DAT_0065617c = 8`); records carry `{vtable_ptr, name_str}`. All 8 register thunks located. TWidget base vtable at `0x5a3ab8` (24 slots). SCROLLBAR widget ctor pinned (`FUN_0042de00`, vtable `0x5a3cd8`, type-id 5) — confirms the canonical widget-ctor pattern `*this = vtable; FUN_0042a210(this, N, ...)`. See [briefs/B_r10_def_widget_subclasses.md](briefs/B_r10_def_widget_subclasses.md). **Correction to Wave-2C:** the functions Wave-2C labelled "widget ctors" (FUN_0042aaf0 BITMAP, FUN_0042b340 TEXT, FUN_0042bd90 BUTTON, etc.) are actually STYLE-ATTRIBUTE PARSERS, not ctors. True ctors for BITMAP / TEXT / BUTTON / FRAME / LISTBOX / EDIT / DROPLIST are small adjacent functions yet to be individually pinned (Wave-4 work via FindImmRefs on each registered record).
- `[x]` **B.r13 — Identify the input dispatcher** — DONE (Wave-2C). Input dispatcher at `0x4361f0` (also handles 'R' hot-reload of the .def — dev feature).
- `[ ]` **B.r14 — Port the engine** so the existing 22 `.def` files render correctly. Test mode: `--test=ui-defwidget-engine` driven against a known-simple `.def` (e.g. `exit.def` — but note: exit.def UI may route through `popup.def`'s "yesno" template, not be a separate activator). Real-target test mode candidate: `popup.def`.
- `[ ]` **B.r15 — Per-screen verification**. Once the engine renders, each DEF-driven screen should "just work" from the `.def` content. **12 per-screen activators identified** (Wave-2C: connect / connectsimple / savegame / loadgame / options / popup / hostgame / joingame / createchar / ingamemenu / mpingame; Wave-3C: selstart). **userinfo** activation routes through `FUN_00463149_MPLobby_ButtonDispatch` (Wave-3C — multiplayer-lobby button dispatcher branching on 7 button names; userinfo is one of its 7 branches, not a standalone activator). Verify each screen against retail behavior + `--test=ui-options`, `--test=ui-savegame`, etc.

### Tier 10 — Special-cased non-HUD screens (NEW)

These are neither pure-HUD panes nor DEF-driven; they have their own special-case classes:

- `[ ]` **B.r16 — TLogoScreen** (`cls_0x5a5d18`) — main menu / splash. Partly identified in `recon/discovered/cls_0x5a5d18_TLogoScreen_{Animate,Initialize}.cpp`. May actually BE the main menu (screenshot 5 / `main_menu_ui.jpg`) or be the pre-menu splash with the menu as a separate screen.
- `[ ]` **B.r17 — TDeathPane** (`src/death.h`) — death screen. Has src/ impl (31+85 lines, very thin); cross-check retail. Recon string evidence: `"Trouble_initializing_Death_pane"` confirms retail class exists. Likely heavily evolved from src/ stub.
- `[ ]` **B.r18 — TBookPane / TScrollPane** (`src/scroll.h`) — book / scroll reader. Renders IN the right sidebar per `CLASSIC_HUD_REFERENCE.md §3c`. Has src/ impl (87+331 lines); cross-check retail. **Note (Wave-3B):** `cls_0x5a5ae8` is NOT TBookPane/TScrollPane (Wave-3B explicitly rejected those candidates — that class is the spell-list sidebar pane). The real retail TBookPane/TScrollPane vtable is in a different .rdata region — separate forensic hunt required.
- `[ ]` **B.r19 — Credits screen** — TBD whether DEF-driven or special-cased. No identification yet. Hunt: check for a `credits.def` activator + look for a "TCredits" / scrolling-text class in recon.
- `[ ]` **B.r20 — Main menu identification** — confirm whether `main_menu_ui.jpg` is TLogoScreen, a separate TMainMenuScreen, or a DEF-driven screen wrapped in special chrome.

### Tier 11 — sidebar tab content classes (NEW — per user 2026-05-16 grouping)

The 6-tab sidebar (per `TSideTabsPane` Wave-1A finding) splits visually into two content regions:

**Upper sidebar (3 tabs):**
- `[ ]` **B.r21 — Character sidebar tab (upper)** — paper-doll + equipment slots, per CLASSIC_HUD §3b. Roster items: `TEquipPane` (src/equip.h) + possibly `TInventory` (src/inventory.h) for the item grid below. Container relationship TBD.
- `[ ]` **B.r22 — Stats sidebar tab (upper)** — `TStatPane` (src/statpane.h). Strong retail candidate: `cls_0x5a5ba0` (vtable[0] = `0x546b50` — the "second stat-pane init" Wave-2B; sibling vtable to `cls_0x5a5ae8` in .rdata). Confirm via dedicated extraction.
- `[x]` **B.r23 — Spell list sidebar tab (upper)** — DONE per Wave-3B (`cls_0x5a5ae8` = `SpellbookSidebarPane` role descriptor; real class identity remains unnamed). See `briefs/B_r9_parchment_viewer.md`.

**Lower sidebar (combined automap + inventory):**
- `[ ]` **B.r24 — Automap sidebar tab (lower)** — `TAutoMap` (src/automap.h). PERFECT yaml mapping per `RECON_UI_COVERAGE.md`; identify retail vtable + verify.
- `[ ]` **B.r25 — Inventory sidebar tab (lower)** — `TInventory` (src/inventory.h). Per user: this is the lower-sidebar inventory, separate from / combined with the upper Character tab's paper-doll grid. Container relationship needs verification: same class twice? distinct retail classes? part of the Character tab spilling into the lower region?

**Sidebar visibility model (confirmed by user 2026-05-16):** `TSideTabsPane`'s 6 round buttons control sidebar content visibility (confirmed: `sample_screen_1.jpg` shows Stats parchment + Automap circle visible simultaneously, so the buttons are not a flat 1-of-6 mode selector). The exact wire-up — independent per-button toggles vs preset combinations — is recovered by extracting TSideTabsPane's input/click handler; whatever the mapping, the 6 buttons map onto the 5-6 content classes enumerated above plus possibly one for book/scroll display in the sidebar slot (CLASSIC_HUD §3c).

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
