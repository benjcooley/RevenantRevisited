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

| Addr | Class | Status | Notes |
|---|---|---|---|
| `0x0042a350` | `TWidget` base attrs ctor | ⚫ not-started | Sets vtable PTR_FUN_005a3ab8. Parses POS/SIZE/NAME/FIELD/INDEX/TEXT/STRINGREF/FLAGS/GROUP/HOTKEY. |
| `0x0042aaf0` | `BITMAP` widget | ⚫ not-started | Anchors: BITMAP/DRAWMODE strings in body. |
| `0x0042aed0` | (FRAME-bitmap variant) | ⚫ not-started | Possibly an alternate BITMAP/FRAME variant; needs class identity confirmation. |
| `0x0042b340` | `TEXT` widget | ⚫ not-started | Anchors: BGBITMAP/FRAME/FONT. |
| `0x0042bd90` | `BUTTON` widget | ⚫ not-started | Anchors: UPLABELRECT/DOWNLABELRECT. |
| `0x0042d730` | `FRAME` widget | ⚫ not-started | Anchors: BGBITMAP/FRAME. |
| `0x0042f700` | `LISTBOX` inner FIELD/END | ⚫ not-started | Parses per-row format inside a LISTBOX BEGIN…END block. |
| `0x0042f9f0` | `LISTBOX` widget | ⚫ not-started | Anchors: BGBITMAP/RECT/FONT. |
| `0x00431750` | `EDIT` widget | ⚫ not-started | Anchors: BGBITMAP/SPIN. Normal + spin variant in same body. |
| `0x00433170` | `DROPLIST` widget | ⚫ not-started | Anchors: DROPBUTTON/ITEMRECT/LISTHEIGHT. |
| (TBD) | `SCROLLBAR` widget | ⚫ not-started | Constructor not yet pinned to a single function — likely in 0x42c-0x431 range. |

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
| selstart | (unbound) | 🔴 incomplete | Force-CreateFunction at unbound string refs to identify. |
| userinfo | (unbound) | 🔴 incomplete | Same — needs CreateFunction at unbound refs. |

## Renderer

Not yet pinned. Action: `DumpVtable.java 0x5a3ab8` → identify widget draw slot → CallersOf to find pane driver. Likely lives in the same 0x436xxx-0x437xxx translation unit.

## Outstanding work to land a working engine

1. Tokenizer port (FUN_00478720 family).
2. Widget class registry + 6 primary widget subclasses (TEXT/BUTTON/BITMAP/FRAME/SCROLLBAR/LISTBOX).
3. Panel parser + control dispatcher.
4. Renderer walker.
5. Input dispatcher (incl. R hot-reload as dev-mode feature).
6. EDIT/DROPLIST subclasses for advanced screens (options/userinfo).
7. Per-screen test modes (`--test=ui-defwidget-engine-<screen>`).

See `docs/ui/briefs/B_r7_def_widget_engine.md` for the full plan.
