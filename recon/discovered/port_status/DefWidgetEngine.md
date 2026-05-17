# DEF Widget Engine — port status

Engine that parses per-screen `.def` files (`options.def`, `popup.def`, `savegame.def`, `loadgame.def`, `joingame.def`, `hostgame.def`, `ingamemenu.def`, `mpingame.def`, `selstart.def`, `createchar.def`, `userinfo.def`, `connect.def`, `connectsimple.def`, and the popup variants) into a runnable widget screen.

Identified by Wave-2C 2026-05-16 — see `docs/ui/briefs/B_r7_def_widget_engine.md`.

This is engine-identification, NOT a class. Per AGENT_PROTOCOL Rule 2-sibling: most of these are bare/static functions Ghidra missed during the OOAnalyzer pass — extracted on demand to `recon/discovered/FUN_*.cpp`.

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
