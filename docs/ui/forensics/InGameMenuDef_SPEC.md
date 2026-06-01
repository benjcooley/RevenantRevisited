# InGameMenu DEF screen — forensics spec

DEF-driven ESC pause menu. SP variant only — `mpingamemenu` is out of scope per
task framing (multiplayer is a separate Revisited deferral).

## §0 — Sources & status

- **Class**: `cls_0x5b9480` (vtable @ `0x005b9480`). Hosts the SP+MP variants of
  the in-game pause menu. Subclass of the DEF-screen base
  `cls_0x5b93c4` (recon: `recon/classes/cls_0x5b93c4__vftable_5b93c4.cpp`),
  which is in turn a subclass of `cls_0x5a4494_TPane`
  (`recon/classes/cls_0x5a4494__vftable_5a4494.cpp`).
  Vtable layout: `recon/classes/cls_0x5b9480__vftable_5b9480.cpp:7-52`
  (slots 0=Open, 1=Close, 132=DefScreen_Open helper, 148=Command,
  176=ParsePanel, 180=PanelTag-hook).
- **Paint method**: there is no class-private paint method. The pane paints
  via the inherited DEF-engine renderer (DefScreen base
  `cls_0x5b93c4::FUN_00435cb0` and `FUN_00436090` — generic DEF child walker;
  recon `recon/classes/cls_0x5b93c4.cpp:222-256, 330-360`). All chrome and
  child widgets are produced from the parsed `ingamemenu.def` file.
- **Singleton instance**: `DAT_0066f748` (referenced from
  `recon/classes/cls_0x5a5320_TMapPane.cpp:8010-8012` —
  `cls_0x5a5320_TPlayScreen::meth_0x47e500` opens it via the Open/Pump/Close
  triple, see §10).
- **DEF asset**: `data/resources_unzipped/ingamemenu.def`
  (`/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/ingamemenu.def`),
  literal copy in §2.
- **Recon files read** (paths relative to repo root):
  - `recon/discovered/FUN_00537110_LoadDef_ingamemenu.cpp` — Open dispatcher.
  - `recon/classes/cls_0x5b9480.cpp:28-122` — `virt_meth_0x537110` (Open),
    `virt_meth_0x537170` (Close), `cls_0x5b9480` ctor at 0x537420, dtor at
    0x5374a0.
  - `recon/classes/cls_0x5b9480__vftable_5b9480.cpp` — class vtable.
  - `recon/classes/cls_0x5b93c4.cpp:81-143` — DefScreen_Open (`0x00435150`),
    `:299-360` — DrawBackground (`0x00435cb0`) and per-child draw
    (`0x00436090`), `:1410-1474` — ctor + dtor.
  - `recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp` — generic
    activator (concatenates ".def", calls LoadFile, then activates).
  - `recon/discovered/FUN_00435150_DefScreen_Open.cpp` — high-level wrapper
    (also the body Ghidra emitted as a free function — identical bytes to
    `cls_0x5b93c4::virt_meth_0x435150`).
  - `recon/classes/cls_0x49c830.cpp:14-74` — `meth_0x49c830` (pause-all
    AIL samples) and `meth_0x49c890` (resume-all). Called from
    Open/Close of the ingamemenu (see §10).
  - `recon/discovered/renames/agent_ui_wave2_defengine.txt:62-310` — full
    DEF widget engine architecture, all per-screen activators.
  - `recon/discovered/port_status/DefWidgetEngine.md:9-73` — engine status,
    widget class registry (8 widget types, factory addresses).
  - `recon/classes/cls_0x5a5320_TMapPane.cpp:7999-8045` —
    `cls_0x5a5320_TPlayScreen::meth_0x47e500` — open/pump/close menu and
    dispatch the returned command id (1/2/3/4/5) to the next DEF screen.
  - Raw disassembly extracted from `data/Revenant.exe`:
    - `0x00537110..0x00537170` — LoadDef_ingamemenu push order.
    - `0x00537190..0x005373f3` — `virt_meth_0x537190` command dispatcher
      (Ghidra did NOT emit a body for this slot; decoded here from disasm).
    - `0x00537400..0x0053741b` — `virt_meth_0x537400` KeyPress handler
      (ESC inside the menu → resume).
    - `0x00435040..0x004351f0` — DefScreen_Open / LoadAndShow param-decode.
  - Pre-existing UI spec convention: same chip/frame discipline as
    `TPlyrStatusBar_SPEC.md`.
- **Status**: `forensics-complete` for the SP variant under the engine
  contract — i.e., the spec pins **(a)** which DEF file and assets drive
  the screen, **(b)** the pane rect and dispatch glue, and **(c)** the
  per-button command map. Pixel-exact paint of each BUTTON / FRAME widget
  is deferred to the **DEF widget renderer** which is itself an unported
  shared engine (`DefWidgetEngine.md:106-115` — outstanding work). When
  the renderer lands, the values in §2/§4 are the per-pixel inputs it
  consumes; no panel-private paint exists.
- **Errors to close** in existing ports: none — no engine code has been
  written for this screen yet. Greenfield.
- **Snapshot note (supplementary — superseded by retail)**: there is no
  pre-release `src/*.cpp` for an "ingame menu" pane in the worktree. The
  snapshot's analogue is `src/playscreen.cpp` ESC handling, which is also
  pre-1999 and doesn't show this DEF-driven design. The DEF engine itself
  is the 1999 retail design and the only one in scope. (Verified by
  search: no `src/ingamemenu*` or `ingame_menu*` file exists.)

## §1 — Overview

The In-Game Menu is the **ESC pause modal** during gameplay. When opened:

1. All audio samples are paused (`cls_0x49c830::meth_0x49c830`,
   `recon/classes/cls_0x49c830.cpp:14-42`).
2. The simulation loop is suspended by the modal pump (`meth_0x48f040`,
   pumped from `cls_0x5a5320_TPlayScreen::meth_0x47e500:8009-8045`).
3. A 394×316 modal panel is drawn centred on the 640×480 display, with a
   character portrait on the left and a single column of six buttons
   (Load / Save / Options / Quit / Exit / Resume) on the right.
4. The user picks a button. The command code is returned to the caller,
   which then opens the appropriate sub-screen (loadgame.def,
   savegame.def, options.def) or unwinds the game (quit / exit / resume).

There is **exactly one instance** (`DAT_0066f748`) — the screen is a
singleton constructed at program start by the cls_0x5b9480 ctor at
`recon/classes/cls_0x5b9480.cpp:61-82`.

Visibility predicate: the screen is "open" while the modal pump
(`meth_0x48f040`) is running on this instance. Outside the pump, the
pane is destroyed/inactive; there is no game-loop draw path that would
hit it. Confirmed by the open/pump/close triple at
`cls_0x5a5320_TMapPane.cpp:8010-8012`.

For the SP variant (this spec), the gate is `DAT_0066829c == 0` (the
multiplayer flag; `FUN_00537110:9-12`). MP path is OUT OF SCOPE.

The screen is shown over a frozen game-frame backdrop — the DEF engine's
`background` sprite is `"background"` per `FUN_00435040:31` (`0x5cd8d4`),
which is the resource-cache default name for the current-frame snapshot.
The actual modal chrome is the `Background` bitmap from
`ingamemenutex.dat` (394×316 — matches the rect exactly).

## §2 — Asset roster (table)

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| Background | `ingamemenutex.dat` / `ingamemenunotex.dat` | `Background` | 394×316 | full modal chrome (frame + portrait recess + button column gutter, painted as one chunky bitmap) | whole | dump_dat.py `data/resources_unzipped/ingamemenutex.dat` (entry 0); see §0 |
| Picture | `ingamemenutex.dat` / `ingamemenunotex.dat` | `Picture` | 172×210 | placeholder portrait artwork. Referenced via `BITMAP NAME "picture" FIELD "picture"` — the FIELD binding lets the runtime swap in a save-game thumbnail or character imagery when one exists. Default = this static `Picture`. | whole | dump_dat.py entry 1 |
| ClearFrame2 | `widgetstex.dat` / `widgetsnotex.dat` | `ClearFrame2` | 64×57 | BUTTON-up sprite (9-slice) — shared, from `widgets.def` STYLE BUTTON NORMAL UP. Per ingame button cell 141×27. | 9-slice `FRAME 10 10 12 12 MARGINS 0 0 4 4` (widgets.def:90) | `widgets.def:88-93`, dump_dat.py entry 3 |
| FillFrame2 | `widgetstex.dat` / `widgetsnotex.dat` | `FillFrame2` | 64×57 | BUTTON-down sprite (9-slice) — pressed state, same frame metrics. | 9-slice `FRAME 10 10 12 12` | `widgets.def:89`, dump_dat.py entry 4 |
| ClearFrame1 | `widgetstex.dat` / `widgetsnotex.dat` | `ClearFrame1` | 64×57 | FRAME widget sprite (9-slice) — used by the `FRAME POS 24 62 176 215` line (portrait inset border). | 9-slice `FRAME 14 14 16 16 MARGINS 0 0 4 4` | `widgets.def:81-82`, dump_dat.py entry 0 |
| "Med" font | (font registry) | `Med` | n/a (TTF / 14px bitmap) | text font used by BUTTON labels and FRAME labels. Per the FONT.DEF `"Med"` slot — bitmap font in retail, mapped to Arimo-14 in port (cf. `TPlyrStatusBar_SPEC.md`). | n/a | `widgets.def:86,91,92,97-98,103-104` |

**Confirmed**: every asset listed above is present in
`/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/`
and decoded via `tools/ui/dump_dat.py`. No procedural fallbacks are
needed.

### §2.1 — Literal DEF body (decisive)

```
PANEL "ingamemenu"
BEGIN

  BITMAP POS 26 65 176 216 NAME "picture" FIELD "picture"
  FRAME POS 24 62 176 215

// Options
  BUTTON POS 212 63 141 27 NAME "load" TEXT "Load Game"
  BUTTON POS 212 100 141 27 NAME "save" TEXT "Save Game"
  BUTTON POS 212 137 141 27 NAME "options" TEXT "Game Options"
  BUTTON POS 212 174 141 27 NAME "quit" TEXT "Quit Module"
  BUTTON POS 212 211 141 27 NAME "exit" TEXT "Exit Program"
  BUTTON POS 212 248 141 27 NAME "resume" TEXT "Resume Game"

END
```

(Source:
`/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/ingamemenu.def:4-18`,
shipping retail asset, treated as authoritative.)

## §3 — Coordinate frames & surfaces

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| screen | framebuffer TL | (root) | (0,0) | the 640×480 framebuffer (`DAT_0065bb10` width=640, height=480 from cls_0x5b9480 ctor `mbr_0xc=0x280`, `mbr_0x10=0x1e0`) | `recon/classes/cls_0x5b9480.cpp:66-69` |
| pane-local | screen | TL | screen `(0x7e, 0x41) = (126, 65)` — passed as `param_4=0x7e, param_5=0x41` to DefScreen_Open from `FUN_00537110:0x53713f-0x537141` | the 394×316 modal pane. Width = `0x18a = 394`, height = `0x13c = 316` (`param_6, param_7`). Origin is **TL-anchored constant** — NOT centered programmatically. Note `(640-394)/2 = 123`, `(480-316)/2 = 82` → 126/65 are **off-center** by (+3, −17), i.e. the modal sits 3px right of horizontal centre and 17px above vertical centre. This is the LITERAL retail layout. | LoadDef_ingamemenu disasm `0x53713f-0x537143`, signature `recon/discovered/renames/agent_ui_wave2_defengine.txt:222-226` |
| chrome (Background bitmap) | pane | TL | pane `(0,0)` — DEF engine fills it to pane bounds | the 394×316 `Background` chrome sprite | inferred from DefScreen_Open `bg_sprite_name = "widgets"` / actual background fill at `FUN_00435040:31` and §0 (no per-bitmap POS = fills pane). |
| picture-cell | pane | TL | pane `(26, 65)` from `BITMAP POS 26 65 176 216` | the 176×216 portrait/thumbnail cell (Picture sprite, possibly swapped via FIELD) | `ingamemenu.def:7` |
| frame-border | pane | TL | pane `(24, 62)` from `FRAME POS 24 62 176 215` | the 176×215 9-slice inset around picture-cell. Slightly different rect from picture-cell: starts 2px left, 3px above, and is 1px shorter — frames the picture with a thin overlap. | `ingamemenu.def:8` |
| button-stack | pane | TL | pane `(212, *)` where * = `63 + i*37` for i=0..5 — buttons share x and stack with **pitch = 37 px** (vertical step between successive Y values: 100−63 = 37, 137−100 = 37, ...). | the 141×27 BUTTON cells | `ingamemenu.def:11-16` |
| button[i] cell | button-stack | TL | pane `(212, 63 + 37*i)` for i in 0..5 (load=0, save=1, options=2, quit=3, exit=4, resume=5) | one BUTTON's hit rect + sprite-blit cell. Each is 141×27. | `ingamemenu.def:11-16` |

### Composition formula

For any element placed in the DEF file at `POS X Y W H`, the final screen
coordinate is:

```
screen_x = 126 + X
screen_y = 65  + Y
```

i.e. the DEF coordinates ARE pane-local. The pane is TL-anchored to a
literal `(126, 65)` constant on the 640×480 stage. There is no centering
math at runtime.

### Pane backing store

The DEF engine uses TPane / TButtonPane inheritance — there are no
dedicated scratch surfaces beyond what TPane provides. The DEF engine
walks the child widget array (`this+0x98`, count `this+0x88`,
`cls_0x5b93c4::virt_meth_0x435cb0:235-256`) and draws each child
directly to the pane buffer. No mosaic / scratch surface is involved for
this screen.

**Direct-renderer contract**: the port composes the modal into one
offscreen `TSurface` RT (per
[NOMENCLATURE](NOMENCLATURE.md) §3), then `DrawSurface`s it to the HUD.

### Layout diagram (final screen-space coords)

```
screen
0                                                                       640
┌───────────────────────────────────────────────────────────────────────┐
│                                                                       │
│        ┌── pane @ (126, 65), 394×316 ───────────────────────────┐     │
│   65 ──┤                                                         │     │
│        │ (Background chrome 394×316 fills pane)                  │     │
│        │                                                         │     │
│        │  ┌─ Frame inset (pane-local 24,62; 176×215) ─┐          │     │
│        │  │┌ Picture cell (pane-local 26,65;          │          │     │
│        │  ││  176×216) — runtime swappable via FIELD  │          │     │
│        │  ││  "picture"                               │          │     │
│        │  ││                                          │          │     │
│        │  ││ (172×210 retail asset center-aligned     │          │     │
│        │  ││  inside the 176×216 cell, ~2px slack)    │          │     │
│        │  │└──────────────────────────────────────────┘          │     │
│        │  └────────────────────────────────────────────┘          │     │
│        │                                                         │     │
│        │                   button-stack pane-local (212, 63):    │     │
│        │                   ┌─[Load Game]─────────┐ 141×27 y=63   │     │
│        │                   ┌─[Save Game]─────────┐       y=100   │     │
│        │                   ┌─[Game Options]──────┐       y=137   │     │
│        │                   ┌─[Quit Module]───────┐       y=174   │     │
│        │                   ┌─[Exit Program]──────┐       y=211   │     │
│        │                   ┌─[Resume Game]───────┐       y=248   │     │
│        │                                                         │     │
│  381 ──└─────────────────────────────────────────────────────────┘     │
│                                                                       │
│                                                                  480  │
└───────────────────────────────────────────────────────────────────────┘
       126                                                       520
        (pane right edge = 126 + 394 = 520)
        (pane bottom = 65 + 316 = 381)
```

## §4 — Static element layout (table)

All `(x,y)` in **pane-local** space (the spec's chosen single space for this
screen). Final screen coord = pane `(x,y) + (126, 65)`. Frame column is
the §3 frame.

| element | frame | pane (x,y) | (w,h) | source rect | sprite | cite |
|---|---|---|---|---|---|---|
| Background chrome | pane | (0, 0) | (394, 316) | whole 394×316 | `Background` | DEF engine bg blit; bitmap size confirmed by dump_dat.py |
| Picture cell | pane | (26, 65) | (176, 216) | whole 172×210 source, centered or top-left within cell (UNCONFIRMED-A — needs BITMAP draw recon) | `Picture` (default) | `ingamemenu.def:7` |
| Frame inset | pane | (24, 62) | (176, 215) | 9-slice (cf. §2 ClearFrame1 frame `14 14 16 16`) | `ClearFrame1` | `ingamemenu.def:8` |
| BUTTON "load" | pane | (212, 63) | (141, 27) | 9-slice (`FRAME 10 10 12 12` per widgets.def:90) | up=`ClearFrame2`, down=`FillFrame2` | `ingamemenu.def:11` |
| BUTTON "save" | pane | (212, 100) | (141, 27) | 9-slice | up=`ClearFrame2`, down=`FillFrame2` | `ingamemenu.def:12` |
| BUTTON "options" | pane | (212, 137) | (141, 27) | 9-slice | up=`ClearFrame2`, down=`FillFrame2` | `ingamemenu.def:13` |
| BUTTON "quit" | pane | (212, 174) | (141, 27) | 9-slice | up=`ClearFrame2`, down=`FillFrame2` | `ingamemenu.def:14` |
| BUTTON "exit" | pane | (212, 211) | (141, 27) | 9-slice | up=`ClearFrame2`, down=`FillFrame2` | `ingamemenu.def:15` |
| BUTTON "resume" | pane | (212, 248) | (141, 27) | 9-slice | up=`ClearFrame2`, down=`FillFrame2` | `ingamemenu.def:16` |

**Verification of button stack:**
- All x values are exactly `212` (constant TL anchor in pane).
- Y deltas: `100−63=37`, `137−100=37`, `174−137=37`, `211−174=37`,
  `248−211=37`. Pitch = 37 px = 27 (button height) + 10 (gap). Confirmed.
- Last button bottom = `248 + 27 = 275` (pane-local) < pane height 316 ✓.
- Last button right = `212 + 141 = 353` (pane-local) < pane width 394 ✓.

There is no mirrored side — this is a single-sided menu.

## §5 — Draw order / composition

Draw is the generic DEF-engine paint walk
(`cls_0x5b93c4::virt_meth_0x435cb0` at `recon/classes/cls_0x5b93c4.cpp:222-256`):

1. **Background blit**: paint the `Background` chrome sprite (`this->mbr_0x84`
   = the loaded backdrop, set by `FUN_00435040:31`'s `vtable+0xa0` call with
   `"background"` and resolved to the `Background` resource). Drawn opaque
   into the pane buffer at pane-local `(0,0)` via the surface vtable+0x5c
   call (`recon/classes/cls_0x5b93c4.cpp:237-238` — uses
   `FUN_00438d80_BlitEffect_StructInit(buf, 0,0,0,0, sprite_w, sprite_h, 0x80000000)`
   then `(*+0x5c)(buf, sprite, 0, 0)` — opaque `DM_USEDEFAULT`).
2. **Per-child walk** (`recon/classes/cls_0x5b93c4.cpp:240-249`): iterate
   `this->field_0x98[i]` for i in 0..`this->field_0x88` (the child array,
   widgets in DEF declaration order):
   - BITMAP "picture" (i=0) — `(*piVar1 + 0x40)(in_stack_00000004)` =
     widget vtable+0x40 (Draw). The BITMAP widget's Draw blits the
     `picture` sprite into its `(26, 65, 176, 216)` cell.
   - FRAME (i=1) — vtable+0x40 = the FRAME widget's 9-slice draw of
     `ClearFrame1` (with `FRAME 14 14 16 16 MARGINS 0 0 4 4` per
     `widgets.def:81-82`) at `(24, 62, 176, 215)`.
   - BUTTON load (i=2), save (i=3), options (i=4), quit (i=5),
     exit (i=6), resume (i=7) — each draws via its widget vtable+0x40.
     BUTTON Draw uses `ClearFrame2` (up) or `FillFrame2` (down) as a
     9-slice (`FRAME 10 10 12 12 MARGINS 0 0 4 4`), then renders the
     `TEXT` label centered (`TEXT_CENTER | TEXT_VCENTER | TEXT_SINGLELINE
     | TEXT_SHADOW`) using `"Med"` font, white when up (`COLOR 255 255 255`,
     widgets.def:91) or black when down (`COLOR 0 0 0`, widgets.def:92).
3. **Focused child late-draw** (`recon/classes/cls_0x5b93c4.cpp:251-254`):
   `this->mbr_0xa8` is re-drawn last (covers focus highlight). At rest
   `mbr_0xa8 == 0` so no second pass.
4. **Effect-pipeline iterate** (`FUN_00436090:341-359`,
   `FUN_004aacb0_BlitEffect_Iterate`) — runs any registered shadows/glows
   from the global registry (§8 of UI_METHOD_MAP). Empty for this screen.

The whole composition is **single-pass into the pane buffer**; the
buffer is then blitted to the HUD swapchain by the screen manager (not
the pane). Standard direct-renderer contract.

There is no separate `DrawBackground`/`Draw`/`Update` split for this
class — `0x435cb0` and `0x436090` collectively handle the whole frame.

## §6 — Algorithms (pseudocode per helper)

### §6.1 — `LoadDef_ingamemenu` (this+0x537110)

The Open trampoline. Pure dispatcher. **DAT_0066829c==0** path only
(SP) — MP branch out of scope.

```
this_pane = &DAT_0066f748  // singleton TIngamemenuScreen
DAT_0066829c == 0 ? panel_name = "ingamemenu" : panel_name = "mpingamemenu"
DefScreen_Open(
    this        = this_pane,
    name_a      = "ingamemenu"          // s_005e40a0  // arg 1 (push order rev)
    panel_name  = panel_name,           // arg 2
    flags       = 0x11,                 // arg 3 (DefScreen flags — bit0=alt
                                        // background-style, bit4=modal,
                                        // exact meaning TPane-flags
                                        // UNCONFIRMED-B)
    x           = 0x7e = 126,           // arg 4 — pane x in screen
    y           = 0x41 = 65,            // arg 5 — pane y
    w           = 0x18a = 394,          // arg 6
    h           = 0x13c = 316,          // arg 7
    bg_w        = 200,                  // arg 8 — DEF "background" bg sprite w
    bg_h        = 0x50 = 80,            // arg 9 — DEF "background" bg sprite h
    bg_resource = "widgets",            // arg 10 — DAT name for the bg-class
                                        //          (resource cache key)
    outer_name  = "ingamemenu",         // arg 11 — passed as in_stack_0000002c
                                        //          to virt_meth_0x435150
                                        //          (the `Filter` widget?)
)
if (result == nonzero) cls_0x49c830::meth_0x49c830(&DAT_00667548)
    // i.e. pause-all AIL samples on the singleton audio mixer.
return result
```

Note 1 — the function is `__thiscall` with `this == &DAT_0066f748`
(see `cls_0x5a5320_TMapPane.cpp:8010-8012`; the FREE-function form
`FUN_00537110` in `recon/discovered/` is the SAME bytes, the disasm
shows no `this` setup at entry — the caller delivers `ecx`).

Note 2 — `bg_w=200, bg_h=80` is the DEF-engine bg-sprite's nominal size
(the `widgets` background overlay), NOT the modal's chrome size. The
`Background` chrome (394×316) is loaded separately by the DEF parser
when it encounters the panel-body bg.

### §6.2 — `DefScreen_Open` (0x435150 / cls_0x5b93c4::virt_meth_0x435150)

Pseudocode (from `recon/classes/cls_0x5b93c4.cpp:81-143`):

```
DefScreen_Open(this, name_a, panel_name, flags, x, y, w, h,
               bg_w, bg_h, bg_resource, outer_name)
  TPane::Initialize(this)                  // 0x434e40
  this->mbr_0x60 = flags                   // store DEF-screen flags
  if (bg_resource == nullptr) goto LOAD_AND_SHOW
  if (stricmp(bg_resource, "widgets") != 0):
    // Custom bg class — load via DefScreen_OpenBgArchive (0x435990)
    DefScreen_OpenBgArchive(this, bg_resource)
    goto LOAD_AND_SHOW
  // bg_resource == "widgets" — share the globally-loaded widgets pack.
  if ((flags & 1) == 0) use_global = DAT_0065bb10  // notex pack
  else                  use_global = DAT_0066733c  // tex pack
  if (use_global == this->mbr_0x74)
    this->mbr_0x78 = 0  // already same; mark unowned
    goto LOAD_AND_SHOW
  if (this->mbr_0x74 != 0 && this->mbr_0x78 != 0)
    FUN_00482f80(this->mbr_0x74)           // release previous
  this->mbr_0x74 = use_global
  this->mbr_0x78 = 0                       // unowned
LOAD_AND_SHOW:
  if (outer_name != nullptr) FUN_00435b20(outer_name)
  DefScreen_LoadAndShow(this, name_a, panel_name, flags,
                        x, y, w, h, bg_w, bg_h)
```

`flags & 1` here picks **textured vs notex** widgets pack. For
ingamemenu the flag is `0x11`, so `(flags & 1) == 1`, the **tex** pack
(`DAT_0066733c`) is used.

### §6.3 — `DefScreen_LoadAndShow` (0x435040)

(From `recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp` and disasm
0x435040-0x4350f7.)

```
DefScreen_LoadAndShow(this, name_a, panel_name, flags, x, y, w, h, bg_w, bg_h):
  this->mbr_0x18 = x                       // store screen-x
  this->mbr_0x14 = y                       // store screen-y
  this->mbr_0xb0 = w                       // store pane width
  this->mbr_0xb4 = h                       // store pane height
  // SetSize/SetPos — calls vtable+0x20 (Open) which sets up pane buffer
  this->vtable[+0x20](this)                // = ctor-ish open: alloc surface
  bg = this->vtable[+0xa0]("background", 0xffffffff)  // resolve "background"
                                            // resource (frame snapshot)
  FUN_00435660(this, bg)                    // attach as backdrop
  FUN_00435380(this)                        // init child-array bookkeeping
  if (name_a) strncpy(this->mbr_0x158, name_a, 0x1f)  // store screen name
  if (panel_name):
    // build "<panel_name>.def" filename, call DefWidget_LoadFile
    fname = panel_name; strip trailing ".def" if present; append ".def"
    if (!DefWidget_LoadFile(this, fname)) return 0
  this->vtable[+0x2c](this, 1)             // Activate / show
  return 1
```

`DefWidget_LoadFile` (`recon/discovered/FUN_004377c0_*.cpp`) loads
`widgets.def` + `<panel_name>.def`, concatenates into `this->buf` at
`+0xc8/+0xcc`, then `DefWidget_ParseBuffer` (`0x437620`) tokenizes and
the `+0xb4` discriminator routes to `DefWidget_ParsePanel` (`0x437000`)
for the `PANEL ... BEGIN ... END` body.

### §6.4 — `virt_meth_0x537190` — command dispatcher (slot 148)

**Ghidra did NOT extract a body for this slot.** Decoded directly from
the disassembly at `0x00537190..0x005373f3` (extracted via
`objdump -d --start-address=0x537190 --stop-address=0x537420 data/Revenant.exe`).
Verbatim string xrefs from `recon/classes/_data.txt:114302-114369` confirm
each control name.

Signature (from prologue `subl $0x100,%esp; ... retl $0x8`):

```
virt_meth_0x537190(this /*ecx=esi*/, param_1 /*[esp+0x108]=ebx*/,
                   param_2 /*[esp+0x114]=edi*/)
```

`param_1` is a widget pointer (`ebx`, then `leal 0x18(%ebx),%edi` =
`ebx+0x18` which is the widget's NAME field per `TWidget` base layout
from `recon/discovered/renames/agent_ui_wave2_defengine.txt:44-45`).
`param_2` is an event code; constant `0xbb8 = 3000` selects "BUTTON
clicked" (matches `cls_0x5b93c4::meth_0x437a80:757` which uses the
same constant as the BUTTON click ack).

Behavior (verbatim decode):

```
virt_meth_0x537190(this, widget, event):
  meth_0x437a80(this, widget, event)           // base-class hook (panel-tag)
  if (event != 3000) goto IGNORE                // not a button click
  name = widget + 0x18                          // widget's NAME C-string
  if  (stricmp(name, "options") == 0):
      this->mbr_0x5c = 3
      this->vtable[+0x08](this)                 // = Close pane (TPane::Close)
      return 1
  elif (stricmp(name, "load")    == 0):
      if (DAT_0066829c == 0):                   // SP only (MP has no "load")
          this->mbr_0x5c = 1
          this->vtable[+0x08](this)
          return 1
      goto IGNORE
  elif (stricmp(name, "save")    == 0):
      if (DAT_0065d0d0 == 0)            goto IGNORE        // saves disabled?
      if (DAT_0066829c != 0
            && DAT_0067682c == 0):
          // MP host-only guard: not host → show "MPMUSTBEHOST" popup
          popup_yesno("MPMUSTBEHOST", 9)
          return 1
      this->mbr_0x5c = 2
      this->vtable[+0x08](this)
      return 1
  elif (stricmp(name, "saveplr") == 0):         // MP-only: "Save Player"
      if (DAT_0066829c == 0)            goto IGNORE        // SP doesn't have it
      // 1) Reset player state via cls@DAT_00667fcc.meth_0x51e0a0(0,0)
      // 2) Format the localized "MPPLAYERSAVED" template via
      //    cls@DAT_0065d4d0.meth_0x49d800("MPPLAYERSAVED") + sprintf into
      //    a 0x100-byte stack buffer with the player's id at +0x38
      // 3) Open popup with 0x19 buttons-mask
      popup_format("MPPLAYERSAVED" args, 0x19)
      return 1
  elif (stricmp(name, "quit")    == 0):
      // quit module: confirm with "quitgameyn" popup; if user confirms,
      // tear down audio (cls@0x667548.meth_0x49c8f0 = AIL release-all),
      // set mbr_0x5c = 4 and Close.
      if (!popup_yesno("quitgameyn", 3))   goto IGNORE
      cls@0x667548.meth_0x49c8f0()              // release/teardown samples
      this->mbr_0x5c = 4
      this->vtable[+0x08](this)
      return 1
  elif (stricmp(name, "exit")    == 0):
      // exit program: confirm with "exitgameyn" popup; if confirmed,
      // tear down audio, set mbr_0x5c = 5 and Close.
      if (!popup_yesno("exitgameyn", 3))   goto IGNORE
      cls@0x667548.meth_0x49c8f0()
      this->mbr_0x5c = 5
      this->vtable[+0x08](this)
      return 1
  elif (stricmp(name, "resume")  == 0):
      this->mbr_0x5c = 6
      this->vtable[+0x08](this)
      return 1
IGNORE:
  this->mbr_0x5c = 0xffffffff                   // -1 sentinel
  return 1
```

Where:
- `vtable[+0x08]` = TPane's `Close` slot, calls the destructor-equivalent
  unwind that stops the modal pump.
- `popup_yesno(name, mask)` is `FUN_0053c060(name, mask)` (the popup
  helper — see §10).
- `this->mbr_0x5c` is the **return value** read by the modal pump
  `meth_0x48f040` after the pane closes; the pump returns it to the
  caller which switches on 1..6 (see §10).

**Command-code map (canonical):**

| name | TEXT | mbr_0x5c after click | resulting action (per caller §10) |
|---|---|---|---|
| load | "Load Game" | 1 | open `loadgame` DEF (`FUN_00539380`) |
| save | "Save Game" | 2 | open `savegame` DEF (`FUN_005399f0`) |
| options | "Game Options" | 3 | open `options` DEF (`FUN_0053a8b0`) |
| quit | "Quit Module" | 4 | tear down module — return to logo screen |
| exit | "Exit Program" | 5 | `PostQuitMessage(0)` — kill app |
| resume | "Resume Game" | 6 | close menu, resume play |
| (ESC) | — | 6 (via `0x537400`) | close menu, resume |
| no match | — | -1 | discard, stay open |

### §6.5 — `virt_meth_0x537400` — KeyPress (decoded from disasm)

From `0x00537400..0x0053741b`:

```
virt_meth_0x537400(this /*ecx*/, vkey /*[esp+0x4]*/, isDown /*[esp+0x8]*/):
  if (vkey != 0x1b) return                       // 0x1b = VK_ESCAPE
  if (isDown == 0)  return                       // ignore key-up
  this->mbr_0x5c = 6                             // "resume" command
  this->vtable[+0x08](this)                      // Close pane
```

ESC inside the menu = Resume (no popup confirmation). This is the
**KeyPress** slot inherited from TPlayScreen-family, overridden on
cls_0x5b9480 — it shortcuts the otherwise-required mouse click on the
Resume button.

### §6.6 — `virt_meth_0x537110` — same as §6.1

(Duplicate of LoadDef_ingamemenu, this is the slot-0 vtable entry. Same
body as the free-function form.)

### §6.7 — `virt_meth_0x537170` — Close

```
virt_meth_0x537170(this):
  cls_0x49c830::meth_0x49c890(&DAT_00667548)     // RESUME-all AIL samples
  cls_0x5b98b8::virt_meth_0x434f30(this)         // TPane::Close (base unwind)
```

(`recon/classes/cls_0x5b9480.cpp:51-57`.) Pairs with the §6.1 Open path's
`meth_0x49c830` sample-pause.

## §7 — Effects & shadows

No panel-private shadow registry. Inherits the DEF widget engine's
defaults:

- **BUTTON text shadow**: per `widgets.def:91` `FLAGS TEXT_CENTER |
  TEXT_VCENTER | TEXT_SINGLELINE | TEXT_SHADOW` for the up-state label.
  `TEXT_SHADOW = 0x400` (`widgets.def:24`). Per
  [UI_METHOD_MAP §5](UI_METHOD_MAP.md#5) the `0x400` font-id bit drives
  the 3-pass black drop shadow at `(0, 0)` + `(+1, 0)` + `(0, +1)`,
  then the colored pass at `(0, 0)`. Color = white `(255, 255, 255)`
  for up state, black `(0, 0, 0)` for down state.
- **No chroma key** for the modal chrome: `Background` and `Picture`
  have `kc=0x0` (per dump_dat.py) and are drawn opaque (`DM_USEDEFAULT
  = 0x80000000`). The widgets sprites (`ClearFrame*` / `FillFrame*`) use
  `kc=0x7c1f` (magenta in RGB555) which the port treats globally as
  transparent (see `UI_METHOD_MAP §16`, `[[project-retail-pink-halo-bug]]`).
- **Pink-halo bug**: text rendered through `FUN_004be2b0` on a
  magenta-cleared scratch can show the pink fringe. Documented retail
  bug — DO NOT REPRODUCE (`[[project-retail-pink-halo-bug]]`,
  `TPlyrStatusBar_SPEC.md:295`). Use real alpha.

## §8 — Text rendering (table)

All BUTTON labels share the same cell geometry — they are CENTER /
VCENTER inside the button's `(141, 27)` rect.

| string | cell (pane-local x,y,w,h) | font | px | color | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| "Load Game" | (212, 63, 141, 27) | "Med" | (TTF Arimo-14 in port; bitmap font in retail) | (255,255,255) up / (0,0,0) down | center | vcenter | 3-pass `0x400` | literal `TEXT "Load Game"` | DEF | `ingamemenu.def:11`, `widgets.def:91-92` |
| "Save Game" | (212, 100, 141, 27) | "Med" | as above | as above | center | vcenter | yes | literal | DEF | `ingamemenu.def:12` |
| "Game Options" | (212, 137, 141, 27) | "Med" | as above | as above | center | vcenter | yes | literal | DEF | `ingamemenu.def:13` |
| "Quit Module" | (212, 174, 141, 27) | "Med" | as above | as above | center | vcenter | yes | literal | DEF | `ingamemenu.def:14` |
| "Exit Program" | (212, 211, 141, 27) | "Med" | as above | as above | center | vcenter | yes | literal | DEF | `ingamemenu.def:15` |
| "Resume Game" | (212, 248, 141, 27) | "Med" | as above | as above | center | vcenter | yes | literal | DEF | `ingamemenu.def:16` |

- **`TEXT_CENTER | TEXT_VCENTER`** (widgets.def:91) — DrawTextA flag
  bits map to `param_10 & 2` (h-center) and `param_10 & 0x20` (v-center)
  per `UI_METHOD_MAP §5:148-153`.
- **`TEXT_SINGLELINE`** = `0x80` (widgets.def:21) — single-line override
  forces the format word to `0x20` (top), but `0x40` (vcenter) wins
  because it's explicitly set first.
- **`TEXT_SHADOW`** = `0x400` (widgets.def:24) — sets the shadow gate
  on `param_8` (font id) at `FUN_004be2b0:377`.
- **Color source**: widgets.def `COLOR 255 255 255` (up) /
  `COLOR 0 0 0` (down). Packed via `FUN_00429950_BlitEffect_PackRGB(buf,
  B, G, R)` → stored R,G,B,0 (`UI_METHOD_MAP §6`). The colored-pass
  uses `param_7` byte-swapped to BGR for GDI `SetTextColor`.
- **Vertical alignment**: TEXT_VCENTER is genuinely set
  (`widgets.def:91`), so the in-cell label is vertically centered
  (not top — exception to the NOMENCLATURE §2 default).

No localization swap; the TEXT literals are baked into ingamemenu.def.
The "Med" font is the retail bitmap font; the port substitutes Arimo-14
via the canonical `BuildTTFAtlas` path (`UI_METHOD_MAP §12`).

There are no `STRINGREF` indirections in the ingamemenu screen (none of
the BUTTONs use `STRINGREF`, all use `TEXT "..."`).

## §9 — Animation & dynamic behavior

- **No transitions on open**: the modal appears in one frame. No fade,
  no slide.
- **No hover-fade ramp on the buttons**: BUTTON widgets in this engine
  flip between Up and Down sprites on mouse-down/up state; there is no
  intermediate cur-counter ramp like TSideTabsPane. (Confirmed by
  `widgets.def:89-93` having only UP / DOWN sprites and no DRAWMODE
  hint of a glow.)
- **Audio**: AIL samples pause on Open (§6.1) and resume on Close
  (§6.7). This is the only audible animation.
- **Picture FIELD swap**: BITMAP "picture" has `FIELD "picture"`
  (ingamemenu.def:7). The DEF engine binds field-named widgets to a
  caller-supplied sprite at activation time. **UNCONFIRMED-A**: the
  sprite actually swapped in by the retail SP code path. Default is the
  packaged `Picture` (172×210).

Dirty-redraw model: standard TPane — `cls_0x5b93c4`'s `mbr_0xbc`/`mbr_0xc0`
are an open/close `0..N` step counter (`recon/classes/cls_0x5b93c4.cpp:269-283`)
for any deferred fade-in (only active if `mbr_0x60 & 0x10` flags bit is
set; for ingamemenu `flags=0x11` so bit4 IS set — UNCONFIRMED-C: whether
this drives a visible fade-in transition or just a 1-tick activation
debounce. The bit is referenced from `:68-71, 269-283, 405-409` in
cls_0x5b93c4).

No state machine beyond Open → Pump → Close.

## §10 — Input & dispatch

The screen is interactive. Standard DEF-engine input flow:

1. **Mouse / keyboard input** arrives at the pane via the screen
   manager's input dispatch and hits TPane's per-child hit-test
   (`cls_0x5b93c4::virt_meth_0x4364d0` at `:548-566`). Each BUTTON
   widget has hit rect = its `POS x y w h` from the DEF (table
   below).
2. **BUTTON click** → BUTTON widget's mouse handler emits a "control
   event" to the panel via `cls_0x5b93c4::meth_0x437a80` (slot 148
   default — the panel-tag hook) and then via the panel's own slot 148
   (which in cls_0x5b9480 is overridden as `virt_meth_0x537190`, §6.4),
   with `event=3000` (0xbb8).
3. **Command dispatch** (§6.4): names "load" / "save" / "options" /
   "quit" / "exit" / "resume" → set `this->mbr_0x5c` and close.
4. **ESC key** (`virt_meth_0x537400`, §6.5): same as clicking "Resume".
5. **Modal pump** (`meth_0x48f040`) returns the `mbr_0x5c` value to
   the caller `cls_0x5a5320_TPlayScreen::meth_0x47e500`
   (`recon/classes/cls_0x5a5320_TMapPane.cpp:7999-8045`):

   ```c
   void TPlayScreen::meth_0x47e500() {
     if (DAT_0066f788 == 0) {
       do {
         cls_0x5b9480::virt_meth_0x537110(&DAT_0066f748);   // Open
         cmd = meth_0x48f040(this, &DAT_0066f748);          // Pump
         cls_0x5b9480::virt_meth_0x537170(&DAT_0066f748);   // Close
         switch (cmd) {
           case 1: open loadgame DEF (cls_0x5b9584::virt_meth_0x539380)
                   then pump → cls_0x5b9584::virt_meth_0x539440 (close);
                   break;
           case 2: open savegame DEF (cls_0x5b963c::virt_meth_0x5399f0)
                   then pump → cls_0x5b963c::virt_meth_0x539ab0 (close);
                   break;
           case 3: open options DEF (cls_0x5b9744::virt_meth_0x53a8b0)
                   then pump → cls_0x5b9744::virt_meth_0x53aa60 (close);
                   UNK_0065cb3c._4_4_ = 1;
                   goto LAB_0047e517;          // re-open ingamemenu
           case 4: set teardown-state, exit method (back to logo);
                   return;
           case 5: PostQuitMessage(0);
                   // fall through (default is the loop exit)
           default: break;                      // exits loop (resume game)
         }
         UNK_0065cb3c._4_4_ = 1;
       } while (cmd != 5 && /* default */ false);
     }
   }
   ```

   So case 3 (options) re-loops back to ingamemenu after the options
   screen closes (logical UX — close options → back to pause menu).
   Cases 1/2 close ingamemenu, open the sub-screen, and DO NOT
   re-open ingamemenu — control returns to the caller of meth_0x47e500
   (which is the TimerTick ESC path; ESC press → menu → choice → next
   screen takes over).

**Hit rects** (pane-local — matches §4 BUTTON rows; add `(126, 65)` for
screen-space):

| control | command id | hit rect (pane-local) | cite |
|---|---|---|---|
| BUTTON "load" | 1 | (212, 63, 141, 27) | `ingamemenu.def:11`, §6.4 |
| BUTTON "save" | 2 | (212, 100, 141, 27) | `ingamemenu.def:12`, §6.4 |
| BUTTON "options" | 3 | (212, 137, 141, 27) | `ingamemenu.def:13`, §6.4 |
| BUTTON "quit" | 4 | (212, 174, 141, 27) | `ingamemenu.def:14`, §6.4 |
| BUTTON "exit" | 5 | (212, 211, 141, 27) | `ingamemenu.def:15`, §6.4 |
| BUTTON "resume" | 6 | (212, 248, 141, 27) | `ingamemenu.def:16`, §6.4 |
| ESC key | 6 (= "resume") | n/a | `virt_meth_0x537400`, §6.5 |

**Modal-state globals consulted by the dispatcher**:

| global | meaning | effect on dispatch |
|---|---|---|
| `DAT_0066829c` | MP mode flag (0=SP, nonzero=MP) | "load" only fires in SP; "save" semantics flip; "saveplr" only in MP; out-of-scope for this spec |
| `DAT_0067682c` | MP-is-host flag | "save" path requires host in MP; otherwise show MPMUSTBEHOST popup |
| `DAT_0065d0d0` | save-allowed flag (script-controlled?) | when 0, "save" is IGNORED silently — UNCONFIRMED-D: which game state clears it |

**Trigger** (how `meth_0x47e500` is reached): the user task brief
describes this as the `s_exitgameyn` ESC path inside TPlayScreen's
TimerTick (`virt_meth_0x47c630_TPlayScreen_TimerTick`,
`recon/discovered/renames/agent_screens.txt:186-191`). Cross-verified
from raw disasm:

- `0x0047ca66` pushes `s_exitgameyn_005d7680` and calls
  `FUN_0053c060` (popup yes/no), `0x0047ca6b`.
- After the popup branch returns, the code path falls through to
  `0x0047cb74` which calls `meth_0x47e500` (the ingamemenu loop).
- The intermediate region `0x0047caa1..0x0047cb74` is the F12
  screenshot-capture branch (anchor `s_ss.bmp_005d768c`).

The exact branch conditions feeding `meth_0x47e500` from TimerTick (i.e.
which keystroke / state precisely triggers it) is in TimerTick's body,
which is a large method not yet decompiled into `recon/discovered/`.
**UNCONFIRMED-E**: a tight statement of the ESC vs F-key conditions
that select the menu vs the exitgameyn popup branch. The bound on the
spec is that **both paths exist and converge on meth_0x47e500 for
opening this modal**, and the modal's behaviour itself is fully pinned
here.

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo on BUTTON text** — see [[project-retail-pink-halo-bug]]
   and `UI_METHOD_MAP §16`. The retail magenta-cleared scratch +
   antialiased shadow produces a pink fringe on every shadowed label.
   The intent is white-text/black-shadow on the up state. Reproduce
   intent — use real alpha (no magenta colorkey) when compositing text.
2. **Off-center modal**: the literal pane position `(126, 65)` is
   `(+3, −17)` from true centre on a 640×480 framebuffer. This is NOT
   a bug — it's the retail layout (the panel artwork is asymmetric:
   the portrait inset is left-biased, the button column is right-biased,
   so the overall composition is visually balanced even though the pane
   rect itself is off-centre by these literal amounts). Reproduce
   literally; do not "fix" by centering programmatically.
3. **None known specific to ingamemenu**. Audio pause/resume is
   correct behaviour, not a bug.

## §12 — Reconstruction pseudocode

Engine-level rather than panel-level (this is a DEF-driven screen, the
implementor wires the dispatch glue and lets the DEF engine paint).

```cpp
// At program startup (singleton ctor — TIngamemenuScreen mirrors cls_0x5b9480):
//   member init from recon/classes/cls_0x5b9480.cpp:61-82
//     screen_w = 640;  screen_h = 480;
//     buf_w    = 640;  buf_h    = 480;     // 0x1c=0x280, 0x20=0x1e0
//     child_array = TArray<TWidget*>(0x10);  // cap=16

void TIngamemenuScreen::Open() {
    if (g_MPMode != 0) {
        // OUT OF SCOPE — mpingamemenu variant
        DefScreen::Open("ingamemenu", "mpingamemenu",
                        /*flags*/ 0x11, /*x*/126, /*y*/65,
                        /*w*/394, /*h*/316,
                        /*bg_w*/200, /*bg_h*/80,
                        /*bg_resource*/"widgets",
                        /*outer*/"ingamemenu");
    } else {
        DefScreen::Open("ingamemenu", "ingamemenu",
                        0x11, 126, 65, 394, 316, 200, 80,
                        "widgets", "ingamemenu");
    }
    g_AudioMixer.PauseAllSamples();        // cls_0x49c830::meth_0x49c830
}

void TIngamemenuScreen::Close() {
    g_AudioMixer.ResumeAllSamples();       // cls_0x49c830::meth_0x49c890
    TPane::Close();                        // cls_0x5b98b8::virt_meth_0x434f30
}

// Slot 148 override — the BUTTON-clicked dispatcher.
int TIngamemenuScreen::OnControl(TWidget* w, int event) {
    TButtonPane::OnControl(w, event);      // base hook (meth_0x437a80)
    if (event != 3000) return 1;           // not a BUTTON click
    const char* name = w->name;            // widget+0x18

    if      (icmp(name, "options")) { last_cmd = 3; Close(); }
    else if (icmp(name, "load"))    { if (g_MPMode == 0) {
                                          last_cmd = 1; Close(); } }
    else if (icmp(name, "save"))    {
        if (!g_SaveAllowed) return 1;
        if (g_MPMode != 0 && !g_IsHost) {
            ShowPopup_YesNo("MPMUSTBEHOST", 9);  // OK button only
            return 1;
        }
        last_cmd = 2; Close();
    }
    else if (icmp(name, "saveplr")) {                  // MP only
        if (g_MPMode != 0) {
            g_Player.ResetState(0, 0);
            char buf[0x100];
            const char* tpl = g_StringRefs.Lookup("MPPLAYERSAVED");
            snprintf(buf, sizeof buf, tpl, g_Player.id);
            ShowPopup_Format(buf, 0x19);  // OK
        }
    }
    else if (icmp(name, "quit")) {
        if (!ShowPopup_YesNo("quitgameyn", 3)) return 1;
        g_AudioMixer.TeardownSamples();
        last_cmd = 4; Close();
    }
    else if (icmp(name, "exit")) {
        if (!ShowPopup_YesNo("exitgameyn", 3)) return 1;
        g_AudioMixer.TeardownSamples();
        last_cmd = 5; Close();
    }
    else if (icmp(name, "resume")) { last_cmd = 6; Close(); }
    else { last_cmd = -1; }                            // discard
    return 1;
}

bool TIngamemenuScreen::OnKey(int vkey, bool isDown) {
    if (vkey == VK_ESCAPE && isDown) {
        last_cmd = 6;                                  // Resume
        Close();
        return true;
    }
    return false;
}
```

**At the caller** (TPlayScreen ESC handler), reproduce the
open/pump/close + switch from `recon/classes/cls_0x5a5320_TMapPane.cpp:8001-8045`:

```cpp
void TPlayScreen::OpenInGameMenu() {
    while (true) {
        g_InGameMenu.Open();
        int cmd = ModalPump(&g_InGameMenu);
        g_InGameMenu.Close();

        switch (cmd) {
        case 1:                              // load
            g_LoadGameScreen.Open();
            ModalPump(&g_LoadGameScreen);
            g_LoadGameScreen.Close();
            return;                          // close menu, hand off to load
        case 2:                              // save
            g_SaveGameScreen.Open();
            ModalPump(&g_SaveGameScreen);
            g_SaveGameScreen.Close();
            return;
        case 3:                              // options — re-open menu after
            g_OptionsScreen.Open();
            ModalPump(&g_OptionsScreen);
            g_OptionsScreen.Close();
            continue;                        // re-enter menu loop
        case 4:                              // quit module
            g_TPlayScreen.module_state = QUITTING;
            g_TPlayScreen.TeardownModule();
            return;
        case 5:                              // exit program
            PostQuitMessage(0);
            return;
        case 6: default:                     // resume
            return;
        }
    }
}
```

Per-widget paint is the responsibility of the DEF widget renderer
(`DefWidgetEngine.md`); this spec inventories the values that renderer
must consume (§4 §8) but does not re-spec BUTTON / FRAME paint here —
that lives in the engine spec, which when written will cover all DEF
screens uniformly.

## §13 — Port mapping notes

| retail call (in §5/§6) | port primitive | home |
|---|---|---|
| `(*+0x5c)(buf, sprite, 0, 0)` opaque blit | `Renderer->DrawBitmapSubrectToTarget(bm, ...)` w/ `DM_USEDEFAULT` semantics | renderer |
| `FUN_00438d80_BlitEffect_StructInit` for opaque dest rect | `MakeDP(dx, dy, dw, dh, sx, sy, sw, sh, drawmode)` (struct fill) | inline / shared |
| BUTTON Up/Down sprites (9-slice) | `Renderer->DrawBitmapNineSlice(bm, dst_rect, slice={10,10,12,12}, margins={0,0,4,4})` | renderer — see [[project-ui-layout-system]] for "proper 9-slice" requirement |
| BUTTON label text | `font.cpp DrawTextShadowedToTarget(atlas, "Load Game", cell_x, cell_y, cell_w, cell_h, kAlignCenterHCenterV, 1.0, 1.0, 1.0, /*shadow*/ true)` | font.cpp |
| BITMAP "picture" | `Renderer->DrawBitmapToTarget(picture_or_field_swap, x, y)` | renderer |
| FRAME widget (9-slice with ClearFrame1) | same `DrawBitmapNineSlice` primitive | renderer |
| Modal pump | application-level event loop (no engine primitive; the port can wrap in a "modal screen" helper) | screen manager |
| Pause-all / Resume-all audio | `AudioMixer::Pause()` / `Resume()` (new primitive needed if not already wired — UNCONFIRMED whether the port has these) | audio |
| Popup yes/no | needs DEF engine + `popup.def` reconstruction (separate spec) | DEF engine |

**Missing primitives** (→ "needs new primitive"):
- 9-slice draw with explicit slice + margins. (Confirmed missing per
  [[project-ui-layout-system]] — the layout system spec explicitly notes
  "proper 9-slice" as outstanding.)
- DEF widget renderer (whole engine — see `DefWidgetEngine.md:106-115`).
- Audio mixer `PauseAllSamples` / `ResumeAllSamples` (UNCONFIRMED — may
  already exist in `src/audio*` from the gameflow track).

## §14 — UNCONFIRMED / open questions

- **UNCONFIRMED-A — Picture FIELD-swap content**: BITMAP "picture" has
  `FIELD "picture"` (ingamemenu.def:7). The DEF engine binds
  field-named BITMAPs to caller-supplied sprite slots; the actual
  sprite swapped in by the SP open path is not visible from
  `FUN_00537110` (no setup before the call). **Impact if wrong**: the
  user sees the static placeholder `Picture` (172×210) instead of a
  context-appropriate image (e.g. character portrait, save-slot
  thumbnail). **Resolve**: extract `FUN_00435b20` (the
  `outer_name="ingamemenu"` handler called from `DefScreen_Open:55-57`)
  via `DecompileAddr.java 0x00435b20`, OR `CallersOf` on the BITMAP
  widget's `FIELD` setter to see who writes the binding. Visual-verify
  by booting into an SP save and pressing ESC — what's in the cell.
- **UNCONFIRMED-B — `flags=0x11` exact bit semantics**: bit 0 (tex pack
  vs notex) is decoded; bit 4 (`0x10`) is referenced in
  `cls_0x5b93c4.cpp:68-71` and gates the open/close ramp in `:269-283`,
  but its precise visual effect (fade-in vs 1-tick debounce) is
  uncertain. **Impact**: maybe a 1-frame pop vs a brief fade-in.
  **Resolve**: trace `mbr_0xbc`/`mbr_0xc0` consumers — these are
  consulted somewhere in the draw path (likely an alpha multiplier
  computed from `bc/c0`).
- **UNCONFIRMED-C — Background slot resolution**: the
  `FUN_00435040:31` call `(*+0xa0)("background", 0xffffffff)` resolves
  by name on a vtable+0xa0 slot. In context that's probably a
  resource-cache lookup that returns the `Background` chrome bitmap
  from `ingamemenutex.dat`. Sole UNCONFIRMED: whether `"background"` is
  resolved as a literal asset name in the .dat (the entry IS named
  "Background" — capital B) or as a special token meaning "the
  per-screen bg sprite registered by DefScreen_Open's
  `bg_resource="widgets"` arg". **Impact**: wrong bg sprite picked.
  **Resolve**: `DecompileAddr.java 0x00435b20` (the
  `outer_name="ingamemenu"` handler) and trace what it does with the
  "ingamemenu" string — likely it preloads the
  `ingamemenu{tex,notex}.dat` archive so the by-name resolver finds
  the `Background` entry there.
- **UNCONFIRMED-D — `DAT_0065d0d0` save-gate**: when 0, the "save"
  button is silently ignored (`0x537240`-ish branch). **Impact**:
  reproducing exact retail behaviour during in-cutscene / boss-fight
  states where save is blocked. **Resolve**: `FindWriters` on
  `DAT_0065d0d0` in Ghidra — likely a script API hook from
  combat/cutscene code.
- **UNCONFIRMED-E — TimerTick precise condition for opening the
  menu**: TimerTick body (`0x47c630`) is large and not yet in
  `recon/discovered/`; the disasm confirms the path
  `exitgameyn-popup → ss.bmp-save → meth_0x47e500` exists, but the
  exact predicate that selects "open menu" vs other branches is in
  TimerTick's switch logic, partly inside the `0x47caa1..0x47cb74`
  block. **Impact**: knowing which user keystroke triggers the menu
  vs just the popup. **Resolve**: `DecompileAddr.java 0x0047c630` and
  cross-reference with `agent_screens.txt:186-191`.
- **UNCONFIRMED-F — TPlyrStatusBar-style font ascent jank**: BUTTON
  vcenter relies on `font.cpp DrawText*ToTarget` placing the label
  correctly; the shared font helper has a one-time GDI-DT_TOP
  correction (`NOMENCLATURE.md:103`) — verify the centred BUTTON
  label sits visually centered, not 2px high. **Impact**: ~2px
  off-vertical label, common across all DEF screens. **Resolve**:
  visual-verify in a `--test=ui-ingamemenu` harness when the engine
  lands.
- **UNCONFIRMED-G — `bg_w=200, bg_h=80` semantics**: these constants
  are stashed somewhere by DefScreen_LoadAndShow (the disasm shows
  stores into `this->mbr_0xb0` and `this->mbr_0xb4` as w/h, but the
  bg_w/bg_h pair is two ADDITIONAL ints not visible in the
  Ghidra-decoded signature). They may be the bg-sprite's cell width
  for a tile/pattern background, or a clip rect for the bg pass.
  **Impact**: wrong bg tiling/clipping under the chrome (likely
  invisible — chrome is opaque and covers everything). **Resolve**:
  `DecompileAddr.java 0x00435660` (where the bg is attached after
  the +0xa0 lookup) and check whether bg_w/bg_h are consumed.

## End

Forensics complete for the SP variant. The MP variant
(`mpingamemenu`) is explicitly out of scope per task brief and gated
by `DAT_0066829c != 0` at `FUN_00537110:9-12`. All numeric facts in
§3/§4/§6/§8/§10 cite either the DEF file (which is shipping retail
data), the bytes of `data/Revenant.exe` decoded via objdump (with the
exact address range), or the Ghidra recon files (with explicit
file:line). No screenshot-derived coordinates.
