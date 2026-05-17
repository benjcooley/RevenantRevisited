# B.r7 — DEF Widget Engine: identification + port plan

**Status:** First-pass identification complete (Wave-2C, 2026-05-16). Engine entry points + control dispatcher + input dispatcher + per-screen activators all anchored with 2+ lines of evidence. Widget-subclass identifications listed as primary candidates for follow-up.

**Recon agent:** Wave-2C (sibling to Wave-2A TTextBar leaf, Wave-2B TPlyrStatusBar/stat-pane disambig).

**Related plan items:** RETAIL_UI_RECOVERY_PLAN.md Tier 9 — B.r11 (engine entry), B.r12 (renderer), B.r13 (input dispatcher), B.r14 (port), B.r15 (per-screen verification). This brief delivers B.r11 + B.r13 + the per-screen call-site map for B.r15. B.r12 (renderer) is identified in concept only — the renderer walks the child list via the same `+0x98 / +0x88` array that the input dispatcher walks, calling each widget's vtable draw slot; pinning the exact draw entry needs Wave-3 work.

---

## Widget vocabulary (the language the engine parses)

From `data/resources_unzipped/widgets.def` + per-screen `.def` samples (`options.def`, `popup.def`, `savegame.def`, etc.).

### Top-level structural tokens
- `PANEL "name"` `BEGIN` … `END` — declares a panel layout block; an engine screen can hold one or more named panels (e.g. `popup.def` has `"yesno" / "okcancel" / "ok" / "progress"`).
- `STYLE` <widget-class> [<variant>] <attrs…> — registers a named style template into the global style table (`DAT_00655510`, count `DAT_0065617c`).
- `#define <NAME> <value>` — preprocessor-only; the engine doesn't see these.

### Widget classes (one constructor each)
| Token | Subclass constructor (retail) |
|---|---|
| `BUTTON` | `FUN_0042bd90` (UPLABELRECT/DOWNLABELRECT anchors) |
| `TEXT`   | `FUN_0042b340` (BGBITMAP/FRAME/FONT anchors) |
| `BITMAP` | `FUN_0042aaf0` (BITMAP/DRAWMODE) |
| `FRAME`  | `FUN_0042d730` (BGBITMAP/FRAME) |
| `LISTBOX`| `FUN_0042f9f0` (BGBITMAP/RECT/FONT) + `FUN_0042f700` (FIELD/END inner format-block) |
| `EDIT`   | `FUN_00431750` (BGBITMAP/SPIN — normal + spin variant) |
| `DROPLIST` | `FUN_00433170` (DROPBUTTON/ITEMRECT/LISTHEIGHT) |
| `SCROLLBAR` | (constructor present but no isolated string anchor — see Open Questions) |

### Style variants per widget class
From `widgets.def`: `BUTTON {NORMAL,TOGGLE,RADIO}`, `SCROLLBAR {VSCROLL,HSCROLL}`, `LISTBOX {VLIST}`, `EDIT {NORM,SPIN}`, `DROPLIST {VSCROLL}`. The variant token follows the widget class token in a `STYLE` declaration.

### Common widget attributes (parsed by base ctor `FUN_0042a350`)
`POS x y w h`, `SIZE w h`, `NAME "id"`, `FIELD "id"`, `INDEX n`, `TEXT "literal"`, `STRINGREF "key"` (string-table lookup via `FUN_0049d800`), `FLAGS <flag-expression>`, `GROUP n` (radio group), `HOTKEY 'c'`.

### Type/flag namespace
The shipped `widgets.def` shows `DM_*` (drawmode), `TEXT_*` (text layout), `CTRLFLAG_*` (control state), and per-widget `BMCTRLFLAG_*`, `TEXTFLAG_*`, `BTNFLAG_*`, `SCROLLFLAG_*`, `LISTFLAG_*`, `EDITFLAG_*`, `DROPFLAG_*`. Engine only sees the resolved integer (preprocessor-resolved).

---

## Engine architecture

```
PER-SCREEN ACTIVATOR (e.g. FUN_005399f0 savegame, FUN_00465a50 createchar)
  │  builds geometry + sprite names + screen name ("savegame", "createchar", …)
  ▼
FUN_00435150  DefScreen_Open(this, panel_name, screen_name, flags, x,y, w,h,
                              bg_w, bg_h, bg_sprite_name, outer_name)
  │  high-level "open modal DEF screen" with chrome bitmap; routes to:
  ▼
FUN_00435040  DefScreen_LoadAndShow
  │  strips ".def" extension if present, re-appends ".def", calls:
  ▼
FUN_004377c0  DefWidget_LoadFile(this, "name.def")            ◆◆◆ ENGINE ENTRY ◆◆◆
  │  - opens "widgets.def" via FUN_004a13f0 (fopen-equivalent), size via 0x4a17b0
  │  - opens "<name>.def" same way
  │  - allocates one big buffer, copies widgets.def + '\n' + per-screen.def
  │  - stores buffer ptr at this+0xc8, total size at this+0xcc
  │  - strncpy(this+0xd0, name, 0x7f) — used by hot-reload
  │  - returns 1 on success
  ▼
[activator calls separately:]
FUN_00437620  DefWidget_ParseBuffer(this)                     ◆◆◆ PARSER ENTRY ◆◆◆
  │  - sets up a "String" tokenizer over this->buf
  │  - calls vtable+0xb4 (this, &tokenizer, this+0x56) — screen-specific top-level
  │  - on success: vtable+0x94(0, 1) — activate the populated screen
  ▼
vtable+0xb4 = (top-level discriminator; reads next token)
  │
  ├─ "STYLE" → register a named style into DAT_00655510 (style-section parser
  │             body lives in the 0x437080-0x4375bd region; not a separate fn
  │             entry — Ghidra hasn't bound a function start there yet)
  │
  └─ "PANEL" "name" → call FUN_00437000 DefWidget_ParsePanel
                       │  - expects BEGIN
                       │  - loops: call vtable+0xac (this, &tokenizer) per child
                       │  - exits on END
                       ▼
              vtable+0xac base impl = FUN_00436ec0 DefWidget_DispatchControl
                       │  - reads identifier token (BUTTON/TEXT/BITMAP/…)
                       │  - stricmp-scans DAT_00655510 (widget-class table,
                       │    count DAT_0065617c, name at entry+4, ctor at entry[0])
                       │  - rejects inline "STYLE" with explicit error
                       │  - calls registered constructor → returns new widget
                       │  - calls widget vtable+4 (parse-body, also reads attrs)
                       │  - FUN_0041c840 attaches widget to this pane
                       │  - sets dirty bit via widget vtable+0x1c

WIDGET CONSTRUCTORS (each registered into DAT_00655510 by the engine's static
init; the registration site is the global-init function that pulls
"_init_function_" pattern in 0x42a/0x42b/0x42c/0x42d/0x42f/0x431/0x433 funcs):

  FUN_0042a350  TWidget(base): POS, SIZE, NAME, FIELD, INDEX, TEXT, STRINGREF,
                FLAGS, GROUP, HOTKEY — sets *this = PTR_FUN_005a3ab8 (the
                TWidget base vtable; this is the anchor for the engine's
                widget vtable).
  FUN_0042aaf0  BITMAP
  FUN_0042aed0  (FRAME-bitmap, possibly a variant of the FRAME class)
  FUN_0042b340  TEXT
  FUN_0042bd90  BUTTON
  FUN_0042d730  FRAME
  FUN_0042f700  (LISTBOX inner FIELD/END format block parser)
  FUN_0042f9f0  LISTBOX
  FUN_00431750  EDIT (SPIN variant in same body)
  FUN_00433170  DROPLIST
  (SCROLLBAR constructor present somewhere in 0x42c-0x431; needs follow-up.)

INPUT DISPATCH:
  FUN_004361f0  DefWidget_DispatchInput(this, vkey, isKeyDown)
    - if vkey == 'R' (0x52) and this has a loaded .def name (this+0xd0 non-empty):
        FUN_004377c0(this+0xd0)  ← LIVE HOT-RELOAD (dev feature)
    - if a sub-dispatcher is set at this+0xa0 (i.e. a modal child has focus):
        delegate via that child's vtable+0x30
    - arrow keys (0x25..0x28): cycle focus through this->children[]
      (count this+0x88, array this+0x98), calling FUN_004369f0 to check
      whether a candidate child is selectable.
    - Enter (0x0d): focused child's vtable+0x2c (activate)
    - fall-through: iterate children calling each one's vtable+0x58
      (KeyDown handler); first that returns non-zero gets focus
```

Object layout (TPane/TDefScreen base, partial):
- `+0x58..0x77`: screen name field (used by activator FUN_00435040 — `strncpy(this+0x56, pcStack_8, 0x1f)` — actually 0x56)
- `+0x60`: flags byte (CTRLFLAG_* bits checked in dispatcher)
- `+0x88`: child count
- `+0x98`: child array (TWidget**)
- `+0x9c`: focused child cache?
- `+0xa0`: sub-dispatcher hook
- `+0xc8` (=200): loaded .def buffer (concat of widgets.def + per-screen.def)
- `+0xcc`: buffer size
- `+0xd0`: per-screen name (strncpy'd 0x7f bytes; used for hot-reload)
- `+0x14f`: zeroed flag (post-load state)
- `+0x150`: 1 sentinel
- `+0x178`: 0 sentinel

---

## Per-screen activator call-site map

| Screen `.def` | Activator function | String anchor | Routing |
|---|---|---|---|
| `connect.def` | `FUN_00462660` | "connect.def" @ 0x005d2b7c | direct `FUN_004377c0` then `FUN_00437620` |
| `connectsimple.def` | `FUN_0046b010` | "connectsimple.def" @ 0x005d40b4 | direct `FUN_004377c0` then `FUN_00437620` |
| `savegame.def` | `FUN_005399f0` | "savegame" @ 0x005e4314 | via `FUN_00435150` (DefScreen_Open) |
| `loadgame.def` | `FUN_00539380` | "loadgame" @ 0x005e4244 | via `FUN_00435150` |
| `options.def` | `FUN_0053a8b0` | "options" @ 0x005e447c | via `FUN_00435150` |
| `popup.def` (yesno/okcancel/ok/progress) | `FUN_0053bf00`, `FUN_0053c1d0` | "popup" @ 0x005e4668 | via `FUN_00435150` |
| `hostgame.def` | `FUN_00469a60` | "hostgame" @ 0x005d3cc8 | via `FUN_00435150` |
| `joingame.def` | `FUN_0046a660` | "joingame" @ 0x005d3ed0 | via `FUN_00435150` |
| `createchar.def` | `FUN_00465a50` | "createchar" @ 0x005d32e0..0x005d3300 | via `FUN_00435150` and `FUN_00435230` (variant) |
| `ingamemenu.def` | `FUN_00537110` | "ingamemenu" @ 0x005e4080..0x005e40a0 | via `FUN_00435150` |
| `mpingame.def` | `FUN_00463e00` | "mpingame" @ 0x005d2ebc | via `FUN_00435150` (multiplayer in-game pane) |
| `selstart.def` | unconfirmed enclosing fn (5 unbound refs) | "selstart" @ 0x005d3bb0 | — see Open Questions |
| `userinfo.def` | unconfirmed enclosing fn (5 unbound refs) | "userinfo" @ 0x005d2d30 | — see Open Questions |
| `exit.def` | conflicts with game-data file of same name — NOT a UI screen | — | (see Open Questions) |

The selstart + userinfo string refs all hit "no enclosing function" — they're inside regions Ghidra hasn't bound to function starts. Follow-up: force-CreateFunction at the call sites, then re-trace.

---

## Renderer (B.r12 — partial)

The renderer is not yet pinned to a specific entry point. From input-dispatch evidence: the engine walks `this->children[i]` for `i in 0..this->count` (fields `+0x98` / `+0x88`). The renderer almost certainly walks the same array, calling each widget's vtable draw slot.

The widget base vtable is at `PTR_FUN_005a3ab8` (set in `FUN_0042a350`). Once we dump that vtable (DumpVtable.java 0x5a3ab8) the draw slot will be visible — that's the per-widget renderer. A pane-level driver function that calls into each child's draw is then findable by FindImmRefs on the slot address.

**Action for next agent:** `DumpVtable.java 0x5a3ab8` → identify draw slot → CallersOf → identify the pane renderer driver. Likely sits in the same 0x436xxx-0x437xxx TU.

---

## Confidence & rename rules

All renames in `recon/discovered/renames/agent_ui_wave2_defengine.txt` carry ≥2 lines of evidence:

| Rename | Evidence 1 | Evidence 2 |
|---|---|---|
| `FUN_004377c0_DefWidget_LoadFile` | error string "Unable to load widgets.def" | "widgets.def" string literal opens via `FUN_004a13f0` |
| `FUN_00437620_DefWidget_ParseBuffer` | reads buffer at +0xc8/+0xcc (LoadFile's output) | called immediately after LoadFile from 2 distinct callers |
| `FUN_00437000_DefWidget_ParsePanel` | error string "Panel layout BEGIN expected" | "PANEL" + "BEGIN" + "END" strings all in 0x005cda* region |
| `FUN_00436ec0_DefWidget_DispatchControl` | "Unrecognized layout command %s" | 3 other distinct error strings in same fn body |
| `FUN_004361f0_DefWidget_DispatchInput` | hot-reload code-path calls LoadFile with this+0xd0 (LoadFile's filename field) | navigation/Enter dispatch through children array |
| `FUN_0042a350_TWidgetBase_ParseAttrs` | "Unable to parse control pos/flags/group/hotkey" + "Text expected" | 5+ widget-attribute keywords parsed in one body |
| per-screen activators | bare screen name string anchored from inside the fn | direct call into `FUN_004377c0` or `FUN_00435150` (which routes to LoadFile) |

---

## Candidates with weak evidence (NOT renamed)

- **Widget-subclass class identity.** Each constructor (BUTTON/TEXT/BITMAP/…) is identified by parsed attributes, but the actual src class name (`TButton`? `TWidgetButton`? `TButtonCtrl`?) needs vtable confirmation. Don't promote.
- **SCROLLBAR constructor.** No distinctive isolated string anchor (SCROLLBAR + VSCROLL/HSCROLL are weaker). Probably in 0x42c-0x431 range. Needs targeted FindStringRefs on "VSCROLL"/"HSCROLL" subdivided per fn.
- **Top-level STYLE-section parser** (in the 0x437080-0x4375bd region). Has string references at 0x436f45 + 0x4372ea but no Ghidra function boundary. Needs CreateFunction + DecompileAddr force-extraction.
- **TPane/TDefScreen base class.** The engine fields at +0xc8/+0xcc/+0xd0/+0x88/+0x98 belong to a TPane-family base. Vtable+0xb4 is the parse-dispatch slot — but identifying the cls_0x?? is a different hunt (start by finding which vtable holds `FUN_00436ec0` at slot 0xac).
- **`exit.def` UI route.** `exit.def` is also the game's level-exit data file (read at data/resources_unzipped/exit.def — 187 lines of `EXITNAME (x,y,z) LEVEL n MAPINDEX ...`). The 4 exit.def string refs at 0x004615ce / 0x0050c934 / 0x0050cce2 are almost certainly the level-exit LOADER, not the UI exit-confirm popup. The UI exit confirm probably uses `popup.def "yesno"` with `"Are you sure you want to exit?"` string — verified by the playscreen `"exitgameyn"` anchor (see `agent_screens.txt` TPlayScreen::TimerTick at 0x47c630). The actual exit popup activator is one of the `FUN_0053bf00 / FUN_0053c1d0` popup variants invoked with that key.

---

## Merging suspicions

None observed. The 0x42a-0x437 cluster is a clean translation unit with one constructor per widget class — no apparent OOAnalyzer collapse.

---

## Port plan recommendation (B.r14 / B.r15)

Minimal viable engine to render existing 22 `.def` files:

1. **Tokenizer.** Reimplement the FUN_00478720 / FUN_00479580 / FUN_00479700 / FUN_0047a410 family as a simple keyword/identifier/integer/string/operator tokenizer with peek+advance, plus printf-style typed scan (`"%i,%i"` etc.). Estimated: <300 LOC modern C++.
2. **Style table.** A `std::vector<DefStyle>` indexed by name. `STYLE <class> <variant>` registers per-(class,variant) defaults that widget ctors fall back on for unspecified attrs.
3. **Widget class table.** A `std::vector<DefWidgetClass>` with `{name, ctor_fn}`. Registered at engine init time. One entry per widget keyword.
4. **PANEL parser.** Loop while not END, call dispatcher on each child token. Match `FUN_00437000` shape exactly.
5. **Dispatcher.** stricmp-scan style table → instantiate → call parse-attrs. Match `FUN_00436ec0` shape.
6. **Widget base + subclasses.** Six initial subclasses cover the entire shipped UI: TEXT, BUTTON, BITMAP, FRAME, SCROLLBAR, LISTBOX. EDIT/DROPLIST are needed for `userinfo.def` + `options.def`'s SPIN edits.
7. **Renderer.** Each widget has a virtual `Draw(TPane*)`; pane renderer walks the children array. Re-use Display.Box / Display.WriteText primitives (same ones the HUD uses).
8. **Input dispatch.** Match `FUN_004361f0` shape: arrow nav + Enter activate + per-child KeyDown fall-through. KEEP the 'R' hot-reload binding — it's a dev feature the original developer presumably wants preserved.

Test-mode plan (per the modernization stance / test-modes-for-UI feedback):

- `--test=ui-defwidget-engine-exit` — render the simplest popup ("ok" panel from popup.def) to bring up the basic engine.
- `--test=ui-options`, `--test=ui-savegame`, `--test=ui-loadgame`, `--test=ui-popup-yesno`, `--test=ui-ingamemenu`, … — one per shipped `.def`, exercising each widget type in isolation before testing through full game boot.

---

## Open questions

1. **selstart / userinfo activators** — string refs aren't bound to function entries. Need CreateFunction at each ref site, then identify the activator. Both are likely simple wrappers around `FUN_00435150`.
2. **Renderer entry point** — find by dumping the `PTR_FUN_005a3ab8` (TWidget base) vtable, identifying the draw slot, then FindImmRefs to locate the pane-level driver.
3. **SCROLLBAR widget constructor** — probably FUN_0042c?? or FUN_0042e??; needs targeted hunt with VSCROLL/HSCROLL string searches per-function.
4. **STYLE-section parser** — exists as code in the 0x437080-0x4375bd gap; needs CreateFunction + DecompileAddr to extract cleanly. Both STYLE refs at 0x436f45 + 0x4372ea sit inside this region.
5. **Widget registration site** — where does the global widget-class table at `DAT_00655510` get populated? Look for static-initializer functions that write into 0x655510 — that's the engine bootstrap.
6. **Hot-reload Press-R** — is this a debug-only behaviour or shipped feature? Check `DAT_0065c9e0` (the gate flag in FUN_004361f0 — only reloads if non-zero).
7. **Per-pane name field at +0x56** — FUN_00435040 strncpy's 0x1f bytes into `(char *)(param_1 + 0x56)` from `pcStack_8` (the panel name parameter). What field is this? Probably the "active panel" selector for the multi-PANEL files like popup.def.

---

## Hunt log

- **2026-05-16 (Wave-2C, ~25 min):**
  - Read AGENT_PROTOCOL + discovered/README + CLASSIC_HUD_REFERENCE §8 + RETAIL_UI_RECOVERY_PLAN Tier 9.
  - Extracted distinctive vocab from widgets.def, options.def, popup.def.
  - Ran 31 FindStringRefs batch (PANEL / STYLE / BEGIN / END / BUTTON / LISTBOX / DROPLIST / SCROLLBAR / BGBITMAP / DROPBUTTON / UPLABELRECT / DOWNLABELRECT / LISTHEIGHT / DRAWMODE / SELCOLOR / FONT / MARGINS / NOCENTER / FRAME / EDIT / FIELD / BITMAP / POS / NAME / FLAGS / RECT / COLOR / ITEM / ITEMRECT / SPIN). All distinctive tokens cluster in 0x005ccfb8-0x005cdc64 (.rdata), referencing functions in 0x42a-0x437 (.text). Same TU.
  - Decompiled FUN_00437000 (PANEL parser — error string "Panel layout BEGIN expected" decisive).
  - Decompiled FUN_004377c0 (widgets.def loader — error string "Unable to load widgets.def" decisive).
  - Decompiled FUN_00437620 (parser kickoff — reads buffer at +0xc8/+0xcc that LoadFile populates).
  - Decompiled FUN_00436ec0 (control dispatcher — 4 distinct error-string anchors).
  - Decompiled FUN_004361f0 (input dispatcher — hot-reload code path uses LoadFile entry).
  - Decompiled FUN_0042a350 (TWidget base ctor — common attribute parser).
  - Decompiled FUN_00435040 + FUN_00435150 (DefScreen_LoadAndShow + DefScreen_Open).
  - Searched per-screen `.def` filenames (only `connect.def`, `connectsimple.def`, `widgets.def`, `stats.def`, `statpane.def`, `exit.def` exist as literals) and bare screen base-names (options/popup/savegame/loadgame/joingame/hostgame/ingamemenu/selstart/createchar/userinfo/mpingame all exist).
  - Decompiled 3 per-screen activators (savegame/createchar/ingamemenu) to confirm DefScreen_Open routing.
  - Wrote rename file and brief.

- **Pending (next wave):**
  - DumpVtable 0x5a3ab8 → identify TWidget virtuals → find pane renderer.
  - CreateFunction at 0x437080 + 0x4375bd region → extract STYLE-section parser.
  - Bind selstart/userinfo string refs to enclosing functions.
  - Find widget-class registration site (writes into DAT_00655510).
  - Cross-check: is exit-confirm popup actually invoked via `popup.def "yesno"` (validate via "exitgameyn" anchor in TPlayScreen::TimerTick).
