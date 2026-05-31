# DEF Widget Engine — port status

Engine that parses per-screen `.def` files (`options.def`, `popup.def`, `savegame.def`, `loadgame.def`, `joingame.def`, `hostgame.def`, `ingamemenu.def`, `mpingame.def`, `selstart.def`, `createchar.def`, `userinfo.def`, `connect.def`, `connectsimple.def`, and the popup variants) into a runnable widget screen.

Identified by Wave-2C 2026-05-16 — see `docs/ui/briefs/B_r7_def_widget_engine.md`.

This is engine-identification, NOT a class. Per AGENT_PROTOCOL Rule 2-sibling: most of these are bare/static functions Ghidra missed during the OOAnalyzer pass — extracted on demand to `recon/discovered/FUN_*.cpp`.

---

## ✅ PORT LANDED (static render) — 2026-05-30 — `src/defscreen.{h,cpp}`

A modern, data-driven reimplementation of the DEF widget engine now lives in
`src/defscreen.{h,cpp}` (`TDefScreen` + `SDefWidget`/`SDefStyle`). It is NOT a
1:1 address-level port of the retail functions above — it reproduces the same
*behaviour* from the same shipped `.def` data:

- **Preprocessor + tokenizer**: strips `//` + `/* */` comments, joins `\`-newline
  continuations, parses `#define NAME VALUE` into a symbol table, evaluates
  `FLAGS A | B | C` expressions against it.
- **STYLE library**: parses every `STYLE <type> [<variant>] …` block in
  `widgets.def` into a `(type,variant)`-keyed style table; screen-local STYLE
  overrides (popup.def) merge on top.
- **PANEL parser**: builds the requested panel's widget tree, resolving each
  widget's style by type+flags and overlaying per-widget attribute overrides;
  handles the nested LISTBOX `BEGIN … FIELD … END` row format.
- **Renderer walker** (compose-to-target, NOMENCLATURE §3): BITMAP, FRAME
  (9-slice), TEXT (align/vcenter/shadow), BUTTON (NORMAL 9-slice + TOGGLE
  checkbox), SCROLLBAR (HSCROLL bar+arrows+thumb), LISTBOX (rows + selection
  fill + CTRLFLAG_CLEARBG), EDIT (frame + text). Assets resolve out of the
  mounted `resources.rvr` by name; "Med" font → Arimo-14 TTF.
- **New shared primitives** added to `TRenderer`: `DrawNineSliceToTarget`,
  `DrawSolidRectToTarget` (compose-to-target variants of the existing swapchain
  calls). Also added to UI_METHOD_MAP / RECONSTRUCTION_PROTOCOL toolbox.

**Verified screens** (test harness `src/uidefscreentest.cpp`):
`--test=ui-ingamemenu` (ingamemenu.def), `--test=ui-options` (options.def),
`--test=ui-savegame` (savegame.def), `--test=ui-loadgame` (loadgame.def). Each
visually matches its `docs/ui/forensics/*Def_SPEC.md` layout. The per-screen
DefScreen_Open pane rects (x/y/w/h + chrome dat) are host-side constants in the
harness's `kScreens` table (the .def carries pane-local coords only).

**Background variant selection** (confirmed against a retail Options screenshot
2026-05-30): each full-screen DEF screen ships up to three chrome `.dat`s:
`<screen>notex.dat` = engraved frame with a **transparent (black-keyed) interior**
(1998 software-renderer fallback / overlay form); `<screen>tex.dat` = ARGB4444
texture variant; `<screen>alpha.dat` = the **full pre-composited backdrop** (the
painted scene + frame + title plate). Screens reached from the main menu
(options, loadgame) ship the `alpha` scene variant and the port uses it
(`optionsalpha.dat` = the Ahkuilon vista; `loadgamealpha.dat` = the battle
vista). `savegame` is only reached in-game and has no `alpha` variant — its
`notex` chrome has a transparent interior meant to overlay the frozen game
frame. The harness `kScreens` table picks the right variant per screen.

**Controller-list layout CONFIRMED** (was OptionsDef_SPEC §4.5/§14 UNCONFIRMED-D):
the retail screenshot shows the key-binding list rendered in THREE columns
aligned to the headers — Control Name @x70 (white), Key 1 @x300 (white), Key 2
@x455 (yellow) — populated from the global controller table (Invoke 1..4,
Combat Combo 1..4, ...). The `name0..name7` "Up/Down/..." TEXT widgets are a dead
earlier-design overlay and are NOT drawn. (The test harness reproduces this with
3 synthetic column fields + suppressed name labels; real game-flow binding to the
controller table is still a TODO.)

**Scrollbar piece naming gotcha** (confirms / extends spec §0 asset notes): the
`.dat` stores scrollbar arrow/thumb pieces as `<base>U`/`<base>D` (button
up/down state), while `widgets.def` names only the base (`HScrollUp`,
`HScrollThumb`). The engine appends `U` for the unpressed static render.
Toggle/button faces (`CheckU`/`CheckD`, `ClearFrame2`/`FillFrame2`) are literal
full names — no suffix.

**Interactivity (ported 2026-05-30, verified via scripted headless filmstrips):**
- BUTTON press/release + click dispatch (down-state FillFrame2 + black label).
- TOGGLE checkbox flip on click (CheckU↔CheckD).
- HSCROLL slider click + drag → value (thumb tracks the cursor) + arrow ±1 step.
- LISTBOX row select on click (selection fill moves to the clicked row).
- EDIT text entry: click to focus (yellow editcolor + caret), VK→char typing +
  backspace, MAXLEN clamp. Keys route TestModes::HandleKeyPress → OnKey.
- Two runners: `tools/ui/defscreen_verify.sh` (basic) and
  `tools/ui/defscreen_widget_tests.sh` (exercises EVERY widget per screen +
  greps the per-screen log for button-dispatch). Both headless, filmstrip-out.
- Per-widget verification 2026-05-30: InGameMenu 6 buttons; Options 6 toggles +
  4 sliders + OK; SaveGame row-select + EDIT "hero save" + Save; LoadGame
  row-select + Load/Exit — all confirmed in the filmstrips + dispatch logs.

**Deferred (not yet ported — integration):**
- Options key-REBINDING flow: the controller LISTBOX is populated with SYNTHETIC
  rows in test. The real keymapper exists — `TControlMap ControlMap`
  (src/ctrlmap.{h,cpp}, extern in revenant.h), initialized in
  TPlayScreen::Initialize from `g_defaultGameControls[]` and used for runtime
  command dispatch (runtimemode.cpp `GetCommand`). Wiring TODO: populate the
  controller list from `ControlMap.NumControls/GetControlEntry` + `MakeKeyString`
  (name / key1 / key2 columns), and implement select-row→capture-keys→
  `SetControlEntry`→`Save("Controls")`. The standalone --test would need to
  `ControlMap.Initialize(...)` first (it's only init'd inside TPlayScreen today).
- LISTBOX auto-spawned scrollbar thumb (the gamelist gutter draws, no thumb yet).
- DROPLIST + EDIT SPIN variants (no in-scope screen uses them).
- Real game-flow integration (TPlayScreen opening the modal, command dispatch to
  loadgame/savegame/options) — the screens currently run only in test modes.

---

## Engine core (entry points)

| Addr | Identified name | Status | Notes |
|---|---|---|---|
| `0x004377c0` | `DefWidget_LoadFile(this, name)` | ⚫ not-started | Loads `widgets.def` + `<name>.def` → buffer at this+0xc8/+0xcc; stores name at this+0xd0. Anchor strings inside fn body. |
| `0x00437620` | `DefWidget_ParseBuffer(this)` | ⚫ not-started | Tokenizes the loaded buffer, calls vtable+0xb4 (top-level discriminator), activates via vtable+0x94. |
| `0x00437000` | `DefWidget_ParsePanel(this, tokenizer)` | ⚫ not-started | `PANEL "name" BEGIN ... END` body parser. |
| `0x00436ec0` | `DefWidget_DispatchControl(this, token)` | ⚫ not-started | Looks up identifier in `DAT_00655510` (widget-class table, count `DAT_0065617c`), instantiates registered widget. |
| `0x004361f0` | `DefWidget_DispatchInput(this, vkey, isDown)` | ⚫ not-started | Hot-reload (R), arrow nav, Enter activate, per-child KeyDown fall-through. |
| `0x00435040` | `DefScreen_LoadAndShow(this, name)` | ⚫ not-started | Strips/re-appends ".def", calls LoadFile, activates via vtable+0x2c. |
| `0x00435150` | `DefScreen_Open(this, panel, name, flags, geom..., bg)` | ⚫ not-started | High-level "open named modal DEF screen" entry. |
| (gap 0x437080..0x4375bd) | DefWidget_ParseStyleSection | ⚫ not-started | Body exists but Ghidra hasn't bound a function start; needs CreateFunction. |

## Widget base + subclass constructors

**Update (Wave-4C, 2026-05-17):** Wave-2C's "widget ctor" entries below are corrected — those functions are ATTRIBUTE PARSERS, not class constructors. The true parse-time ctors and factories are listed in the "Widget class roster" section that follows. See `docs/ui/briefs/B_r13_def_engine_closeout.md` and `recon/discovered/renames/agent_ui_wave4_def_relabels.txt` for the relabel.

| Addr | Role (corrected) | Status | Notes |
|---|---|---|---|
| `0x0042a350` | `TWidget` base parse-time init (`_TWidgetBase_ParseAttrs`) | ⚫ not-started | Sets vtable PTR_FUN_005a3ab8. Parses POS/SIZE/NAME/FIELD/INDEX/TEXT/STRINGREF/FLAGS/GROUP/HOTKEY. Called by every parse-time widget ctor with its type-id. |
| `0x0042a210` | `TWidget` base programmatic init (rename pending — 1 caller confirmed) | ⚫ not-started | Sets vtable PTR_FUN_005a3ab8. Direct-arg variant of `0x42a350`; used by helper code (e.g. LISTBOX-internal SCROLLBAR construction). |
| `0x0042aaf0` | `BITMAP` attr parser | ⚫ not-started | Sub-block parser; called from BITMAP ctor's ParseBody. |
| `0x0042aed0` | `FRAME` attr parser (BGBITMAP/FRAME/DRAWMODE composite) | ⚫ not-started | Called by `FUN_0042b090` (FRAME ctor). |
| `0x0042b340` | `TEXT` attr parser | ⚫ not-started | BGBITMAP/DRAWMODE/RECT/COLOR. |
| `0x0042bd90` | `BUTTON` attr parser | ⚫ not-started | Bitmap-name lookups (UPLABELRECT/DOWNLABELRECT). |
| `0x0042d730` | `FRAME` attr parser | ⚫ not-started | BGBITMAP/FRAME/DRAWMODE/RELR/RELB. |
| `0x0042f700` | `LISTBOX` inner FIELD/END | ⚫ not-started | Parses per-row format inside a LISTBOX BEGIN…END block. |
| `0x0042f9f0` | `LISTBOX` attr parser | ⚫ not-started | BGBITMAP/FRAME/DRAWMODE/RECT. |
| `0x00431750` | `EDIT` attr parser | ⚫ not-started | RECT + UP/DOWN spin RELR. |
| `0x00433170` | `DROPLIST` attr parser | ⚫ not-started | BGBITMAP/FRAME/DRAWMODE/RECT. |

### Widget class roster (Wave-4C 2026-05-17 — all 8 confirmed)

DAT_00655510 holds 8 records (count DAT_0065617c=8). Each record = `{ class_descriptor_vt*, name* }`. The class descriptor vtable has 3 slots, slot 0 = factory. DispatchControl calls `descriptor_vt[0](pane, token)` → factory allocates the widget + calls the real ctor.

| # | Keyword | Record | DescriptorVT | Factory | Sizeof | Ctor | InstanceVT | TypeID |
|---|---|---|---|---|---|---|---|---|
| 1 | BITMAP | 0x655dc8 | 0x5a3b18 | 0x437e00 | 0x98 | **0x42abe0** | 0x5a3b24 | 1 |
| 2 | FRAME | 0x655508 | 0x5a3b84 | 0x437ee0 | 0xc0 | **0x42b090** | 0x5a3b90 | 2 |
| 3 | TEXT | 0x656218 | 0x5a3bf0 | 0x437fd0 | 0xe0 | **0x42b5d0** | 0x5a3bfc | 3 |
| 4 | BUTTON | 0x6562d0 | 0x5a3c5c | 0x4380c0 | 0x148 | **0x42c6a0** | 0x5a3c68 | 4 |
| 5 | SCROLLBAR | 0x656388 | 0x5a3ccc | 0x438220 | 0x168 | **0x42df00** | 0x5a3cd8 | 5 |
| 6 | LISTBOX | 0x655f48 | 0x5a3d38 | 0x438380 | 0x1d0 | **0x430090** | 0x5a3d44 | 6 |
| 7 | EDIT | 0x656210 | 0x5a3da4 | 0x4385c0 | 0x148 | **0x431ca0** | 0x5a3db0 | 7 |
| 8 | DROPLIST | 0x655dc0 | 0x5a3e10 | 0x438930 | 0x240 | **0x433640** | 0x5a3e1c | 8 |

Helper (programmatic-only):
- **`0x42de00`** — SCROLLBAR programmatic ctor; same class as parse-time `0x42df00`, different invocation. Called only from LISTBOX ctor for the nested child scrollbar. (Was misidentified by Wave-3C as the SCROLLBAR parse-time ctor.)

Per-widget ctor + factory bodies extracted to `recon/discovered/FUN_<addr>_DefWidget_<Type>_{ctor,factory}.cpp` (8 ctor + 8 factory files).

## Per-screen activators

| Screen | Activator | Status | Notes |
|---|---|---|---|
| connect | `FUN_00462660` | ⚫ not-started | Direct `FUN_004377c0("connect.def")` + `FUN_00437620`. |
| connectsimple | `FUN_0046b010` | ⚫ not-started | Direct `FUN_004377c0("connectsimple.def")` + `FUN_00437620`. |
| savegame | `FUN_005399f0` | ⚫ not-started | Via DefScreen_Open. |
| loadgame | `FUN_00539380` | ⚫ not-started | Via DefScreen_Open. |
| options | `FUN_0053a8b0` | ⚫ not-started | Via DefScreen_Open. |
| popup | `FUN_0053bf00` + `FUN_0053c1d0` | ⚫ not-started | Two variants; one per popup-panel set. |
| hostgame | `FUN_00469a60` | ⚫ not-started | Via DefScreen_Open. |
| joingame | `FUN_0046a660` | ⚫ not-started | Via DefScreen_Open. |
| createchar | `FUN_00465a50` | ⚫ not-started | Via DefScreen_Open + variant DefScreen_Open2 (FUN_00435230). |
| ingamemenu | `FUN_00537110` | ⚫ not-started | Handles SP + MP variant ("mpingamemenu" + "ingamemenu" base names). |
| mpingame | `FUN_00463e00` | ⚫ not-started | Multiplayer in-game pane. |
| selstart | `FUN_00469370` | ⚫ not-started | Identified by Wave-3C 2026-05-16. 90-byte DefScreen_Open wrapper. |
| userinfo | (in `FUN_00463149` MPLobby_ButtonDispatch) | 🟡 partial | Wave-3C 2026-05-16: userinfo activation is the first branch of a 1850-byte 7-button MP-lobby dispatcher (force-extracted at unbound 0x463149). No standalone activator fn. |

## Renderer

Not yet pinned. Action: `DumpVtable.java 0x5a3ab8` → identify widget draw slot → CallersOf to find pane driver. Likely lives in the same 0x436xxx-0x437xxx translation unit.

## Widget subclasses (Wave-3C 2026-05-16)

See `docs/ui/briefs/B_r10_def_widget_subclasses.md` for full evidence.

**Architecture refinement.** The "widget ctor" functions Wave-2C labelled in §22-36 of this file (FUN_0042aaf0 BITMAP, FUN_0042b340 TEXT, FUN_0042bd90 BUTTON, FUN_0042d730 FRAME, FUN_0042f9f0 LISTBOX, FUN_00431750 EDIT, FUN_00433170 DROPLIST) are STYLE-ATTRIBUTE PARSERS, not true class constructors. They take 3-4 params and don't set a `this->vtable` pointer. The TRUE widget ctors are smaller adjacent functions that:

1. Set `*this = &PTR_FUN_005a3xxx` (the per-class widget vtable).
2. Call `FUN_0042a210(this, N, ...)` (the shared TDefWidget base-init helper) with a small widget-type-id `N`.
3. Initialize subclass-specific fields.

**Registry layout.** `DAT_00655510` is `DefWidgetClassRecord**` (count `DAT_0065617c = 8`). Each record:
- `+0x00`: vtable pointer (0x5a3xxx in .rdata)
- `+0x04`: name C-string (e.g. "SCROLLBAR")

Eight register thunks (each a small static-init Ghidra missed) populate the table at program start.

**One widget tentatively confirmed in Wave-3C, full roster pinned in Wave-4C:**

| Addr | Class | Vtable | Status | Notes |
|---|---|---|---|---|
| `0x0042de00` | `TDefScrollbar` programmatic ctor | `0x5a3cd8` | 🟡 corrected | Wave-3C labelled as the SCROLLBAR ctor; actually the *programmatic-init* variant called by LISTBOX for its nested scrollbar. The parse-time SCROLLBAR ctor is at `0x42df00`. See B.r13 §2. |

**Remaining 7 widget classes pinned Wave-4C (see "Widget class roster" table above).** Per-record FindImmRefs + per-factory DumpBytes + per-ctor decomp closed out the loop. See `docs/ui/briefs/B_r13_def_engine_closeout.md`.

## Outstanding work to land a working engine

1. Tokenizer port (FUN_00478720 family).
2. Widget class registry + 6 primary widget subclasses (TEXT/BUTTON/BITMAP/FRAME/SCROLLBAR/LISTBOX).
3. Panel parser + control dispatcher.
4. Renderer walker.
5. Input dispatcher (incl. R hot-reload as dev-mode feature).
6. EDIT/DROPLIST subclasses for advanced screens (options/userinfo).
7. Per-screen test modes (`--test=ui-defwidget-engine-<screen>`).

See `docs/ui/briefs/B_r7_def_widget_engine.md` for the full plan.
