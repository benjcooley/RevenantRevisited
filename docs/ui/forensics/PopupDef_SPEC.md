# PopupDef — forensics spec (popup.def, all four panels)

Generic modal popup screen. One `.def` file (`popup.def`) defines **four**
PANEL bodies — `yesno`, `okcancel`, `ok`, `progress` — selected at open time
by two activator entry-points (`FUN_0053bf00` for yes/no/ok, `FUN_0053c1d0`
for progress). All four panels share the same on-screen chrome (the
`Background` bitmap in `popuptex.dat`), the same screen rect, and the same
DEF widget engine. The four panels differ only in their child widgets (1 or
2 buttons, or a progress BITMAP/FRAME pair).

## §0 — Sources & status

- **Class**: NO dedicated class. The popup is a free-function pair of
  activators (`FUN_0053bf00`, `FUN_0053c1d0`) that operate on the **generic
  DEF-screen base** (the same base that hosts `ingamemenu.def`,
  `savegame.def`, `loadgame.def`, …; vtable `cls_0x5b93c4`, in turn a
  subclass of `cls_0x5a4494_TPane` — see `InGameMenuDef_SPEC.md §0`).
  - The yes/no/ok activator operates on a `this` passed in `ecx`
    (the caller's TDefScreen instance, e.g. a member of the screen that
    opened the popup — see §10).
  - The progress activator (`FUN_0053c1d0`) is **standalone** (allocates
    its own TDefScreen instance pair in a pair of global slots
    `DAT_0066ff10` and `DAT_00670214` — see §10), so it has no
    instance-class either; it constructs and destructs a generic
    DefScreen on the fly.
- **No class id** — this is *engine-id*, not class-id. (See AGENT_PROTOCOL
  Rule 2-sibling: bare/static activators routed through the shared engine.)
  `FUN_0053bf00` is the screen's "LoadDef_popup" entry, `FUN_0053c1d0` the
  "LoadDef_popup_progress" entry (per
  `recon/discovered/renames/agent_ui_wave2_defengine.txt:305-310`).
- **Paint method**: no panel-private paint. All four panels render via the
  shared DEF-engine renderer over the assets named in `popup.def` and
  `widgets.def` (see `DefWidgetEngine.md`, §"Renderer", which is itself
  outstanding port work — `recon/discovered/port_status/DefWidgetEngine.md:106-115`).
- **DEF asset (literal copy below)**:
  `data/resources_unzipped/popup.def`
  (absolute:
  `/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/popup.def`).
- **Chrome bitmap asset**:
  - `popuptex.dat` (Classic, hi-res): one bitmap `Background` 398×212,
    BM_15BIT chunked/compressed (flags=0x10400), kc=0 — measured
    `tools/ui/dump_dat.py /…/popuptex.dat`.
  - `popupnotex.dat` (15-bit, alt): one bitmap `Background` 398×212,
    flags=0x402, kc=0.
  - `popupalpha.dat` (with alpha track): one bitmap `Background` 398×212,
    flags=0x502, kc=0. (Same archive-name selection rule as other multi-DAT
    chrome bitmaps — see `TPlyrStatusBar_SPEC.md` for the parallel.)
- **Recon files read** (paths relative to repo root):
  - `recon/discovered/renames/agent_ui_wave2_defengine.txt:1-310` — engine
    architecture, activator string anchors, `popup.def`
    activator-vs-screen-name confirmation (lines 149, 305-310).
  - `recon/discovered/port_status/DefWidgetEngine.md:62-77` — engine roster,
    popup row (FUN_0053bf00 + FUN_0053c1d0 both ref "popup").
  - `docs/ui/briefs/B_r7_def_widget_engine.md:43-122,142-160` — engine call
    chain (DefScreen_Open → DefScreen_LoadAndShow → DefWidget_LoadFile),
    activator call-site map, pane field layout.
  - `docs/ui/forensics/InGameMenuDef_SPEC.md` — the worked-example
    precedent for a DEF-driven modal pane (the popup is the simpler sibling).
  - Raw disassembly extracted from `data/Revenant.exe` (.text base 0x401000;
    `objdump -d -M intel --start-address=…`):
    - `0x0053bf00..0x0053bf8b` — `FUN_0053bf00 LoadDef_popup` (yes/no/ok
      selector + DefScreen_Open call).
    - `0x0053bf90` — single-instruction thunk: `jmp 0x434f30` (the
      engine's "clear children" method; also called inline by
      `FUN_0053c060` after pump-exit). Identified separately from the
      activator at 0x53bf00, but in the same module.
    - `0x0053bfa0..0x0053c05d` — `FUN_0053bfa0` Command-dispatch for the
      yes/no/ok variants.
    - `0x0053c060..0x0053c0cd` — `FUN_0053c060` cursor-save wrapper that
      opens a popup synchronously on the global instance.
    - `0x0053c100..0x0053c13f` — `FUN_0053c100` progress-popup setup
      (variant-1 in-place / wrapper around DefScreen_Open with progress
      panel + flags=0x11).
    - `0x0053c150..0x0053c189` — `FUN_0053c150` Command-dispatch for
      progress.
    - `0x0053c190..0x0053c1c3` — `FUN_0053c190` lookup-by-name helper
      ("progress" → returns the progress bitmap widget pointer).
    - `0x0053c1d0..0x0053c35e` — `FUN_0053c1d0 LoadDef_popup_progress`
      (variant-2): allocates a fresh global DefScreen, registers it, and
      opens "popup"/"progress".
    - `0x0053c360..0x0053c3ce` — `FUN_0053c360` Close-popup-progress
      (frees the global instance + clears tracking globals).
    - `0x0053c3d0..0x0053c4bc` — `FUN_0053c3d0` Update-progress (compute
      progress-bar width from value, blit, sync, optionally call frame-step).
    - `0x00435040..0x00435150` — `FUN_00435040 DefScreen_LoadAndShow`
      (param decoder).
    - `0x00435150..0x00435225` — `FUN_00435150 DefScreen_Open` (high-level
      "open modal DEF screen"; param-order decode below).
    - `0x00437a80` — `FUN_00437a80` (Command/save-name post-handler; the
      `cmd == 0xbb8` branch).
    - `0x00436980` — `FindChildByName(this, name)` (children-list strcmp
      scan).
    - `0x00436de0` — `meth_0x436de0` (focus / dirty-set helper; vtable
      slot used by progress-set-message path).
- **Status**: `forensics-complete` for the popup-engine contract +
  the runtime dispatcher (§13a). The four panels' layout, geometry,
  chrome, button command-ids, variant-selection, modal-pump and
  close-paths are all pinned at the literal-arg level. The full
  retail call-site census + message-key catalog + flag-bit decoding
  is now in §13a, and confirms the absence of `autosaveYN` and
  save-overwrite popups in retail (treated as modernization items).
  **Per-pixel paint of each child widget** (button, bitmap, frame,
  text) is deferred to the DEF widget renderer, which is shared
  engine work and not yet ported (`DefWidgetEngine.md:106-115`).
- **Errors to close** in existing ports: none — no engine code has been
  written yet for popup.def specifically. Greenfield. Once the
  DefWidgetEngine renderer lands, this spec is its driver for the popup
  family.
- **Snapshot note (supplementary — superseded by retail)**: no pre-release
  `src/popup*.cpp` exists in the worktree (verified by grep). The
  snapshot's analogue is the inline "MessageBox()" calls in
  `src/playscreen.cpp` ESC + save paths, which the retail engine replaced
  wholesale with the DEF-driven popup. Snapshot is not a source for
  layout or behavior here.

## §1 — Overview

`popup.def` is the **generic modal-dialog vocabulary** of Revenant. The same
398×212 chrome bitmap with the same screen rect hosts four logical roles:

- **`yesno`** — Yes/No confirmation (e.g. "Quit to main menu?").
  Two BUTTON children: "Yes" at (84,139) and "No" at (201,139), both 93×27.
- **`okcancel`** — destructive-action confirmation (e.g. "Overwrite save?").
  Two BUTTON children: "Ok" at (84,139) and "Cancel" at (201,139).
- **`ok`** — single-button acknowledgement (e.g. "Save failed.").
  One BUTTON: "Ok" at (141,139), 93×27.
- **`progress`** — progress modal during long ops (e.g. autosave, file IO,
  module load). Replaces the buttons with a BITMAP `progress` (driven by
  the `progress` data field, see §9) wrapped in a FRAME chrome.

All four share a TEXT child named "message" — the dialog body string — and
the same chrome backdrop (`Background` 398×212).

**Visibility / show predicate.** The popup is shown via one of:
1. `FUN_0053bf00(this, flags, [opt msg-key])` — called directly with a
   `flags` byte that picks the panel: `bl & 1` → "yesno", `bl & 4` →
   "okcancel", else "ok". The `bl & 0x10` bit chooses whether the message
   string is a literal pointer or a lookup-table key (`DAT_0065d4d0`, the
   game string table) — see §6.
2. `FUN_0053c060(message_key)` — convenience wrapper. Saves cursor, calls
   `FUN_0053bf00` on a global TDefScreen instance (`DAT_00670090`), runs
   the modal pump (`FUN_0048f040`, mask 7), restores cursor. This is what
   callers like exit-confirm use.
3. `FUN_0053c1d0(message_key)` — fresh allocation path for the **progress**
   modal: `new TDefScreen` in two global slots (`DAT_0066ff10`,
   `DAT_00670214`), register, open "popup"/"progress". The pump is run
   externally; close via `FUN_0053c360`.

**Hide predicate.** The yes/no/ok flavor closes itself from
`FUN_0053bfa0` (the Command handler) by clearing `this+0x5c` and calling
`vtable+0x8` (close). The progress flavor is closed externally by
`FUN_0053c360`.

## §2 — Asset roster

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| `Background` | `popuptex.dat` (Classic) | "Background" | 398×212 | chrome backdrop for **all 4 panels** | full bitmap | `tools/ui/dump_dat.py popuptex.dat → 00_Background.png (398×212 flags=0x10400 kc=0)` |
| `Background` | `popupnotex.dat` (15-bit alt) | "Background" | 398×212 | same role, 15-bit format | full bitmap | dump_dat → flags=0x402 kc=0 |
| `Background` | `popupalpha.dat` (alpha track) | "Background" | 398×212 | same role, w/ alpha | full bitmap | dump_dat → flags=0x502 kc=0 |
| `ClearFrame3` | (`widgets.dat`; not measured here) | per `popup.def:6` | NN×NN UNCONFIRMED | TEXT widget chrome (`STYLE TEXT BGBITMAP "ClearFrame3"`) | 9-slice via FRAME 5,5,5,5 | `popup.def:6-8` |
| `ClearFrame2` | (`widgets.dat`) | per `popup.def:11` | NN×NN UNCONFIRMED | BUTTON UP chrome (`STYLE BUTTON NORMAL UP "ClearFrame2"`); also FRAME chrome around progress (`popup.def:59`) | 9-slice via FRAME 10,10,12,12 | `popup.def:11,59` |
| `FillFrame2` | (`widgets.dat`) | per `popup.def:11` | NN×NN UNCONFIRMED | BUTTON DOWN chrome (`STYLE BUTTON NORMAL ... DOWN "FillFrame2"`) | 9-slice via FRAME 10,10,12,12 | `popup.def:11` |
| `progress` (BITMAP entry name) | per-screen data | "progress" | 280×24 (the BITMAP's POS w,h) | the live progress fill — `FIELD "progress"`, driven by `DAT_0066ff14`/`DAT_0066ff18`/`DAT_0066ff08+0x60..0x64` | full bitmap (no atlas) | `popup.def:60`, `FUN_0053c3d0:0053c43e..0053c4bc` |

> Note: `ClearFrame3`, `ClearFrame2`, `FillFrame2` are entries inside the
> shared `widgets.dat` archive — that archive isn't measured in this spec.
> Each is a 9-slice frame whose corner/edge thicknesses are given by the
> `FRAME L T R B` numbers in the STYLE definition. The widget renderer
> draws them via the engine's generic FRAME / BITMAP / BUTTON paths
> (`DefWidgetEngine.md` Widget class roster) — NOT panel-private code.

> The four popup-archive variants (`popuptex.dat`, `popupnotex.dat`,
> `popupalpha.dat`) follow the same Classic-vs-hi-res selection rule as
> other multi-format chrome (see `TPlyrStatusBar` §11). The port should
> resolve at runtime which one the active resolution / format expects.

## §3 — Coordinate frames & surfaces

The popup is a flat, single-pane DEF screen. There are NO chip/mirror
gymnastics. Frame chain is short:

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer TL (root) | n/a | (0, 0) | display surface (640×480 stock, 0x280×0x1e0) | DEF activator constants are baked for 640×480 |
| **popup-pane** | screen | TL (literal x,y from activator) | screen `(0x81, 0x7a)` = (129, 122) | the popup pane rect; size `(0x18e, 0xd4)` = (398, 212). `Background` chrome fills the pane. | `FUN_0053bf00:0053bf72..bf74` (push 0x7a, 0x81) and `FUN_0053bf00:0053bf6d..bf68` (push 0x18e, 0xd4); same literals at `FUN_0053c1d0:0053c121..c11a` |
| **panel-local** | popup-pane | TL (= same origin as pane) | (0, 0) in pane | the coord space of `popup.def`'s `POS x y w h` values for child widgets | `popup.def:21,24,25,33,36,37,45,48,56,59,60` |
| **widget-local** | panel-local | TL | each widget's `POS x y` | the coord space of each widget's child content (label text inside a BUTTON, the "message" TEXT body, the `progress` BITMAP fill) | per-widget; e.g. message TEXT POS 32 60 → widget-local origin at panel-local (32,60) |
| **source-rect** | source bitmap TL | n/a | per-asset | the (sx,sy,sw,sh) sub-rect copied OUT of `Background` / `ClearFrame*` / `progress` | per-asset |

**Composition formula.** A widget element at widget-local `(wx, wy)`
maps to screen as:

```
screen_x = pane_origin_x + widget_pos_x + wx
         = 0x81 + widget_pos_x + wx
         = 129  + widget_pos_x + wx

screen_y = pane_origin_y + widget_pos_y + wy
         = 0x7a + widget_pos_y + wy
         = 122  + widget_pos_y + wy
```

The popup pane is **TL-anchored at literal (129, 122)** in screen — NOT
centered by formula. The retail values approximately center a 398×212
pane in a 640×480 display: ideal-center x = (640-398)/2 = 121, retail
chose 129; ideal-center y = (480-212)/2 = 134, retail chose 122. So the
popup sits 8 px right of and 12 px above ideal center. Treat (129, 122)
as the literal anchor — do NOT recompute from display size in the port,
those literals are what the retail activator passes to DefScreen_Open.

**Surfaces.** No scratch surfaces in the popup-specific code: each widget
draws directly to the screen via the DEF widget engine's renderer. The
direct-renderer contract still applies (compose into the shared HUD RT,
then blit) — but the popup itself does not allocate its own buffer.

The progress flavor (`FUN_0053c1d0`) DOES allocate one extra surface
(`DAT_00670214`) at construction (`FUN_0053c1d0:0053c1f9..c217` — the
`FUN_004a1ec0(0x118, 0x18, …)` allocation + `[eax+0x14]=0x100` /
`[eax+0x18]=DAT_006668d0` stores). This is a side-buffer for the
progress widget — see §9 for its role.

### Layout diagram (panel-local, screen anchor at (129, 122))

```
panel-local 0,0                                            panel-local 398,0
├──────────────────────────────────────────────────────────┤   ← top edge
│                                                          │
│            popup chrome (Background, full 398×212)       │
│                                                          │
│  YESNO / OKCANCEL / OK PANELS:                           │
│                                                          │
│  POS 32,60  ┌──────────────────────────────────┐  306×72│
│             │  TEXT "message" (style ClearFrame3│        │
│             │  9-slice, FONT "Med" TEXT_CENTER, │        │
│             │  TEXT_SHADOW; NO TEXT_VCENTER)    │        │
│             │  Body string — looked up via      │        │
│             │  DAT_0065d4d0 string table OR     │        │
│             │  passed as literal (see §6).       │        │
│             └──────────────────────────────────┘        │
│                                                          │
│  Row of buttons at y=139:                                │
│    yesno:    BUTTON 84,139 "yes"  93×27 ─┐  BUTTON 201,139 "no"     93×27│
│    okcancel: BUTTON 84,139 "ok"   93×27 ─┤  BUTTON 201,139 "cancel" 93×27│
│    ok:       BUTTON 141,139 "ok"  93×27   (single, centered)              │
│                                                          │
│                                                          │
│  PROGRESS PANEL:                                         │
│  POS 26,68 ┌────────────────────────────────────┐ 324×44│
│            │  TEXT "message" (same style)        │       │
│            │  (Body string)                       │       │
│            └────────────────────────────────────┘        │
│                                                          │
│  POS 42,119 ┌──────────────────────────────────┐ 296×38│
│             │ FRAME BITMAP "ClearFrame2"        │       │
│             │ ┌─────────────────────────────┐   │       │
│             │ │ POS 49,126 BITMAP "progress"│   │       │
│             │ │ 280×24 — clipped to value/max│   │      │
│             │ └─────────────────────────────┘   │       │
│             └──────────────────────────────────┘        │
│                                                          │
└──────────────────────────────────────────────────────────┘
panel-local 0,212                                          panel-local 398,212
```

All coordinates above are **panel-local**. To convert to screen, add
the pane origin `(0x81, 0x7a) = (129, 122)`.

## §4 — Static element layout (table)

For each child widget. Per the §3 rule, x,y,w,h here are **panel-local**;
add `(129, 122)` to compose to screen. There is no mirrored side.

### `yesno` PANEL (`popup.def:17-27`)

| element | space | (x,y) panel-local | (w,h) | source rect | cite |
|---|---|---|---|---|---|
| chrome `Background` | panel-local | (0,0) | (398,212) | full 398×212 | `FUN_0053bf00:0053bf68..bf74` literals + `popuptex.dat` measurement |
| TEXT "message" | panel-local | (32,60) | (306,72) | TEXT widget; BGBITMAP `ClearFrame3` 9-slice | `popup.def:21` |
| BUTTON "yes" (TEXT "Yes") | panel-local | (84,139) | (93,27) | BUTTON UP `ClearFrame2` / DOWN `FillFrame2` 9-slice | `popup.def:24` |
| BUTTON "no" (TEXT "No") | panel-local | (201,139) | (93,27) | same | `popup.def:25` |

### `okcancel` PANEL (`popup.def:29-39`)

| element | space | (x,y) | (w,h) | source rect | cite |
|---|---|---|---|---|---|
| chrome `Background` | panel-local | (0,0) | (398,212) | full | as above |
| TEXT "message" | panel-local | (32,60) | (306,72) | same TEXT style | `popup.def:33` |
| BUTTON "ok" (TEXT "Ok") | panel-local | (84,139) | (93,27) | same BUTTON style | `popup.def:36` |
| BUTTON "cancel" (TEXT "Cancel") | panel-local | (201,139) | (93,27) | same | `popup.def:37` |

### `ok` PANEL (`popup.def:41-50`)

| element | space | (x,y) | (w,h) | source rect | cite |
|---|---|---|---|---|---|
| chrome `Background` | panel-local | (0,0) | (398,212) | full | as above |
| TEXT "message" | panel-local | (32,60) | (306,72) | same | `popup.def:45` |
| BUTTON "ok" (TEXT "Ok") | panel-local | (141,139) | (93,27) | same | `popup.def:48` |

### `progress` PANEL (`popup.def:52-62`)

| element | space | (x,y) | (w,h) | source rect | cite |
|---|---|---|---|---|---|
| chrome `Background` | panel-local | (0,0) | (398,212) | full | as above |
| TEXT "message" | panel-local | (26,68) | (324,44) | same TEXT style; **NOTE** narrower-and-shorter cell than yes/no/ok | `popup.def:56` |
| FRAME (BITMAP="ClearFrame2") | panel-local | (42,119) | (296,38) | 9-slice chrome around the progress bar | `popup.def:59` |
| BITMAP "progress" (FIELD "progress") | panel-local | (49,126) | (280,24) | full asset; **width clipped at draw time** to `value/max * 280` — see §6/§9 | `popup.def:60` |

> Centering checks (literal arithmetic, not eyeballed):
> - In `yesno`/`okcancel`: the two buttons cover panel-local 84..177 and
>   201..294. Row midpoint = (84+294)/2 = 189 ; panel midpoint = 199.
>   Row sits 10 px **left** of panel center.
> - In `ok`: single button at 141..234, center = 187.5 ; panel mid 199.
>   Button sits 11.5 px left of panel center.
> - The "message" TEXT (32..338, w=306) center = 185 ; panel mid 199.
>   Text cell sits 14 px left of panel center.
>
> Combined with the pane itself sitting 8 px **right** of screen-center
> ((640-398)/2 = 121 vs. retail's 129), the **net** position of children
> on screen is roughly: child-screen-center = pane_left + panel_mid −
> ~12 = 129 + 199 − 12 ≈ 316 — about 4 px left of screen center 320.
> So things look approximately centered on screen even though both
> the pane offset and the child offset individually look off. This is
> the retail layout — do not adjust either offset; they cancel.

## §5 — Draw order / composition

There is NO panel-private paint code. Draw order is the DEF widget engine's
generic per-pane walk:

1. **Background** — the popup pane's chrome `Background` 398×212 is drawn
   first, opaque, at panel-local (0,0) (= screen (129,122)). This is the
   `vtable+0x20(w,h)` resize-or-redraw call wired by DefScreen_Open via
   the `bg_sprite_name` arg ("widgets" — see §6) plus the `outer_name`
   ("popup") arg. The DEF engine's `DrawBackground` (see InGameMenuDef
   `DefScreen_Open` chain) blits this chrome.
2. **Children** — every child widget in the active PANEL is then walked
   in `this+0x98[]` order (declaration order in the .def file), each one
   painting itself via its widget vtable+slot — TEXT, BUTTON, FRAME,
   BITMAP. The walker is `cls_0x5b93c4::FUN_00436090` (see
   `InGameMenuDef_SPEC.md §5` — the popup is the same engine).
3. **Dirty/focus overlay** — the focused widget paints its highlight
   above its base via `vtable+0x1c` (the DEF widget "dirty" / focus
   pass). For BUTTON, that's the UP vs DOWN sprite swap when pressed.

The popup does **not** override any draw slots — `FUN_0053bf00` is a pure
"setup → DefScreen_Open" wrapper, not a paint method. The progress flavor
adds an ad-hoc post-step (`FUN_0053c3d0` repaints the `progress` BITMAP
when its value changes — §6/§9).

## §6 — Algorithms (pseudocode per helper)

### 6.1 `FUN_0053bf00 LoadDef_popup(this, flags, msg_arg)` — yesno/okcancel/ok

Signature (from disassembly `0x53bf00..0x53bf8b`):

```
thiscall FUN_0053bf00(TDefScreen* this /*ecx*/,
                      uint32_t flags    /*[esp+4] = arg1*/,
                      void*    msg_arg  /*[esp+8] = arg2*/);
```

Body (decoded literal-by-literal):

```c
void LoadDef_popup(TDefScreen* this, uint32_t flags, void* msg_arg)
{
    char* msg;
    if (flags & 0x10) {                 // 0x53bf08: test bl, 0x10
        msg = (char*) msg_arg;          // 0x53bf0d: literal C-string pointer
    } else {                            // 0x53bf13:
        msg = FUN_0049d800(             // LookupStringByKey(string_table, key)
            DAT_0065d4d0 /*ecx=0x65d4d0; the global string table*/,
            (char*) msg_arg);
    }
    this->msg_ptr  = msg;               // [esi+0x180] = eax            (0x53bf24)
    this->flags    = flags;             // [esi+0x17c] = ebx            (0x53bf2d)
    // chrome_flag = (flags & 1) ? 0 : 0x11
    //   recon: dl = bl & 1; neg dl; sbb edx,edx; and edx,0x11
    //   if flags&1 → edx=0; else → edx=0x11
    uint32_t chrome_flags = (flags & 1) ? 0 : 0x11;   // 0x53bf2a..bf37

    // Select panel name by flag bits:
    char* panel;
    if      (flags & 2) panel = "yesno";     // 0x5e4650  (0x53bf3a..bf41)
    else if (flags & 4) panel = "okcancel";  // 0x5e4658  (0x53bf48..bf4b)
    else                panel = "ok";        // 0x5e4664  (0x53bf52..bf57)

    DefScreen_Open(this,
        /*panel_name */ "popup",      // 0x5e4678  (outer "popup" basename)
        /*screen_name*/ panel,
        /*flags     */  chrome_flags, // 0 or 0x11
        /*x          */  0x81,        // 129
        /*y          */  0x7a,        // 122
        /*w          */  0x18e,       // 398
        /*h          */  0xd4,        // 212
        /*bg_w       */  0x134,       // 308   ← UNCONFIRMED meaning (see §14)
        /*bg_h       */  0x64,        // 100   ← UNCONFIRMED meaning
        /*bg_sprite  */ "widgets",    // 0x5e4670 (the widget archive name)
        /*outer_name */ "popup"       // 0x5e4668
    );
    return;
}
```

Arg-to-DefScreen_Open mapping uses the signature established in
`B_r7_def_widget_engine.md:49-50`:
`DefScreen_Open(this, panel_name, screen_name, flags, x, y, w, h, bg_w, bg_h, bg_sprite_name, outer_name)`.

> Why `chrome_flags` is `0` or `0x11`. The two bits in `0x11` are
> `CTRLFLAG_ERROR (0x1) | CTRLFLAG_SELECTABLE (0x10)` per `widgets.def:30-34`.
> The `0x10` (SELECTABLE) bit makes sense: the popup pane wants to receive
> arrow-key focus. The `0x1` (ERROR) bit is odd here; more likely this is
> the engine's "fresh load" flag rather than the widgets.def ERROR
> definition — UNCONFIRMED.

### 6.2 `FUN_0053bfa0` — Command dispatcher for yesno/okcancel/ok

Signature (from `0x53bfa0..0x53c05d`):

```
thiscall FUN_0053bfa0(TDefScreen* this /*ecx*/,
                      void*       ctrl /*[esp+4] = arg1 — widget that
                                          emitted command*/,
                      uint32_t    cmd  /*[esp+8] = arg2 — command id/code*/);
```

Behavior:

```c
int FUN_0053bfa0(TDefScreen* this, void* ctrl, uint32_t cmd)
{
    FUN_00437a80(this, ctrl, cmd);       // base post-handler (e.g. save-name)
                                         // 0x53bfaf

    if (cmd == 1) {                      // 0x53bfb4
        // "message" body was clicked / "message" widget triggered
        // — re-set the popup's body string into the message TEXT widget.
        TWidget* msg_widget = FindChildByName(this, "message" /*0x5e4680*/);
        // 0x53bfb9..bfc0
        msg_widget->vtable[+0x18](msg_widget, this->msg_ptr);
        // 0x53bfc5..bfd0 — call SetText (vtable+0x18) on the TEXT widget
        return 1;                        // cmd
    }

    if (cmd != 0xbb8 /* 3000 */)         // 0x53bfdb
        return 1;                        // ignore other commands

    // cmd == 3000: a BUTTON pressed. Identify which by widget name
    // (NAME field at ctrl+0x18) and set this->result accordingly.
    const char* name = (const char*)((char*)ctrl + 0x18);  // 0x53bfe3

    if (strcmp(name, "ok"  /*DAT_005e4688*/)     == 0 ||   // 0x53bfe7
        strcmp(name, "yes" /*DAT_005e468c*/)     == 0 ||   // 0x53bff9
        strcmp(name, "cancel" /*s_cancel_005e4690*/) == 0 ||// 0x53c00b
        strcmp(name, "no" /*DAT_005e4698*/)      == 0) {   // 0x53c01d
        // it's one of our buttons
    } else {
        return 1;                        // unknown button → ignore
    }

    // OK/YES path: this->result = 1, close pane
    //   0x53bff6: je 0x53c047 (if "ok" match)
    //   0x53c008: je 0x53c047 (if "yes" match)
    //   0x53c04b: mov [esi+0x5c], 0x1
    if (name in {"ok", "yes"}) {
        this->result = 1;                // [esi+0x5c] = 1   (0x53c04b)
        this->vtable[+0x8](this);        // close            (0x53c052)
        return 1;
    }
    // NO/CANCEL path: this->result = 0, close pane
    //   0x53c01a: je 0x53c02e (if "cancel" match)
    //   0x53c02c: jne 0x53c055 (if "no" mismatch → return); else fall through
    //   0x53c032: mov [esi+0x5c], 0x0
    if (name in {"cancel", "no"}) {
        this->result = 0;                // [esi+0x5c] = 0   (0x53c032)
        this->vtable[+0x8](this);        // close            (0x53c039)
        return 1;
    }
}
```

> **Decoded result convention.** `this+0x5c` is the **return value** the
> popup reports to its caller after close: `1 = positive (ok/yes)`,
> `0 = negative (cancel/no)`. For the `ok`-only panel the result is
> always 1. The caller reads this slot after the modal pump exits.
>
> **NOTE.** The result polarity is **inverted** from the obvious C-style
> convention of "0=ok, 1=cancel" — retail uses **1 = positive button
> pressed**. Likely because the engine's pump-exit condition checks
> "result != 0" (i.e. a non-zero result means "a positive button was
> pressed; close with success"). Verify before naming the return
> constants in the port (UNCONFIRMED §14 item C).

> **Pairing**: the "Yes" and "Ok" buttons map to **result 1**; "No" and
> "Cancel" map to **result 0**. Both name pairs are intentional —
> retail dispatches by NAME, not by panel, so a Yes/No popup and an
> OK/Cancel popup both produce 1=positive/0=negative.

### 6.3 `FUN_0053c060` — synchronous popup wrapper (no progress)

```c
int FUN_0053c060(uint32_t msg_arg)
{
    if (DAT_00667fd0 == 0)            // 0x53c060..c067 — disabled if no UI host
        return 0;

    FUN_0043a080();                   // cursor: SAVE cur cursor    (0x53c06e)
    int saved = eax;                  // ← save the previous cursor handle

    // Set the "wait/arrow" popup cursor (cursor resource "cursor"):
    void* curh = FUN_0046d710(DAT_0065abc0, "cursor" /*0x5e469c*/);
    FUN_0043a020(curh);               //              (0x53c086)

    // Open the popup on global instance DAT_00670090:
    //   FUN_0053bf00(DAT_00670090, flags=arg-from-caller, msg_arg)
    FUN_0053bf00(DAT_00670090, [esp+0x10], [esp+0x14]);  // 0x53c098

    // Register + pump until result is set:
    FUN_0048f040(DAT_00670090, 7);    //              (0x53c0af) — modal pump

    FUN_00434f30(DAT_00670090);       // clear/destroy popup children (0x53c0bb)
                                      // — iterates DAT_00670090[+0x98][]
                                      // and calls each child's vtable[0](1)
                                      // (the engine's "DEF screen tear-down"
                                      // method, also tail-jumped from
                                      // FUN_0053bf90)

    FUN_0043a020(saved);              // cursor: restore previous   (0x53c0c0)
    return eax;                       // FUN_0048f040's return — see §14C
}
```

> The return of `FUN_0053c060` is `eax` from `FUN_0048f040` (the modal
> pump). Whether that's the popup's `+0x5c` result code (forwarded
> through the pump) is UNCONFIRMED — see §14C.

> The mask `7` to `FUN_0048f040` is a flag triple (visible + selectable +
> active, by analogy with the `0xffffffff` mask used by `FUN_0048ed90` for
> unconditional registration in `cls_0x5a5320_TPlayScreen::Pulse`).
> UNCONFIRMED in detail — see §14.

### 6.4 `FUN_0053c100` — progress activator (variant-1, in-place)

```c
thiscall FUN_0053c100(TDefScreen* this /*ecx*/, uint32_t msg_arg /*arg1*/)
{
    this->msg_ptr = msg_arg;           // [ecx+0x17c] = eax       (0x53c132)
    DefScreen_Open(this,
        "popup",                       // 0x5e46c0
        "progress",                    // 0x5e46b4
        0x11,                          // flags = CTRLFLAG_ERROR|SELECTABLE
        0x81, 0x7a, 0x18e, 0xd4,       // x,y,w,h — SAME as yesno/okcancel/ok
        0x190 /*=400*/, 0x64 /*=100*/, // bg_w=400, bg_h=100 (DIFFERENT from
                                       // yes/no's 308×100 — UNCONFIRMED §14)
        "widgets",                     // 0x5e46ac
        "popup"                        // 0x5e46a4
    );
    return;
}
```

### 6.5 `FUN_0053c150` — Command dispatcher for progress

```c
thiscall FUN_0053c150(TDefScreen* this, void* ctrl, uint32_t cmd)
{
    FUN_00437a80(this, ctrl, cmd);     // base                       (0x53c15e)
    if (cmd == 1) {                    // 0x53c163
        TWidget* msg = FindChildByName(this, "message" /*0x5e46c8*/);
        msg->vtable[+0x18](msg, this->msg_ptr);  // SetText           (0x53c17f)
    }
    return 1;
}
```

### 6.6 `FUN_0053c190` — "find progress widget by name"

```c
thiscall FUN_0053c190(TDefScreen* this, char* arg0, void* arg1)
{
    if (strcmp(arg0, "progress" /*0x5e46d0*/) == 0)        // 0x53c19e
        return DAT_00670214;        // the cached progress side-surface
                                    // (0x53c1aa)
    return FUN_00436de0(this, arg0, arg1);  // delegate to base dirty/focus
                                            // 0x53c1bc
}
```

### 6.7 `FUN_0053c1d0 LoadDef_popup_progress(this, msg_arg)` — variant-2 (standalone)

This is the path used when the popup must outlive its caller (e.g. a
streaming/save operation running through several frames). It allocates a
fresh global side-surface and registers it explicitly.

```
thiscall FUN_0053c1d0(? this /*ecx*/, uint32_t msg_arg /*[esp+4]=arg1*/);
```

Body (decoded `0x53c1d0..0x53c35e`):

```c
void LoadDef_popup_progress(void* maybe_unused, uint32_t msg_arg)
{
    // 1) Allocate the side-surface descriptor:
    void* dp = FUN_004a1ec0(           // alloc 0x118 bytes, flags=0x18, 0,0
        0x118, 0x18, 0, /*src=*/(*((int*)DAT_005d79e0+0x38)) & 0x3001f);
    DAT_00670214        = dp;          // 0x53c212 — slot for progress side-surf
    *((uint32_t*)dp+5)  = 0x100;       // [dp+0x14] = DM_TRANSPARENT  (0x53c209)
    *((uint32_t*)dp+6)  = DAT_006668d0;// [dp+0x18] = display-context (0x53c217)

    // 2) Allocate the modal TDefScreen instance:
    void* pane = FUN_00482fb0(0x6c);   // alloc 108 bytes (the TPane subclass)
                                       // 0x53c21a
    if (pane != null) {
        FUN_004bcb00(pane);            // TPane ctor / surface init  (0x53c23d)
        *(uint32_t*)pane = 0x5a3ee4;   // set vtable for this pane    (0x53c24a)
        FUN_004a3a40(pane, dp);        // bind side-surf              (0x53c250)
    }

    // 3) Track the new pane in the global slot:
    DAT_0066ff08 = pane;               // 0x53c26c
    void* host   = DAT_006668d0;       // 0x53c25c

    // Call pane->vtable[+0x1c] to "bind to host"? (per InGameMenu pattern)
    pane->vtable[+0x1c](pane, host);   // 0x53c272

    // Fill the host display with a transparent box (clear behind progress):
    host->vtable[+0x64](                // = TSurface::Box (UI_METHOD_MAP §15a)
        0, 0,
        host->w, host->h,
        0xffff, /*zpos*/ 0x7f7f, /*normal*/ 0x80000000 /*=DM_USEDEFAULT*/);
                                       // 0x53c285..c29e

    // (4) FUN_0049d800: lookup msg_arg in DAT_0065d4d0 string table — same
    // routine as yes/no's variant. Result in eax = the message text ptr.
    char* msg = FUN_0049d800(DAT_0065d4d0, msg_arg_local);   // 0x53c2ab

    // (5) Open the DEF screen on DAT_0066ff10 — a SECOND global pane slot:
    DefScreen_Open(DAT_0066ff10,        // ecx
        "popup",                        // 0x5e46c0
        "progress",                     // 0x5e46b4
        0x11,                           // flags
        0x81, 0x7a, 0x18e, 0xd4,        // x=129, y=122, w=398, h=212
        0x190 /*=400*/, 0x64 /*=100*/,  // bg_w=400, bg_h=100
        "widgets",                      // 0x5e46ac
        "popup"                         // 0x5e46a4
    );                                  // 0x53c2e8

    DAT_0067008c = msg;                 // remember message ptr     (0x53c2e3)

    // (6) Register the new pane with the screen manager:
    FUN_0048ed90(DAT_0066ff10, 0xffffffff);   // ~analogous to push  (0x53c2fa)
    FUN_0048ed60(DAT_0066ff10, 7);            // setup flags=7        (0x53c30e)
    FUN_0048eea0(DAT_0066ff10);               // finalize             (0x53c316)

    // (7) Pump UI events until idle — the screen manager drains the queue
    //     by calling FUN_004911b0(1) (likely SwapBuffers / RunFrame) until
    //     DAT_0066ffcc == DAT_0066ffd0 (queue-head == queue-tail). The
    //     two DATs are an event-queue head/tail pair.

    while (DAT_0066ffcc != DAT_0066ffd0)       // 0x53c326
        FUN_004911b0(1);                       // 0x53c32d

    FUN_004911b0(1);                            // final pump        (0x53c346)
}
```

The progress variant is **fire-and-forget on the calling thread**: it
opens the popup, runs the pump until the event queue drains (so the
popup is fully painted on screen), then returns. The popup persists
on screen until `FUN_0053c360` (Close) is called.

### 6.8 `FUN_0053c360 Close_popup_progress`

```c
void Close_popup_progress()
{
    FUN_00435010(DAT_0066ff10);           // close the DEF screen    (0x53c365)

    // Drain remaining UI events:
    while (DAT_0066ffcc != DAT_0066ffd0)  // 0x53c375
        FUN_004911b0(1);                  // 0x53c37b
    FUN_004911b0(1);                      // final pump              (0x53c394)

    // Call the global pane's destructor / cleanup:
    void* pane = DAT_0066ff08;            // 0x53c399
    if (pane) pane->vtable[+0](pane);     // 0x53c3aa (dtor call)
    DAT_0066ff08 = 0;                     // 0x53c3b1

    // Free the side-surface descriptor:
    FUN_004830f0(DAT_00670214);           // 0x53c3bc
    DAT_00670214 = 0;                     // 0x53c3c4
}
```

### 6.9 `FUN_0053c3d0 Update_popup_progress(value, max)`

This is the "set progress %" call. It computes the bar width, blits a
clear-color box behind the bar, then re-blits the `progress` BITMAP
clipped to the new width.

```c
void Update_popup_progress(uint32_t value, uint32_t max)
{
    // Pack a color (0xaa,0,0) into a 4-byte struct, then call (0x53c3d6..c3e4):
    uint32_t packed_color;
    FUN_00429950(&packed_color, 0, 0, 0xaa);   // pack RGB B,G,R → struct
    // (UI_METHOD_MAP §6 — stored order R=B-arg=0xaa? No — call order is
    //  (dst, B, G, R) and indices store R,G,B. With (dst, 0, 0, 0xaa)
    //  → dst[0]=R=0xaa, dst[1]=G=0, dst[2]=B=0 → final color 0xAA0000 = dark
    //  red. (UI_METHOD_MAP §6 gotcha.))

    int eax = FUN_004384a0(&packed_color);     // ?? convert to display fmt
                                                // 0x53c3eb
    eax &= 0xffff;                              // 16-bit color word

    // value / max gating: edx = arg2 / arg1, then width = ((((edx<<3)-edx) * 8) >> 8?)
    // The exact divider math computes: bar_dy_offset = (max * 3 / 50) or similar.
    // [INCOMPLETE — the arithmetic block 0x53c412..c434 normalizes value/max
    // and stores the displayed-bar-width in `edx`. See UNCONFIRMED §14 item E.]

    // Blit a "cleared" colored box behind the bar:
    void* this_panel = DAT_0066ff08;
    this_panel->vtable[+0x64](           // = TSurface::Box / fill
        0, 0,
        /*width=*/ edx,                  // computed bar width
        /*height=*/ 0x18 = 24,           // the BITMAP height
        eax,                             // packed display color
        0xffff,                          // zpos
        0x7f7f,                          // normal
        0x80000000                       // drawmode = DM_USEDEFAULT
    );                                   // 0x53c43b

    // Re-find the "progress" widget and re-blit its bitmap, offset by
    // the same (DAT_0066ff14, DAT_0066ff18) origin:
    void* bitmap_w = FUN_00436900(DAT_0066ff10, "progress" /*0x5e46e8*/);
                                         // 0x53c453
    if (bitmap_w == 0) goto skip;        // 0x53c458
    {
        int dx = bitmap_w->vtable[+0x60] + DAT_0066ff14;   // 0x53c47b..c483
        int dy = bitmap_w->vtable[+0x64] + DAT_0066ff18;   // 0x53c46f..c479
        FUN_004bd680(dy, dx,             // sprite-stamp helper (§4 UI_METHOD_MAP)
            (uint32_t)bitmap_w_resource, // ?
            0x100, 0);                   // drawmode = DM_TRANSPARENT
                                         // 0x53c48b
        DAT_005d79e0->vtable[+0xa0](DAT_005d79e0, 1);   // 0x53c498 — flush
    }
skip:
    void* msg = FindChildByName(DAT_0066ff10, "progress" /*0x5e46e8*/);  // 0x53c4a7
    int flags = *(int*)((char*)msg + 0x14) | 0x20;                       // 0x53c4ac..c4b1
    msg->vtable[+0x1c](msg, flags);      // set dirty bit on progress     (0x53c4b7)
}
```

> **UNCONFIRMED — Update math.** The bar-width arithmetic
> `0x53c412..0x53c434` ("`lea eax, [8*edx]; sub eax, edx; lea edx, [eax +
> 4*eax]; shl edx,3; imul 0x10624dd3; sar edx,6`") is a classic
> compiler-emitted `n*100/N` style fast division — `0x10624dd3` is the
> magic constant for divide-by-100. The exact formula (likely
> `bar_w = value * 280 / max`) is UNCONFIRMED — see §14 item E.

## §7 — Effects & shadows

- **Drop shadow on TEXT** — both popup body text and BUTTON labels carry
  `TEXT_SHADOW = 0x400` per `widgets.def:24` and `popup.def:8,13`. The
  shared `FUN_004be2b0` renderer detects this via `param_8 & 0x400` and
  does the 3-pass black shadow (`UI_METHOD_MAP §5` step 8). Direction is
  baked: +1 in x, +1 in y (down-right). No flipping; popup has no mirrored
  side.
- **Chroma key** — `popuptex.dat` chrome has `keycolor = 0`. The chrome
  bitmap is intended to fully fill its 398×212 rect (no transparent
  corners), so chroma-key handling is moot for the chrome itself. The
  `ClearFrame*` 9-slice frames around TEXT/BUTTON use the magenta key
  per the global rule (UI_METHOD_MAP §16) — these are the same shared
  widget assets used by every DEF screen.
- **Sprite shadow (FUN_00438d80 path)** — popup widgets do NOT register
  per-widget shadow effects with the §8 blit-effect pipeline. The pane
  has no calls into `FUN_00438d80` / `FUN_004aa850` etc.
- **No glow / no halo.** The pink-halo retail bug only affects text the
  shared text renderer draws over a magenta-keyed scratch surface. The
  reconstruction agent must render text with real alpha (UI_METHOD_MAP
  port-mapping line) and NOT reproduce the halo.

## §8 — Text rendering (table)

All four panels carry a single TEXT widget named "message". The yes/no/ok
PANELs share the same cell (32,60,306,72); the progress PANEL uses a
narrower (26,68,324,44). The cell rule is the same: 9-slice-framed by
`ClearFrame3`, FONT "Med", color (255,255,255), h-align=CENTER, v-align=TOP
(NO TEXT_VCENTER override), shadow=ON.

| string | cell (panel-local x,y,w,h) | font | px | color | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| message body (yesno/okcancel/ok) | (32,60,306,72) | "Med" (TFontTable lookup) | TBD UNCONFIRMED | (255,255,255) | center (`TEXT_CENTER=0x2`) | **top** (NO TEXT_VCENTER) | YES (3-pass, `TEXT_SHADOW=0x400`) | none (raw string) | `this->msg_ptr` (= `string_table[msg_key]` or literal pointer per `flags&0x10`) | `popup.def:6-8,21,33,45` |
| message body (progress) | (26,68,324,44) | "Med" | TBD | (255,255,255) | center | top | YES | none | `this->msg_ptr` (set via `FUN_0053c100`) | `popup.def:6-8,56` |
| BUTTON "Yes" label | inside button rect (84,139,93,27); text positioned by UPLABELRECT(0,0,0,0) → cell == button rect | "Med" | TBD | UP (255,255,255) / DOWN (0,0,0) | center (`TEXT_CENTER`) | vcenter (`TEXT_VCENTER` IS set on buttons) | YES (UP only; DOWN has no shadow) | none | literal "Yes" (from `popup.def:24` TEXT field) | `widgets.def:88-93`, `popup.def:10-15,24` |
| BUTTON "No" label | (201,139,93,27) | same | TBD | same | center | vcenter | same | none | literal "No" | `popup.def:25` |
| BUTTON "Ok" label (okcancel-row) | (84,139,93,27) | same | same | same | center | vcenter | same | none | literal "Ok" | `popup.def:36` |
| BUTTON "Cancel" label | (201,139,93,27) | same | same | same | center | vcenter | same | none | literal "Cancel" | `popup.def:37` |
| BUTTON "Ok" label (ok panel) | (141,139,93,27) | same | same | same | center | vcenter | same | none | literal "Ok" | `popup.def:48` |

> **Notable v-align difference.** popup.def's TEXT style overrides
> widgets.def with **no VCENTER** — the popup body text hangs from the
> cell TOP (NOMENCLATURE §2 baseline rule). Buttons keep VCENTER from
> widgets.def. Do not vertically-center the body text in the cell — it
> will sit at the top.

> **No format strings.** The popup body is a single string buffered into
> `this->msg_ptr` by the activator; the renderer draws it raw. The
> caller supplies either a literal C string (`flags & 0x10`) or a key
> looked up in `DAT_0065d4d0` (the global string table — the same one
> the savegame screen uses).

## §9 — Animation & dynamic behavior

The yes/no/ok panels have NO animation. State is stack-frame–simple:
modal opens, user clicks a button, dispatcher writes the result, modal
closes. No fades, no ramps, no per-frame redraw.

The **progress** panel has one dynamic element — the BITMAP `progress`
that grows from 0→280 px wide as the operation completes. State the
ramp:

```
ramp progress.bar_width:
  range   0..280 px
  step    none (recomputed on each Update call, not interpolated)
  formula bar_w = value * 280 / max     (UNCONFIRMED exact divisor — §14E)
  drive   external — caller updates value/max and re-issues
          Update_popup_progress(value, max) (= FUN_0053c3d0).
  paints  re-fills the cleared box at (49,126,bar_w,24) with packed color
          (R=0xAA,G=0,B=0) = dark-red; then re-blits the `progress` BITMAP
          atop, clipped to bar_w. Sets dirty bit on the "progress"
          widget so the next pane walk reflects the new state.
  on-end  no implicit end — caller closes via FUN_0053c360 when done.
  cite    FUN_0053c3d0:0053c3d6..0053c4b7
```

**Dirty model.** The pane is redrawn each frame the screen manager
pumps (`FUN_004911b0(1)` per progress activator path). Individual widgets
maintain their own dirty flag bits at `+0x14`; the Update_progress call
sets `+0x14 |= 0x20` (CTRLFLAG_DIRTY per `widgets.def:35`) on the
progress widget to force its next redraw (`0x53c4ac..c4b7`).

## §10 — Input & dispatch

### 10.1 The two activator-vs-instance paths

Two distinct lifetimes are at play:

1. **Embedded** (`FUN_0053bf00 LoadDef_popup` + `FUN_0053c060`): a pane
   member of the caller's screen object. The yes/no/ok variant uses
   `FUN_0053c060` to (a) save cursor, (b) open the popup on the global
   instance `DAT_00670090`, (c) pump until result, (d) restore cursor.
   The result is read from the global instance's `+0x5c` slot by the
   caller, who then dispatches Yes vs No / Ok vs Cancel.
2. **Standalone fresh** (`FUN_0053c1d0 LoadDef_popup_progress` +
   `FUN_0053c360`): allocates two global slots
   (`DAT_0066ff10` = pane, `DAT_00670214` = side-surface descriptor)
   on every open, then explicitly closes / frees on `FUN_0053c360`.
   No modal pump loop; the pump drains the existing queue ONCE so
   the popup is fully visible, then returns to the caller's main loop
   so the caller can keep doing background work and call
   `Update_popup_progress(value, max)` periodically.

### 10.2 Hit rects (panel-local)

| control | space | x,y,w,h | cmd id | result | cite |
|---|---|---|---|---|---|
| BUTTON "yes" / "ok" (left-of-pair) | panel-local | (84,139,93,27) | 0xbb8 = 3000 | `this+0x5c = 1` (positive) | `popup.def:24,36`; dispatch `FUN_0053bfa0:0053bff6,0053c008,0053c04b` |
| BUTTON "no" / "cancel" (right-of-pair) | panel-local | (201,139,93,27) | 0xbb8 = 3000 | `this+0x5c = 0` (negative) | `popup.def:25,37`; dispatch `FUN_0053bfa0:0053c01a,0053c02c,0053c032` |
| BUTTON "ok" (single, centered) | panel-local | (141,139,93,27) | 0xbb8 = 3000 | `this+0x5c = 1` | `popup.def:48` |
| TEXT "message" (body) | panel-local | (32,60,306,72) yes/no/ok or (26,68,324,44) progress | NOT hit-tested | n/a | `popup.def:21,33,45,56` |

**All BUTTONs emit the same command id `0xbb8 = 3000`** — the dispatcher
(`FUN_0053bfa0`) discriminates by NAME, not command id. The dispatcher
is registered as the pane's Command-handler vtable slot — likely
`vtable+0xb8` per `DefWidgetEngine.md` (UNCONFIRMED, §14 item B).

### 10.3 The modal pump

For yes/no/ok: `FUN_0048f040(DAT_00670090, 7)` runs the pump. It pumps
the screen-manager's event queue until the active screen reports
"done" — i.e., until the close path (`vtable+0x8`) in the dispatcher
above sets the pane inactive. After pump returns, caller reads
`DAT_00670090->+0x5c` for the result. (UNCONFIRMED pump-exit condition —
§14 item C.)

For progress: NO modal pump loop. The caller's main loop drives
frames; `FUN_0053c1d0` only pumps once to render the popup, then
returns immediately so callers can keep working. The popup persists
until `FUN_0053c360`.

### 10.4 Keyboard

The popup inherits the standard DEF widget input dispatcher
(`FUN_004361f0 DefWidget_DispatchInput` per
`DefWidgetEngine.md:17`):

- Arrow keys cycle focus through the children (yesno/okcancel: between
  the two buttons; ok-only: no-op).
- Enter activates the focused BUTTON (sends cmd 0xbb8).
- 'R' (0x52) hot-reloads the .def (dev/debug only).
- Escape: no popup-private handling; UNCONFIRMED whether the engine
  default closes-with-cancel on ESC (§14 item D). For consistency with
  ingamemenu, expect ESC → close-with-result=1.

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo around BUTTON labels and message body.** Same as every
   other shared-text panel: the GDI 3-pass shadow over a magenta-cleared
   scratch surface sometimes leaves a pink fringe at antialiased edges.
   This is the global retail bug ([[project-retail-pink-halo-bug]],
   UI_METHOD_MAP §16). The reconstruction agent uses real alpha (no
   magenta key) via `DrawTextShadowedToTarget` / `CompositeSwapchainTinted`.
2. **TEXT body centered-in-cell only horizontally.** The intended
   behavior is `TEXT_CENTER | TEXT_SHADOW` with NO `TEXT_VCENTER`. The
   body string hangs from the cell's TOP — not vertically centered. Do
   not "fix" this by adding TEXT_VCENTER unless a panel's own TEXT
   override sets it (none do here).
3. **Slight asymmetric padding (chrome bias ≈8 px right of screen
   center; children ≈10-14 px left of panel center).** See §3 centering
   checks. The two offsets approximately cancel on screen so the popup
   *looks* centered. Do not "fix" either offset independently — the
   chrome bitmap is presumably designed for this exact non-symmetric
   layout. (Speculative: the chrome's left edge may have a wider visual
   border / title bevel than its right, baked into the bitmap art.)

## §12 — Reconstruction pseudocode

The popup has no panel-private paint. It's wholly an instance of the
generic DEF screen + 4 .def-defined PANELs. Reconstruction is:

```
TDefScreen popup_screen;  // global, or per-call instance for progress

// === Open (yesno / okcancel / ok variant) ===
void OpenPopup_YN_or_OkCancel_or_Ok(uint32_t flags, void* msg_arg) {
    // Resolve message:
    const char* msg;
    if (flags & 0x10) msg = (const char*)msg_arg;
    else              msg = LookupStringByKey(g_stringTable, msg_arg);

    popup_screen.msg_ptr = msg;
    popup_screen.flags   = flags;
    uint32_t chrome_flags = (flags & 1) ? 0 : 0x11;

    const char* panel = (flags & 2) ? "yesno"
                      : (flags & 4) ? "okcancel"
                      :               "ok";

    DefScreen_Open(&popup_screen,
                   "popup",          // basename of .def
                   panel,            // which PANEL inside it
                   chrome_flags,     // 0 or 0x11
                   129, 122,         // x, y
                   398, 212,         // w, h
                   308, 100,         // bg_w, bg_h  (or 400,100 for progress)
                   "widgets",        // bg sprite archive
                   "popup");         // outer name (echo of basename)
}

// === Open (progress variant — standalone fresh) ===
void OpenPopup_Progress(uint32_t msg_arg) {
    g_progressSide = AllocSideSurface(/*size=*/0x118, /*flags=*/0x18, ...);
    g_progressSide.drawmode = DM_TRANSPARENT;          // 0x100
    g_progressSide.host_ctx = g_display;

    g_progressPane = new TDefScreen();                  // alloc 0x6c bytes
    g_progressPane.vtable = &vt_5a3ee4;
    g_progressPane.BindSurface(g_progressSide);

    g_display.Box(0,0, g_display.w, g_display.h,
                  0xffff, 0x7f7f, DM_USEDEFAULT);       // clear under popup

    const char* msg = LookupStringByKey(g_stringTable, msg_arg);

    DefScreen_Open(&g_progressPane,
                   "popup", "progress",
                   0x11,
                   129, 122, 398, 212,
                   400, 100,            // NOTE bg_w = 400 not 308
                   "widgets", "popup");
    g_progressPane.msg_ptr = msg;

    ScreenManager_Register(&g_progressPane, 0xffffffff);
    ScreenManager_SetFlags(&g_progressPane, 7);
    ScreenManager_Finalize(&g_progressPane);

    while (!ScreenManager_QueueEmpty())  PumpOneFrame();
    PumpOneFrame();
}

// === Update progress bar ===
void UpdateProgress(uint32_t value, uint32_t max) {
    uint16_t color = PackDisplayColor(0xAA, 0, 0);   // dark red
    int bar_w = (int)((uint64_t)value * 280 / max);   // UNCONFIRMED divisor
    g_progressPane.Box(0, 0, bar_w, 24,
                       color, 0xffff, 0x7f7f, DM_USEDEFAULT);

    TWidget* bm = g_progressPane.FindChild("progress");
    if (bm) {
        int dx = bm.field_60 + g_off_x;   // DAT_0066ff14
        int dy = bm.field_64 + g_off_y;   // DAT_0066ff18
        g_display.DrawSprite(dx, dy, bm.resource, DM_TRANSPARENT);
        g_display.Flush();
    }

    TWidget* msg = g_progressPane.FindChild("progress");
    msg.flags |= CTRLFLAG_DIRTY;          // 0x20
}

// === Close (yes/no/ok) ===
// Driven by Command dispatcher:
int Command_YN_OkCancel_Ok(void* ctrl, uint32_t cmd) {
    BasePostHandler(this, ctrl, cmd);
    if (cmd == 1) {
        FindChild("message")->SetText(this->msg_ptr);
        return 1;
    }
    if (cmd != 3000) return 1;
    const char* name = (const char*)((char*)ctrl + 0x18);
    if (!strcmp(name,"ok") || !strcmp(name,"yes")) {
        this->result = 1;       // positive (note: NOT 0)
        this->Close();          // vtable+0x8
        return 1;
    }
    if (!strcmp(name,"cancel") || !strcmp(name,"no")) {
        this->result = 0;       // negative (note: NOT 1)
        this->Close();
        return 1;
    }
    return 1;
}

// === Close (progress) ===
void ClosePopup_Progress() {
    g_progressPane.Close();              // FUN_00435010
    while (!ScreenManager_QueueEmpty()) PumpOneFrame();
    PumpOneFrame();
    delete g_progressPane;  g_progressPane = nullptr;
    FreeSideSurface(g_progressSide);     g_progressSide = nullptr;
}
```

> The "Close" semantics for yes/no/ok rely on the engine's modal-pump
> exit checking `pane.result`-or-`pane.active` after `vtable+0x8`. The
> exact exit condition is the engine's job, not the popup's; see
> `DefWidgetEngine.md` Renderer / DispatchInput sections.

## §13 — Port mapping notes

Per UI_METHOD_MAP §12 (canonical port primitives):

| retail primitive used by popup | port method | home |
|---|---|---|
| `Background` chrome blit (DM_USEDEFAULT) | `DrawBitmapToTarget` (opaque) | renderer |
| `ClearFrame3` 9-slice around TEXT body | `DrawNineSliceToTarget` (NEW primitive — see below) | renderer |
| `ClearFrame2` / `FillFrame2` BUTTON UP/DOWN states | `DrawNineSliceToTarget` (UP) + state swap (DOWN) | renderer |
| TEXT body (centered, top, shadowed) | `DrawTextShadowedToTarget(atlas, text, cx, cy, cw, ALIGN_CENTER_TOP, 255,255,255, tw,th)` | font.cpp |
| BUTTON label (centered, vcentered, shadowed UP / no shadow DOWN) | `DrawTextShadowedToTarget` (UP) / `DrawTextToTarget` (DOWN) | font.cpp |
| Progress BITMAP fill (clipped subrect to value/max) | `DrawBitmapSubrectToTarget(bm, x, y, 0,0, bar_w,24)` | renderer |
| Progress underlay box (DM_USEDEFAULT colored box) | `TSurface::Box` / `DrawColorBoxToTarget` | surface/renderer |
| Modal pump (yes/no/ok wrapper) | ScreenManager `PumpModal(pane)` — needs new primitive | screen mgr |

**Needs new primitives.** The popup uses a 9-slice (FRAME L T R B M0 M1 M2 M3)
draw both for the TEXT background and the BUTTON chrome. The shared
DEF widget renderer needs a `DrawNineSliceToTarget(asset, dx, dy, dw, dh,
left, top, right, bottom)` primitive on `Renderer`. Once landed, every
DEF screen including popup uses it.

Confirm `ScreenManager_Register/SetFlags/Finalize` line up with
`FUN_0048ed90 / FUN_0048ed60 / FUN_0048eea0` semantics before naming
the port methods (UNCONFIRMED §14 item C).

The `FUN_0053c060` cursor save/restore is a thin wrapper — the port
can do it inline or in the screen-manager helper.

## §13a — Runtime "show message" dispatcher (callers & message-key catalog)

This section documents how the rest of the engine **invokes** `popup.def` at
runtime: which functions call which activator, what `(flags, msg-key)` pair
they pass, and how they read the result back. Everything below is wired
through the activators specced in §6.1–§6.4 — no new popup machinery — but
the call-site geometry is essential for the port to know "where does the
modal pump come from?" and "how do I dispatch a save-overwrite confirm?".

### §13a.1 The three callable shapes (recap)

The retail engine exposes **exactly three entry points** for showing a
popup. The "runtime dispatcher" is literally just these three; there is no
intermediate `ShowMessage(key, kind)` helper.

| entry | role | who reads result | called from N sites |
|---|---|---|---|
| `FUN_0053c060(msg_key, flags)` | embedded yes/no/ok wrapper. Saves cursor, calls `FUN_0053bf00` on the GLOBAL TDefScreen instance `DAT_00670090`, **runs the modal pump (`FUN_0048f040(.,7)`)**, returns popup result in `eax`. | caller, immediately after the call returns | **38** |
| `FUN_0053c1d0(msg_key)` | standalone progress popup. Allocates two new globals (`DAT_0066ff10` pane + `DAT_00670214` side-surface), opens `panel="progress"`, drains the event queue ONCE, returns — caller drives the pump | none yet; result is read by `FUN_0053c360` close | **1** |
| `FUN_0053bf00(this, flags, msg_arg)` | low-level open on a caller-owned TDefScreen. Caller must run own pump and close. | caller | **0** direct external callers (only `FUN_0053c060` calls it at 0x53c09d). |

The "runtime dispatcher" surface is therefore:
- **`FUN_0053c060` for any modal yes/no/ok**. Returns popup result code synchronously.
- **`FUN_0053c1d0` + `FUN_0053c3d0` + `FUN_0053c360` for progress**. Asynchronous, caller updates and closes.

Cite: `objdump -d data/Revenant.exe | grep -E "call.*0x53c060|call.*0x53c1d0|call.*0x53c360|call.*0x53c3d0"` (38 + 1 + 1 + 3 sites; tally `[recon raw scan]`).

### §13a.2 `FUN_0053c060` calling convention (cdecl, 2 stack args)

The 38 call sites all push `(msg_key, flags)` in cdecl order and clean
8 bytes with `add esp, 8`. **Note the order**: `FUN_0053bf00` is
__thiscall with `(flags, msg_arg)` in stack-args 1/2; `FUN_0053c060` is
__cdecl and **swaps the order** so the caller pushes `flags` LAST.

```
push <flags>             ; bottom of stack
push <msg_key_ptr>       ; top of stack
call 0x53c060            ; -> FUN_0053c060
add  esp, 8
test eax, eax            ; result: 1 = positive (Yes/Ok), 0 = negative (No/Cancel)
```

Inside `FUN_0053c060`, after the cursor swap, it loads the two args back
off its own stack and forwards them as `FUN_0053bf00(DAT_00670090,
flags, msg_key)` — verified at `0x53c098`. Pump runs at `0x53c0af`; pump
return is what `FUN_0053c060` returns to its caller.

`eax` at return is the modal pump's `FUN_0048f040(DAT_00670090, 7)`
return. Empirically the caller-side code unconditionally treats it as
"1 = Yes/Ok was pressed" (e.g. `test eax, eax / je <skip-exit>` at
`0x490d11`, `0x47ca7f`, `0x49132b`). UNCONFIRMED §14C in detail — but
the **observable contract** for callers is: `FUN_0053c060` returns 1
when positive, 0 when negative, 0 if popup was canceled by an external
mechanism (ESC etc.).

### §13a.3 Flag bits observed in the wild

Disassembly of all 38 sites shows **exactly three flag values**:

| flag | binary | panel selected | chrome | msg form | observed usage |
|---:|---|---|---:|---|---|
| `0x03` | `0000 0011` | "yesno" (bit 1) | `0` (bit 0) | string-table key | confirm prompts (`quitgameyn`, `exitgameyn`, `EXITGAMEYN`, `MPDELCHAR`) |
| `0x09` | `0000 1001` | "ok" (no bit 1/2) | `0` (bit 0) | string-table key | error/info popups (all MP failures, `GAMENOTFOUND`-style) |
| `0x19` | `0001 1001` | "ok" (same as 0x09) | `0` (bit 0) | **literal C string** (bit 4 = 0x10 → msg_arg is char*) | dynamically-formatted error strings (`wsprintf` into a local buffer, then pop up) |

Cite: `objdump … | grep -B 4 "call.*0x53c060" | grep "push 0x"` — sole
push immediates seen are `0x3`, `0x9`, `0x19`. No site passes `0x4`
(okcancel), `0x10` alone, or any other combination.

> **Bit `0x08` mystery.** Every retail caller sets bit 3 = `0x08`. The
> activator (`FUN_0053bf00`) explicitly tests bit 0, bit 1, bit 2, bit
> 4 (see §6.1) but **does NOT test bit 3**. The pre-handler
> `FUN_00437a80` (called by the Command dispatcher at `0x53bfaf`) may
> consume it, or it may be a logging/category hint that's ignored by
> the popup engine entirely. UNCONFIRMED — see §14J.
>
> **Observed practical decoding (port-safe):**
> - `flags == 0x03` → yes/no confirm, msg is string-table key
> - `flags == 0x09` → ok-only error/info, msg is string-table key
> - `flags == 0x19` → ok-only error/info, msg is literal `const char*`
>
> No site uses `okcancel` ("save overwrite confirm" is not a retail
> feature — see §13a.6). The port can assume these three flag values
> are the only ones in use.

### §13a.4 Message-key catalog (full retail census)

The 38 `FUN_0053c060` sites resolve to these unique message keys
(string-table tokens; English text from
`data/resources_unzipped/english.def`). The caller table identifies which
class/method invokes which key.

| key | flag | text (english.def) | called from | call-site (.exe addr) |
|---|---:|---|---|---:|
| `MPREPLYFAIL` | `0x09` | (MP — not in english.def excerpt; reply-msg-fail) | `FUN_00461900` | `0x461b2b` |
| `MPMUSTSELCHAR` | `0x09` | "Please select a character first." (UNCONFIRMED text — not in excerpt) | `virt_meth_0x462e50`, `virt_meth_0x46bd60`, `virt_meth_0x463f00` (multiple sites) | `0x4631fe, 0x463395, 0x463695, 0x463752, 0x46bfe7, 0x46c028, 0x46c33f, 0x46c38a` |
| `MPMUSTSELMOD` | `0x09` | (MP — must select module) | `virt_meth_0x462e50`, `virt_meth_0x46bd60` | `0x463227, 0x4633be, 0x46c06d` |
| `MPMUSTSELGAME` | `0x09` | (MP — must select game) | `virt_meth_0x462e50`, `virt_meth_0x46bd60` | `0x4633e9, 0x46c2dd, 0x46c310, 0x46c3d3` |
| `MPMODULEUNAVAILABLE` | `0x09` | (MP — module unavailable) | `virt_meth_0x462e50`, `virt_meth_0x46bd60` | `0x463444, 0x46c434` |
| `MPDELCHAR` | `0x03` | "\nDeleted characters can not be restored. Are you sure you want to delete this character." | `virt_meth_0x462e50` | `0x46379b` |
| `quitgameyn` | `0x03` | "\nAre you sure you want to quit the current module?" | `virt_meth_0x463f00` (MainMenu/HostGame), `virt_meth_0x537190` | `0x464471, 0x537326` |
| `MPMUSTBEHOST` | `0x09` | (MP — must be host) | `virt_meth_0x463f00`, `virt_meth_0x537190` | `0x4645a7, 0x464665, 0x537262` |
| `MPCANTEDIT` | `0x09` | (MP — can't edit) | `virt_meth_0x463f00` | `0x464b34` |
| `MPCANTVIEW` | `0x09` | (MP — can't view) | `virt_meth_0x463f00` | `0x464b7a` |
| `MPSELSTARTFAILED` | `0x09` | (MP — session start failed) | `virt_meth_0x469460` | `0x4696ee` |
| `MPCOULDNTCONNECT` | `0x09` | (MP — connect failed) | `virt_meth_0x46a720`, `virt_meth_0x46bd60` | `0x46a79c, 0x46c501` |
| `MPLOGINFAILED` | `0x09` | (MP — login failed) | `virt_meth_0x46a720`, `virt_meth_0x46bd60` | `0x46a7fb, 0x46c54a` |
| `MPUPLOADFAILED` | `0x09` | (MP — upload failed; same arg slot as `MPLOGINFAILED`, alt-branch) | `virt_meth_0x46a720` | `0x46a7fb (alt)` |
| `mpdisconnectedtimeout` | `0x09` | (MP — disconnected via timeout) | `virt_meth_0x47bd20` (TPlayScreen pump tick) | `0x47c298` |
| `mpdisconnected` | `0x09` | (MP — disconnected; same arg slot) | `virt_meth_0x47bd20` | `0x47c298 (alt)` |
| `exitgameyn` | `0x03` | "\nAre you sure you want to exit the game and return to Windows?" | `virt_meth_0x47c630` (TPlayScreen GUI handler), `virt_meth_0x537190` | `0x47ca6b, 0x537378` |
| `EXITGAMEYN` | `0x03` | same — alt-cased duplicate string-table entry | `virt_meth_0x490bd0` (WM_CLOSE handler), `FUN_004911b0` (screen-mgr quit) | `0x490d09, 0x491321` |
| `loadingmap` | (n/a — progress) | "The game is loading map graphics, characters, and animations… Please Wait." | `virt_meth_0x539590` (`cls_0x5b9584::LoadGameScreen::Command`) **via `FUN_0053c1d0` (progress)** | `0x539701` |
| dynamic msg `0x19` | `0x19` | wsprintf'd into local 256-byte buffer; varies — disconnect details, fail reasons | `virt_meth_0x463f00`, `virt_meth_0x537190` | `0x464567, 0x5372f4` |

Pattern: **everything Multiplayer-related (MP*)** is an info-OK popup;
the only **yes/no** uses are the three exit/quit confirmations and
`MPDELCHAR`; the only **progress** use is `LoadGame` map-streaming.
The literal-string `0x19` cases fall to **wsprintf into local buf →
push buf** (e.g. at `0x464560`: `lea edx, [esp+0x70]` after a 256-byte
`wsprintfA` builds, then `push edx, push 0x19, call 0x53c060`).

### §13a.5 `GAMENOTFOUND` — clarification (NOT a popup)

The brief mentioned `FUN_0054d170(&DAT_0065c5d0, "GAMENOTFOUND")` and
similar runtime "show message" calls. **These are NOT popup invocations.**
`FUN_0054d170` is the TPlayScreen *message-log* printf shim (the in-game
yellow-text message line above the inventory bar); it is a thin wrapper
around `FUN_0054d1b0` which calls `meth_0x54d0c0` (appends a colored entry
to the TPlayScreen message-ring at `+0x6c`) and ALSO writes the same
text to `%sTextDump.txt` when `DAT_00668178` (the text-dump enable flag)
is set.

Cite:
- `FUN_0054d170` body at `.text:0x54d170..0x54d185` —
  pushes args + flag `1` (the message category) and tail-calls
  `FUN_0054d1b0` (`[recon raw scan: objdump -d --start-address=0x54d170 …]`).
- `FUN_0054d190` body at `.text:0x54d190..0x54d1a8` — variant that
  takes the category as caller-supplied arg-0 (used for `category=0x10`
  saves at `0x5a5320:1531` and category=1 at `0x5a5ff0:214`).
- `FUN_0054d1b0` body at `.text:0x54d1b0..0x54d2ec` — opens
  `"%sTextDump.txt"` (`s__sTextDump_txt_005e582c`), formats via
  vsnprintf-like helper, appends to message log via `meth_0x54d0c0`.
  String literal `"Revenant Text Dump executed at …"` at
  `s_Revenant_Text_Dump_executed_at___005e5840` cited at
  `meth_0x54d1b0:0x54d22c` (recon
  `recon/classes_original/_data.txt:117135`).
- DAT_0065c5d0 is the **TPlayScreen global instance**, not a popup
  pane; `meth_0x54d170` is a method on `cls_0x5a5320` (mislabeled
  `TCharacter` by OOAnalyzer; the host file is
  `recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp` but the class is
  actually TPlayScreen — see recon notes on the 0x5a5320 / TPlayScreen
  conflation).

**TPlayScreen::Initialize** (`virt_meth_0x47a660`) uses this at
`0x47af0a..0x47af19`:

```
; if module open failed (line 0x47af04 jne'd past); else:
push 0x5d7424                ; "GAMENOTFOUND" (string-table key)
mov  ecx, 0x65d4d0           ; the global string-table
call 0x49d800                ; LookupStringByKey(table, key) → eax = "Unable to load saved game..."
push eax                      ; resolved text
push 0x65c5d0                ; TPlayScreen* (DAT_0065c5d0 — global instance)
call 0x54d170                ; -> AppendMessageLog(this, text) — NOT a popup
add  esp, 8
mov  ecx, 0x65a618           ; module manager
call 0x48e610                ; mount next/default module
jmp  0x47b060                ; continue init
```

`GAMENOTFOUND` posts a **transient log message** ("Unable to load
saved game… starting new game") to the message line and continues with a
fresh-game boot. There is **no modal popup** in this path. The popup
would be visible briefly during the LoadGame screen's progress pump if
shown there (but it's not — this is an in-game message).

**Implication for the port.** Any "show transient text message"
implementation must NOT route through `popup.def`. It needs a
**separate** path that emits to the HUD message-log widget (the colored
fade-out text drawn above the HUD at the top of the play area). The
port already has a HUD text-message line; this dispatcher (54d170 /
54d190 / 54d1b0 / 54d0c0) is the producer.

> Other `FUN_0054d170(&DAT_0065c5d0, ...)` call sites: 60+ sites scattered
> across `cls_0x4922c0` (TArea), `cls_0x5a5320` (TPlayScreen),
> `cls_0x5a7b98` (TPlayer/TCharacter), `cls_0x495cf0` (TScriptManager),
> `cls_0x5b8e94` (TObjectInstance Final) — every gameplay subsystem
> uses it as its "warning printf". None of them are popup callers.

### §13a.6 "Autosave-in-progress" warning & "Save overwrite confirm" — neither exists in retail

The brief specifically asked for the **`autosaveYN` modal** (warn that
autosave is running, prompt continue/cancel) and the **save-overwrite
confirm** (Y/N before clobbering an existing slot). **Neither popup
exists in shipped retail**, based on exhaustive string-table grep and
SaveGameDef cmd-dispatch decode.

#### A. autosaveYN — does NOT exist as a popup

- **String-table grep** (`grep -aE "autosave" data/resources_unzipped/english.def`)
  returns nothing matching an `autosaveYN`-style key — no `AUTOSAVEYN`,
  no `AUTOSAVE_IN_PROGRESS`, no `autosaveYN`. Only `AUTOSAVE` /
  `AUTOSAVETIME` (the bare nouns) appear, in `s_Autosave_005d8344`
  used by `FUN_00484500` (the *settings* code that reads/writes the
  `[Autosave]` ini section — `s_AutosaveTime_005d8350` etc., recon
  `recon/classes_original/_data.txt:88692-88699`). This is **settings
  text**, not a popup.
- **No popup activator call site** carries a message key matching
  `autosav*` or `AUTOSAV*` — every push immediate before a
  `call 0x53c060` (the 38 sites in §13a.4) is enumerated above and
  none point to such a key.
- **Banner string** `s_Autosave_[_005d74f4` ("Autosave [") at
  recon `_data.txt:86514` is used by `virt_meth_0x47bd20` (TPlayScreen
  pump tick) at `0x47be8d` — but **NOT through the popup dispatcher**.
  This is the in-game `[Autosaving...]` HUD banner — written via the
  same message-log printf path (`FUN_0054d170`-family) or via direct
  HUD-text widget, NOT through `popup.def`.

**Conclusion**: retail Autosave is silent (no modal interrupting
gameplay). It writes to disk on a timer (`AutosaveTime` ini setting),
shows a brief log-line ("Autosave [<slotname>]"), and continues. There
is NO "Save in progress, please wait" modal popup in shipped Revenant.

> **Port note.** If the modernization plan wants an autosave-in-progress
> popup (e.g. because asynchronous file I/O takes long enough to need
> user feedback), it is a **new feature**, not a retail port. Use the
> `progress` popup variant (`FUN_0053c1d0` + `FUN_0053c360`) with a new
> string-table key (e.g. `AUTOSAVEPROGRESS`). The reconstruction agent
> should not add this without explicit scope approval.

#### B. Save-overwrite confirm — does NOT exist as a popup

- **String-table grep** finds two "overwrite" strings, both
  **console-stdout debug prompts**, not popups:
  - `s_Do_you_wish_to_overwrite_it_(Y/N_005cf800` ("Do you wish to
    overwrite it (Y/N) ?\n") at `_data.txt:72922`, XREF'd from
    `0x443da9` (`virt_meth_0x44...` — a dev/editor `fprintf(stdout,...)`
    + `scanf` Y/N tail).
  - `s_%s_already_exists.._Overwrite_(Y_005cfd18` ("%s already
    exists.. Overwrite (Y/N)\n> ") at `_data.txt:73211`, XREF'd from
    `0x444cf2` — same pattern, editor-only.
  Both are **CRT stdin/stdout** lines, the editor's terminal Y/N
  prompts. Not used by `popup.def`.
- **SaveGameDef Command dispatcher (`virt_meth_0x539c00`)** — the
  retail SaveGame screen's command handler at `0x539c00..0x539f4f`
  decodes the "Save" button press (`cmd == 0x1388 = 5000` or
  `0x1771 = 6001` at `0x539d09..0x539d15`). When the Save action
  fires, the cleaned filename (whitelisted via the safe-char filter at
  `0x539e8b..0x539ec3`) is passed to `meth_0x48d720` (= `SaveGame(name)`
  per `agent_player_init.txt:116-117`) **directly, with no
  pre-check** for filename collision. The Save path at
  `0x539ef7..0x539f10` is:
  ```
  push 0    ; flags = 0
  lea  edx, [esp+0x10]   ; the cleaned filename
  push edx
  mov  ecx, 0x65a618     ; module-manager (TPlayScreen-side global)
  mov  [ebp+0x5c], 0x1   ; this->result = 1 (positive close)
  call 0x48d720          ; SaveGame(name) — writes through
  mov  eax, [ebp]        ; vtable
  mov  ecx, ebp
  call dword ptr [eax+8] ; this->Close() — close savegame screen
  ```
  Retail SaveGame writes the file unconditionally; if the slot existed,
  it's overwritten silently after the existing `MapPane::ClearCurMap` +
  `MapPane::SaveCurMap` pair (`save_system_notes.md` §"Save game", steps
  1–6 — the slot dir is `CreateDirectoryA`'d idempotently, then
  `FindFirstFile *.DAT` + `DeleteFileA` clears any prior contents).
  **There is no UI confirm.**
- The "exit" button branch (`virt_meth_0x539c00`, cmd-by-name "exit"
  at `0x539f22..0x539f43`) sets `[ebp+0x5c] = 0` (negative result) and
  closes — also no popup.

**Conclusion**: retail Save is "silent overwrite". Picking an existing
slot in the list, typing a new name, and hitting Save just clobbers the
old slot without asking.

> **Port note.** A save-overwrite confirm IS a sensible modernization
> (modern UX expectation) and the popup engine already supports the
> required `okcancel` variant (`flags & 4` in `FUN_0053bf00` selects it
> — see §6.1). To add it, the port's SaveGameDef Command handler can
> check whether `<SavePath>Save/Single/<typed_name>/game.sav` exists,
> and if so invoke `FUN_0053c060(<some_new_key>, 0x05)`
> (`flags = 1 | 4 = okcancel + chrome_flags=0`, msg from string
> table; the new key needs adding to `english.def`). The popup result
> is then `1 = OK (overwrite)` / `0 = Cancel`. **This is greenfield
> modernization, not a retail port.** Coordinate with the
> distribution-model policy before adding.

### §13a.7 The single retail progress dispatcher — LoadGame map streaming

`FUN_0053c1d0` has **exactly one caller** in the entire .exe:
`virt_meth_0x539590` (`cls_0x5b9584::LoadGameScreen::Command`), at
`0x539701`. The full sequence is the only retail use of the progress
variant, and it's the canonical "long operation popup" pattern.

```
; In LoadGameScreen::Command, after the user clicks a save slot
; (cmd == 0xbb8 = 3000) and the file is verified loadable
; (string compare against "gamelist" at 0x5398e8 — i.e. checks the
; widget name; non-null = valid selection):

push ebx                  ; ebx = 0 (clear cursor handle)
call 0x43a020             ; cursor save/load wait cursor
push 1
call 0x4911b0             ; FUN_004911b0(1) — pump the screen manager
                          ; ONCE so the load screen redraws before
                          ; the progress popup appears

push 0x5e42dc             ; key = "loadingmap"
call 0x53c1d0             ; LoadDef_popup_progress("loadingmap")
add  esp, 0xc             ; arg + the earlier two

; Now do the load and report progress at fixed milestones:

mov  ecx, [0x66fb04]      ; selected slot index
push ebx                  ; sub-flag = 0
push ecx
mov  ecx, 0x65a618        ; module-mgr global
call 0x48e5b0             ; ModuleManager_LoadSelected — actual
                          ; LoadGame routine (game.sav + curmap)

push 0x50                 ; 80 / 800 = 10% — first milestone
call 0x53c3d0             ; Update_popup_progress(80)
add  esp, 4

; (additional load steps + script-state restoration; cleared elided)

push 0x320                ; 800 / 800 = 100%
call 0x53c3d0             ; Update_popup_progress(800)
add  esp, 4

call 0x53c360             ; Close_popup_progress
```

Cite all: `objdump … --start-address=0x5396ef --stop-address=0x53975e`.

**The `max` for `Update_popup_progress` is 800.** The function at
`0x539990` (a small inline helper just before `0x5399af`) computes
`bar_width_steps = sectors_loaded * 8` (= the `n*100/100*8` magic-divide
sequence at `0x539994..0x5399a4`), then passes that step value to
`Update_popup_progress`. The hardcoded `0x50 = 80` and `0x320 = 800`
endpoints are the integer-units (not pixels) on a 0..800 scale. Per
§14E, the BITMAP fill width formula is most likely
`bar_width_pixels = value * 280 / 800` (since the bitmap is 280 px and
800 is the "full" value here). **Resolves §14E**: `divisor = 800` for the
LoadGame use case. Other callers (if any future progress pops exist)
might pass a different scale; the formula is `bar_w = value * 280 / max`
where `max` is the caller's choice — confirmed by the magic-multiply
`0x10624dd3` being `divide-by-100` and the surrounding shift/lea
pattern multiplying by `25*32 = 800`.

### §13a.8 Result read-back — how callers use the popup result

For all 38 `FUN_0053c060` sites, the read-back pattern is identical:

```
call 0x53c060
add  esp, 8
test eax, eax
je   <skip-positive>      ; if result==0 (No/Cancel), bail out
<...positive action...>   ; if result==1 (Yes/Ok), do the thing
```

Example — `EXITGAMEYN` at `0x490d09`:
```
push 0x3                  ; flags = yesno
push 0x5d9ec4             ; key = "EXITGAMEYN"
call 0x53c060
add  esp, 8
test eax, eax
je   0x490d52             ; result==0 (No) → don't exit
push ebp                  ; result==1 (Yes) → push window handle
call [0x5a3298]           ; -> PostQuitMessage(0) (the WM_QUIT path)
```

Cite: `objdump … --start-address=0x490ce0 --stop-address=0x490d30`.

The convention is rigid: **`eax == 1` means the positive button was
pressed**. The polarity is the OPPOSITE of C's `0 = success`; the port
must preserve this or every yes/no handler will be inverted.

### §13a.9 Port-side dispatcher shape

To map this to the port:

```cpp
// === Synchronous yes/no/ok popup ===
// Returns 1 = positive (Yes/Ok), 0 = negative (No/Cancel).
// Pumps the screen manager internally — caller is blocked until close.
int ShowPopup_Modal(const char* msg_key_or_literal, uint32_t flags);

// Usage examples (matching retail call shapes):
//   ShowPopup_Modal("EXITGAMEYN", 0x03);   // Yes/No: confirm exit
//   ShowPopup_Modal("MPMUSTSELCHAR", 0x09);// OK: "select a character"
//   char buf[256]; wsprintf(buf, "Disconnected: %s", reason);
//   ShowPopup_Modal(buf, 0x19);            // OK: formatted literal

// === Asynchronous progress popup ===
// Open / step / close — caller drives its own work loop in between.
void OpenPopup_Progress(const char* msg_key);   // = FUN_0053c1d0
void UpdatePopup_Progress(uint32_t value);      // = FUN_0053c3d0
                                                // (max baked in by caller — 800 for LoadGame)
void ClosePopup_Progress();                     // = FUN_0053c360

// Retail usage (LoadGame map streaming):
//   OpenPopup_Progress("loadingmap");
//   ModuleMgr_LoadSelected(slot, 0);
//   UpdatePopup_Progress(80);     // 10%
//   ...do work...
//   UpdatePopup_Progress(800);    // 100%
//   ClosePopup_Progress();
```

The port should:
1. Keep the **literal `flags` byte** as the public API rather than
   replacing it with an enum — the 0x08 mystery bit means retail
   callers might rely on bit-3 behavior we haven't decoded, and a new
   `PopupKind::YesNo` enum would silently drop unknown bits.
2. **Preserve the result polarity**: `1 = positive`, `0 = negative`.
3. Keep `OpenPopup_Progress` callable from the caller's main loop —
   the retail caller continues to drive `FUN_004911b0(1)` (= the
   screen-manager's per-frame pump) between progress updates. The
   progress popup is NOT modal in the pump-blocks-caller sense; it
   only does ONE pump to draw itself, then returns.
4. The string-table lookup (= `FUN_0049d800(g_strings, key)`) is
   shared with EVERY DEF screen's localized text — see
   `recon/discovered/exits_doors_notes.md:108` and the call sites
   listed above for `FUN_0049d800` use. The port already needs this
   for general localization (see [[project-localization]]).

## §14 — UNCONFIRMED / open questions

A. **`bg_w / bg_h` meaning** (308×100 for yes/no/ok variants;
   400×100 for progress variant). They are passed to DefScreen_Open as
   the 8th and 9th stack args (per `B_r7_def_widget_engine.md:49-50`
   `(panel_name, screen_name, flags, x,y, w,h, bg_w, bg_h,
   bg_sprite_name, outer_name)`). The `Background` bitmap itself is
   measured at 398×212, NOT 308×100, so these literals cannot be the
   chrome dimensions. Plausible interpretations:
   - title-bar / fixed-content area inside the pane;
   - the "widgets" archive sprite used as a non-chrome inner panel;
   - a hint to the DEF-engine's layout pass for content stretching.
   Why progress passes 400 (not 308) is also unknown.
   **Resolve:** `DecompileAddr.java 0x00435040` (DefScreen_LoadAndShow,
   full body) — currently only the prologue param-decode is captured.
   The body forwards bg_w/bg_h into `vtable+0x20(arg, arg)` on the pane,
   which is a setter we haven't yet identified.
   **Impact if wrong:** the popup chrome may scale or position oddly
   in the port. Worst case: text/buttons inside the pane misalign with
   the chrome graphic by some pixels.

B. **Command-handler vtable slot.** The dispatcher `FUN_0053bfa0` /
   `FUN_0053c150` is reached when a child widget emits a command, but
   we haven't dumped the vtable to confirm WHICH slot they're wired
   into. Per `DefWidgetEngine.md` the slot is one of `+0xb4` /
   `+0xb8`, but we haven't seen the popup pane class vtable.
   **Resolve:** the popup uses the **generic** DEF screen as its
   instance class (no popup-specific vtable) — so the slots are
   inherited from `cls_0x5b93c4`. `DumpVtable.java 0x5b93c4` → match
   the dispatcher addresses. The progress side uses a different vtable
   (`0x5a3ee4` set at `FUN_0053c1d0:0053c24a`) — `DumpVtable.java
   0x5a3ee4`.
   **Impact if wrong:** the port might wire Command into the wrong slot
   and the buttons would silently fail to dispatch.

C. **`FUN_0048f040(pane, 7)` modal-pump semantics.** `FUN_0048ed90`,
   `FUN_0048ed60`, `FUN_0048eea0` are screen-manager registration
   helpers; `FUN_0048f040` is the modal/pump call. The mask `7` is
   probably `visible+selectable+active` flag bits but UNCONFIRMED.
   The pump's exit condition (likely "pane no longer in screen-mgr's
   active list" after the close call) is also unknown.
   **Resolve:** `DecompileAddr.java 0x0048f040` then trace its loop.
   **Impact if wrong:** progress popup might not pump enough frames to
   draw on first open, or yes/no popup might pump forever.

D. **Escape key handling.** Whether the popup closes-with-cancel on ESC
   is not in `FUN_0053bf00`/`bfa0` — it would have to be in the engine's
   `FUN_004361f0 DefWidget_DispatchInput`. By analogy with the in-game
   menu (which the player CAN close with ESC), the popup probably does
   close on ESC.
   **Resolve:** decompile `FUN_004361f0` to see if it has a global
   ESC→close path; cite the result.
   **Impact if wrong:** user can / cannot dismiss popup with ESC.

E. **`FUN_0053c3d0` bar-width formula.** *Partially resolved by
   §13a.7.* The arithmetic between `0x53c412` and `0x53c434` is a
   compiler-emitted `n*100/N` style fast-divide; the magic constant
   `0x10624dd3` is the multiplier for "divide by 100". Combined with
   the surrounding `lea/shl` chain it computes `n * 800 / 100 * 8` =
   `n * 64`-ish — but the exact shape of `FUN_0053c3d0`'s own divider
   (vs the caller-side helper at `0x539990`) is still UNCONFIRMED.
   The retail LoadGame call uses `max = 800` (= the hardcoded `0x320`
   endpoint at `0x539754`), so `bar_w = value * 280 / 800` is the
   working formula for retail's sole progress popup. Other future
   progress popups would set their own `max`.
   **Resolve:** symbolic-execute the 0x53c412..c43b block on a known
   pair (e.g. value=400, max=800 → bar_w should be 140).
   **Impact if wrong:** progress bar grows at wrong rate.

F. **`DAT_0066ff14` and `DAT_0066ff18` (off_x, off_y) for progress
   bar dst.** Read-only at `Update_popup_progress` time but never
   written in the popup activator code we've extracted. Likely written
   when the progress widget is parsed in `FUN_00437000 ParsePanel`
   (the POS 49 126 literal stored). Plausible value:
   `DAT_0066ff14 = 49`, `DAT_0066ff18 = 126` (the BITMAP POS literals
   from popup.def:60), composed with the pane origin.
   **Resolve:** trace the writers of `DAT_0066ff14`/`DAT_0066ff18`
   in the engine's parser.
   **Impact if wrong:** progress bar paints in the wrong place.

G. **Font "Med" — point size.** popup.def + widgets.def use FONT "Med"
   for both TEXT bodies and BUTTON labels. The exact px size is not in
   the .def — it comes from `font.def`. UNCONFIRMED.
   **Resolve:** read `data/resources_unzipped/font.def` for the "Med"
   entry size.
   **Impact if wrong:** text would render too small/large to fit the
   cells.

H. **Background-vs-bg_sprite split.** DefScreen_Open is passed BOTH a
   `bg_sprite_name = "widgets"` (which resolves to the `widgets.dat`
   archive containing `ClearFrame*`, etc.) AND an `outer_name = "popup"`
   (which presumably resolves to one of `popup{tex,notex,alpha}.dat`'s
   `Background`). The two-name pair is presumably "where to find the
   inner-content sprites" vs "where to find the chrome". UNCONFIRMED
   in exact semantics.
   **Resolve:** see `DecompileAddr.java 0x00435040` body §14A.
   **Impact:** if reversed in the port, the popup tries to load
   `Background` from `widgets.dat` and fails.

I. **`FUN_004a3a40(pane, dp)` — exact role.** At `FUN_0053c1d0:0053c250`
   the new pane is bound to the side-surface descriptor `dp`. Inferred
   from context (it sets the pane's draw target). UNCONFIRMED.
   **Resolve:** `DecompileAddr.java 0x004a3a40`.

J. **Bit `0x08` in popup flags.** Every one of the 38 retail
   `FUN_0053c060` call sites passes `flags = 0x03 | 0x08 = 0x0b`-ish
   (actually exactly `0x03`, `0x09`, or `0x19` — all with bit 3 set).
   `FUN_0053bf00` (§6.1) does NOT branch on bit 3. The bit may be:
   - consumed by `FUN_00437a80` (the base post-handler called at
     `0x53bfaf` before the panel select), e.g. a debug/logging hint;
   - consumed by `FUN_0048f040` (the modal pump);
   - or vestigial — set by callers because a header constant included
     it, but ignored by the receiver.
   **Resolve:** `DecompileAddr.java 0x00437a80` + grep for `test … 0x8`
   in the popup-engine path. Cross-check whether other callers (e.g.
   any non-popup callers of `FUN_00437a80`) inspect bit 3.
   **Impact if wrong:** the port might omit a side-effect on popup
   open — e.g. a sound effect or a screen-mgr category change. None
   currently observed in the recon, so the default safe move is to
   forward bit 3 verbatim through the port's `flags` arg.

K. **TPlayScreen-side standalone progress allocation at `0x47ca94`.**
   The `virt_meth_0x47c630` path at `0x47ca94..0x47caf7` allocates a
   side-surface (`0x280, 0x1e0` = full 640×480), then a 0x6c-byte pane
   with vtable `0x5a3ee4` (same as the progress popup's pane vtable,
   per `FUN_0053c1d0:0053c24a`), then calls `FUN_004a3a40` to bind.
   This is the **exact same allocation sequence** as the progress
   popup, but executed inline here. It's reached when **no save slot
   exists** (the `[ebx + 0x5d8]` test). Plausible purpose: a "save
   before exit" forced progress popup, OR an in-place exit-screen
   construction. UNCONFIRMED.
   **Resolve:** trace what `0x47cafc..` does with the new pane after
   the bind; check whether it calls `DefScreen_Open("popup",
   "progress", …)` or some other DEF screen.
   **Impact:** if it IS the same progress popup, the port should
   reuse the same `OpenPopup_Progress` API instead of inline-allocating.
   If it's a different screen, the port needs a separate entry point.

L. **`autosaveYN` and save-overwrite confirm — confirmed NOT in retail.**
   *See §13a.6.* Documented here as resolved-by-absence: an exhaustive
   string-table grep + a full sweep of the 38 popup activator call
   sites finds no `autosaveYN`-style key and no okcancel-flag use.
   Retail Autosave is silent (writes-and-logs), and retail Save is
   silent-overwrite. **The brief's "autosaveYN modal" and "save-overwrite
   confirm" are new features for the port to design from scratch, NOT
   retail items to port.** Resolve-step for the port: decide as a
   modernization-stance question whether to add them; if yes, route
   through the existing okcancel path with new string-table keys.

---

## Summary self-check

- Every coordinate in §4 is panel-local; the composition formula
  (§3) plus the literal pane origin `(129, 122)` yields screen coords.
- Every (x,y,w,h) is cited to either a `popup.def` line OR a
  disassembly address with the literal in hex.
- The yes/no/ok vs progress variant axis is fully decoded: variant
  selection lives in `FUN_0053bf00` flags `&2` / `&4` for yes/no/ok;
  the progress variant has its own activator (`FUN_0053c100` in-place
  or `FUN_0053c1d0` standalone) hard-wired to `panel = "progress"`.
- The cmd-id model is documented: all BUTTONs emit `0xbb8 = 3000`,
  the dispatcher discriminates by NAME, and result is **1 (positive
  — ok/yes)** or **0 (negative — cancel/no)**. The polarity is
  inverted from the obvious C convention — see §6.2 NOTE.
- No panel-private paint exists — paint flows through the shared DEF
  widget engine, which is itself a separate port-status row.
- The runtime dispatcher is fully enumerated: 38 yes/no/ok call sites
  via `FUN_0053c060`, 1 progress call site via `FUN_0053c1d0` (LoadGame
  map streaming with `max = 800`). Flag values observed are exactly
  `0x03`, `0x09`, `0x19`. Message-key catalog in §13a.4.
- `GAMENOTFOUND` and the rest of the `FUN_0054d170(&DAT_0065c5d0, ...)`
  family are the **TPlayScreen message-log printf**, NOT popup
  invocations — §13a.5.
- `autosaveYN` and save-overwrite confirm popups do NOT exist in
  retail. Both are documented as modernization candidates in §13a.6
  and §14L.
- Open questions live in §14 with concrete resolve steps.
