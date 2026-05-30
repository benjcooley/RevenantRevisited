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

- `[x]` **`TPlyrStatusBar` = `cls_0x5a54e4`** (Wave-1A string anchor `"PlyrStatusB(ar)"` + src/revtypes.h:1166 forward decl). Init `FUN_00549740`, Close `FUN_00549d40`. SINGLE INSTANCE, TWO-PASS DRAW with stat-source swap (Wave-3A confirmed from extracted slot 23 body `FUN_0054af20`, 3909 bytes): left pass reads `DAT_00667fcc` (current player) at fixed coords; right pass reads `DAT_00667fcc[0x38][0x11]` (player→target→character) at mirrored coords (`pane_width - 0x88/-0x91/-0x79`). No `SetSource` API — binding implicit through global pointer. Animation counters at +0xd4 (left) / +0xdc (right) drive smooth right-panel fade on target swap. Reframes the "TCharacterPane" placeholder originally posited here. **`HealthBar` / `StaminaBar` globals in `revmain.cpp` are read by `player.cpp` / `food.cpp` / `spell.cpp` / `effect_old.cpp`** — the character panel needs to consume those (or replace them with a richer state model). See [briefs/B_r2_character_panel_hunt.md](briefs/B_r2_character_panel_hunt.md) + [briefs/B_r8_charpane_draw_buttonpane.md](briefs/B_r8_charpane_draw_buttonpane.md).

### Tier 2 — game-log overlay (transparent bottom area)

- `[~]` **TTextBar (retail)** — wrap identified, leaf vtable TBD. **Mislabel retracted**: `cls_0x5a4358` is NOT TTextBar; it's `TConsolePane` (editor console — pre-existing CLASS_MAPPING.md size-heuristic error, caught by Wave-1C 2026-05-16 per `docs/ui/briefs/B_r4_textbar_assessment.md`).
  - **Real retail TTextBar init wrapper:** `FUN_0054bf70` (golden-path confirmed via "text bar" string anchor + cross-ref to pre-release `src/textbar.cpp` features). Body allocates a ~0x450-byte instance with **three `TMosaicSurface` children** — the multi-buffer transparent overlay infrastructure that explains the user's "transparent multi-line overlay" description.
  - **TTextBar leaf class vtable address: still TBD** (Wave-2 to extract the `new <size>` allocation pattern inside FUN_0054bf70 + find the vtable wire site).
  - `SetHealthDisplay` (pre-release opponent name+health overlay path) IS retained in retail bit-for-bit (FUN_0054cb00 — same 155/176/16/186/4-increment constants as pre-release).
  - Read recon decomp; diff vs pre-release; document the missing features.
  - Recon string evidence (RECON_UI_COVERAGE.md §4): "Trouble_initializing_Death_pane" — confirms a TDeathPane exists; similar diagnostic strings around TextBar may exist.

### Tier 3 — right sidebar (multifunction panel)

The sidebar is a single container (`TSidePane` = `cls_0x5a53ec`, Wave-1A) whose top region holds a vertical mode-switcher strip (`TSideTabsPane` = `cls_0x5a5750`, Wave-1A, 6 mode-switcher buttons) and whose remaining area shows the **content pane** for the currently-selected modes in each region.

**Per-tab content classes:** see Tier 11 (B.r21–B.r26) for the per-pane roster — Wave-4A identified all six content panes via the TSideTabsPane button cascade. Quick summary:
- Upper region (3-way mode selector `DAT_0065d1b8`): Equip `cls_0x5a55dc` / Stats `cls_0x5a5ba0` / Book `cls_0x5a5ae8`
- Lower region (3-way mode selector `DAT_0065d1bc`): Inv `cls_0x5a58c0` / Map `cls_0x5a5658` / Spell `cls_0x5a5978`

**Sidebar-content-host pattern (CONFIRMED Wave-4A):** the pre-release shell `TMultiCtrlPane` (`src/multictrl.h`, 4-button switcher) was rebuilt into `TSideTabsPane` (`cls_0x5a5750`, 6-button vertical strip) for retail. Visibility is **two independent 3-way region selectors** — `sample_screen_1.jpg`'s simultaneous Stats + Map confirms it. Dispatch routes through `TPlayScreen::DispatchCommand` (`FUN_0047cf40`) cases 0x7–0xc, NOT through TSideTabsPane's own click handler.

### Tier 4 — bottom quickspell + shelf

- `[x]` **TQuickSpellPane = `cls_0x5a5a30`** (Wave-1A/2A "quick spell pane" string anchor + src/spellpane.h forward decl `class TQuickSpellPane : public TButtonPane`). Init `FUN_00544160`. Inherits from `TButtonPane = cls_0x5a45c8` (Wave-3A confirmed via 12+ shared vtable slots — TQuickSpellPane was Wave-3A's golden-path proof that cls_0x5a45c8 is TButtonPane). NOTE: this is the BOTTOM-area quickspell row (shown in `sample_screen_1.jpg`'s bottom-left), distinct from the lower-sidebar "Spell" tab `cls_0x5a5978` (`QuickSpellSidebarPane`, B.r26).
- `[x]` **Potion shelf folded into `TBarInvPane = cls_0x5a56d4`** (Wave-1A: bottom-row item slots, Init `FUN_0052c970`, global @ `0x65b028`). The "shelf" terminology was retail's; TBarInvPane covers it. TInventory (src/inventory.h) is the LOWER-SIDEBAR inventory tab `cls_0x5a58c0` (B.r24), not a bottom-area class.

### Tier 5 — conditional overlays (dialog, books, scrolls, death, popup)

- `[ ]` **TDialogPane** — pre-release in `src/dialog.h` (117+426 lines); `RECON_UI_COVERAGE.md` confirms this exists in retail; **retail vtable not yet pinned** (Wave-5 candidate).
- `[x]` **TScrollPane = `cls_0x5b5750`** and **TBookPane = `cls_0x5b5808`** (Wave-4B golden-path: `scroll.dat`/`book.dat` + button asset names + VK keycodes match `src/scroll.cpp` Initialize bodies exactly; vtable diff matches src `TBookPane final : public TScrollPane` overriding only Initialize + DrawBackground). Both live in 0x5b5xxx .rdata region (NOT near cls_0x5a5ae8, vindicating Wave-3B's rejection). See [briefs/B_r12_oog_closeout.md](briefs/B_r12_oog_closeout.md).
- `[x]` **TDeathPane = `cls_0x5b93c4`** (Wave-4B golden-path: `"Trouble initializing Death pane"` string at 0x5e3ec8 referenced from screen wrapper `FUN_005338a0`; Initialize at `0x005339b0` creates 3 buttons matching `src/death.cpp` exactly). **Bonus: TDeathScreen = `cls_0x5b9374`** (new retail wrapper class with no pre-release counterpart; loads `death.dat`, invokes TDeathPane::Initialize on global @ DAT_0066f500). Bare-class rename for `cls_0x5b93c4` deferred pending OOAnalyzer-flatten scrub (intermediate TButtonPane = `cls_0x5a45c8` methods mixed in — now unblocked since Wave-3A confirmed the intermediate).
- `[x]` **TPopupPane — folded into Tier 9.** No standalone `TPopupPane` class; popup overlays are DEF-driven through the widget engine (`popup.def` + `popup` activator `FUN_0053bf00` identified Wave-2C).

### Tier 6 — button bar (small tab, bottom-right playfield overlay)

- `[x]` **Folded into Tier 3 + Tier 11.** The "button bar" Tier 6 was originally posited as a separate class is actually `TSideTabsPane = cls_0x5a5750` (Wave-1A), the 6-round-button vertical strip controlling the sidebar's 2× independent 3-way region selectors (Wave-4A confirmed via TPlayScreen DispatchCommand FUN_0047cf40 cases 0x7–0xc).

### Tier 7 — cursor + asset infrastructure

- `[~]` **TCursorHud** — `THudDrawable` infrastructure DONE in HEAD (B.3 per `BURNDOWN.md`). Cursor bitmap selection logic verification vs retail (CURSOR_NONE, CURSOR_EYE, etc. per `src/cursor.h`) still pending.
- `[ ]` **TFontTable / TFont / TFontData** — `project-font-retail-compat` memory says the loader is verified retail-compat at the data layer. Render-side wiring through new HUD primitives is per-pane work; the **font catalog** (which fonts under which roles) should be documented as a retail reference.
- `[ ]` **playscrn.dat / intrface.dat asset catalog** — enumerate every bitmap name actually used by the retail HUD code (the pre-release names like "texthealthbar" / "silverfont" / "equipdownup" might differ from retail). Recon string discovery is the right tool.

### Tier 8 — TPlayScreen pane construction

- `[ ]` **TPlayScreen retail sync** — the decomp is where every HUD pane gets constructed and `AddPane`'d. Most of the work above feeds back into this: once we know each pane class, port `TPlayScreen::Initialize` to construct them all with the retail addresses + AddPane sequence. The full Initialize body is already extracted at `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp` (per Wave-1A).
  - Note: the recon mapping originally tagged `cls_0x5b4f30` as TPlayScreen but per `recon/discovered/README.md` that's TPlayer; TPlayScreen is `cls_0x5a5320`.

### Tier 9 — DEF-driven widget engine (NEW)

Per user, **settings / multiplayer / save / load / character-create / select-start / in-game menu / exit-confirm / popups / user-info** are NOT hard-coded TScreen subclasses. They're rendered by a **widget engine** that walks per-screen `.def` files. The engine + the existing `.def` files in `data/resources_unzipped/` (22 files) = the whole secondary-screen layer.

Don't hunt per-screen classes for these. Hunt the engine.

- `[x]` **B.r11 — Identify the widget engine entry point** — DONE (Wave-2C, commit `42ff390`). Engine entry at `0x4377c0` (load); parse kickoff at `0x437620`; PANEL parser at `0x437000`; control dispatcher at `0x436ec0`; widget vocabulary fully extracted from `.rdata` at `0x005ccfb8-0x005cdc64`. See `docs/ui/briefs/B_r7_def_widget_engine.md`.
- `[x]` **B.r12 — Identify the widget renderer + per-widget classes** — DONE across three waves. Wave-2C identified the per-widget vtable+0xb4 dispatch shape. Wave-3C clarified the registry: `DAT_00655510` is `DefWidgetClassRecord**` (count `DAT_0065617c = 8`); records carry `{vtable_ptr, name_str}`. All 8 register thunks located. TWidget base vtable at `0x5a3ab8` (24 slots). Wave-4C closed it out with all 8 widget true ctors + factories + per-widget class-descriptor vtables + per-instance vtables: BITMAP / FRAME / TEXT / BUTTON / SCROLLBAR / LISTBOX / EDIT / DROPLIST (type-ids 1–8). Wave-4C also corrected two Wave-3C errors (record 0x655508 = FRAME not BGBITMAP-helper; SCROLLBAR parse-time ctor is `0x42df00` not `0x42de00`) and confirmed Wave-2C's 6 "ctor" labels are actually style-attribute parsers (relabel pairs produced). See [briefs/B_r10_def_widget_subclasses.md](briefs/B_r10_def_widget_subclasses.md) + [briefs/B_r13_def_engine_closeout.md](briefs/B_r13_def_engine_closeout.md).
- `[x]` **B.r13 — Identify the input dispatcher** — DONE (Wave-2C). Input dispatcher at `0x4361f0` (also handles 'R' hot-reload of the .def — dev feature).
- `[ ]` **B.r14 — Port the engine** so the existing 22 `.def` files render correctly. Test mode: `--test=ui-defwidget-engine` driven against a known-simple `.def` (e.g. `exit.def` — but note: exit.def UI may route through `popup.def`'s "yesno" template, not be a separate activator). Real-target test mode candidate: `popup.def`.
- `[ ]` **B.r15 — Per-screen verification**. Once the engine renders, each DEF-driven screen should "just work" from the `.def` content. **12 per-screen activators identified** (Wave-2C: connect / connectsimple / savegame / loadgame / options / popup / hostgame / joingame / createchar / ingamemenu / mpingame; Wave-3C: selstart). **userinfo** activation routes through `FUN_00463149_MPLobby_ButtonDispatch` (Wave-3C — multiplayer-lobby button dispatcher branching on 7 button names; userinfo is one of its 7 branches, not a standalone activator). Verify each screen against retail behavior + `--test=ui-options`, `--test=ui-savegame`, etc.

### Tier 10 — Special-cased non-HUD screens (NEW)

These are neither pure-HUD panes nor DEF-driven; they have their own special-case classes:

- `[x]` **B.r16 — TLogoScreen = `cls_0x5a5d18`** — Wave-4B confirmed this IS the main menu: Initialize loads `menus.dat`, creates 5 buttons (NewGame / LoadGame / Multi / Options / Exit), Animate paints `Bitmap("MainMenu")` + `"Revenant v%d.%02d"` version string. Folds B.r20 into this.
- `[x]` **B.r17 — TDeathPane = `cls_0x5b93c4`** + bonus retail wrapper `TDeathScreen = cls_0x5b9374`. See Tier 5 entry for full evidence. Bare-class rename pending OOAnalyzer-flatten scrub (now unblocked).
- `[x]` **B.r18 — TBookPane = `cls_0x5b5808`** + **TScrollPane = `cls_0x5b5750`** (Wave-4B). See Tier 5 entry.
- `[x]` **B.r19 — Credits = Smacker video, no engine class.** Wave-4B `find data -iname "*credit*"` returned only `data/Disk2/MIX_CREDITS.SMK`. Credits playback rolls into burndown C.0 (Smacker decoder).
- `[x]` **B.r20 — Main menu confirmed = TLogoScreen** (folded into B.r16). No separate TMainMenuScreen.

### Tier 11 — sidebar tab content classes (NEW — per user 2026-05-16 grouping)

The 6-tab sidebar (per `TSideTabsPane` Wave-1A finding) splits visually into two content regions:

**Upper sidebar — modal state `DAT_0065d1b8` (0/1/2):**
- `[~]` **B.r21 — Equip tab (upper, mode 0) = `cls_0x5a55dc`** (Wave-4A role descriptor `EquipSidebarPane`; global @ `0x65b7e0`). Likely src `TEquipPane` (src/equip.h) but src-class promotion deferred per AGENT_PROTOCOL Rule 1 (needs vtable-by-vtable verification). Leaner plain-`TPane` subclass (no DEF intermediate) per Wave-4A structural finding.
- `[~]` **B.r22 — Stats tab (upper, mode 1) = `cls_0x5a5ba0`** (Wave-4A confirmed Wave-2B candidate; role descriptor `StatsSidebarPane`; global @ `0x65b140`, ctor `FUN_00488910`). Likely src `TStatPane` (src/statpane.h); src-class promotion deferred. Uses TButtonPane-w-DEF intermediate `cls_0x5a45c8` for scrollable-content machinery.
- `[~]` **B.r23 — Book tab (upper, mode 2) = `cls_0x5a5ae8`** (Wave-3B role descriptor `SpellbookSidebarPane` — player's spell BOOK; UI label is "Book" per Wave-4A button mapping). NOT a 1:1 with any src class. Global @ `0x65a9d8`, ctor `FUN_00488620`. Uses TButtonPane-w-DEF intermediate. See `briefs/B_r9_parchment_viewer.md` + `briefs/B_r11_sidebar_tab_cascade.md`.

**Lower sidebar — modal state `DAT_0065d1bc` (0/1/2):**
- `[~]` **B.r24 — Inv tab (lower, mode 0) = `cls_0x5a58c0`** (Wave-4A role descriptor `InventorySidebarPane`; global @ `0x65d4f8`, ctor `FUN_00487f50`, dtor `FUN_00537980`). Likely src `TInventory` (src/inventory.h); src-class promotion deferred. Leaner plain-`TPane` subclass.
- `[~]` **B.r25 — Map tab (lower, mode 1) = `cls_0x5a5658`** (Wave-4A role descriptor `MapSidebarPane`; global @ `0x65b4f0`, dtor `FUN_0052a400`). Likely src `TAutoMap` (src/automap.h) — PERFECT yaml mapping per `RECON_UI_COVERAGE.md`; src-class promotion deferred. Uses inline-vtable-init (no separate ctor function — different pattern from the other 4 sidebar panes).
- `[~]` **B.r26 — Spell tab (lower, mode 2) = `cls_0x5a5978`** (Wave-4A role descriptor `QuickSpellSidebarPane`; global @ `0x6661b0`, ctor `FUN_00488460`). Src match UNCLEAR — pre-release `TSpellPane` is the talisman composer (Wave-3B established that's a different role). Possibly a new retail class with no src counterpart.

See [briefs/B_r11_sidebar_tab_cascade.md](briefs/B_r11_sidebar_tab_cascade.md) for the full cascade, button-to-pane mapping, and visibility model confirmation. All 6 sidebar content panes have cls_0x identifications; Wave-5 candidate is the src-class promotion sweep.

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
