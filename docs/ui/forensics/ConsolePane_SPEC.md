# TConsolePane — Reconstruction SPEC

The **editor** interactive command-line console pane (EXILE editor only — NOT
visible during normal gameplay). A plain black-background scrolling text buffer
with cursor, paged scrollback, color-escape (`{`/`}`) marker handling, an
optional "chained mouse" widget for click-and-drag numeric entry, and a
background command-processor thread that hangs on a CharAvailable event for
GetLine. Retail leaf class `cls_0x5a4358` (vtable `0x005a4358`), global
singleton `0x00656ce8` (= the editor's global `Console`).

---

## §0 — Sources & status

**Class identity — DECISIVE (95%+).** `cls_0x5a4358` = **TConsolePane**
(EDITOR command console). Three independent lines of evidence pinned by Wave-1C
brief `docs/ui/briefs/B_r4_textbar_assessment.md`:

1. Two unique error strings in `virt_meth_0x43e370` at `0x0043e370`:
   - `s_Could_not_create_kill_event_for_c_005ceefc`
     = `"Could not create kill event for command processor!"`
   - `s_Could_not_create_character_event_005cef30`
     = `"Could not create character event for command processor!"`
   Both appear **only** inside `TConsolePane::Initialize` in the entire
   pre-release `attic/src/editor.cpp:546` and `:550` — nowhere else in the source.
2. Same `virt_meth_0x43e370` calls `CreateEventA` × 2 then `FUN_0058bd47`
   (= `_beginthreadex`) with entry `0x0043f5a0` and `(int)this`, then
   `SetThreadPriority(hThread, 1)`. Line-for-line `TConsolePane::Initialize`
   from `attic/src/editor.cpp:531-563`. **No other pre-release class spawns a
   thread inside `Initialize`** — structural fingerprint.
3. Parent-class call at top of Initialize (`virt_meth_0x43da00` on `this`,
   `recon/ghidra/cls_0x5a4358_TConsolePane.cpp:339`) is byte-identical to
   `cls_0x5a42d4`'s implementation of the same address → `cls_0x5a42d4` =
   `TTextPane`. Inheritance chain `cls_0x5a4494_TPane` → `cls_0x5a42d4_TTextPane`
   → `cls_0x5a4358_TConsolePane` is therefore established.

**Class identity — the OTHER candidate (REJECTED).** `cls_0x4a22f0` (the
"PERFECT mapping" claim) is **NOT** TConsolePane. Its struct is only **76 bytes
total** (`recon/ghidra/cls_0x4a22f0.cpp:7-15`) with fields ending at `mbr_0x48`
and **no string anchors, no thread machinery, no text buffer pointer, no chained
list, no vtable matching the 32-slot TPane derivative shape**. Its single body
`meth_0x4a22f0(uint param_1, undefined2 param_2)` (`cls_0x4a22f0.cpp:21-136`)
is a **fill/draw helper**: it builds a 22-int `SDrawParam`-shape struct on stack
(`local_58[]` cleared in a 0x16-int loop at `:71-74` — matches the
`FUN_00438d80_BlitEffect_StructInit` 22-int layout, UI_METHOD_MAP §8.1), packs a
color into `local_64` via the same `<<8|<<8|<<8` RGB→BGRA byte order that
`FUN_00429950` uses (`:126`), branches on `mbr_0x10 & 0x1f` for pixel format
(cases `2/4/8/0x10` are matching the BM_BIT depth flags, `revdefs.h:240+`),
then calls `FUN_004ad0d0(local_58, &local_ac)` — a low-level paint dispatcher.
That is a bitmap/surface class method (probably TBitmap::Stamp or similar — a
generic graphics primitive), **not** an editor console pane. Mapping claim
overruled. Recon files agree (no `Console` / `Kill event` / `command processor`
string xref to `0x4a22f0`).

**Class identity — DISTINGUISHING FROM THE IN-GAME GAME-LOG.** The persistent
multi-line in-game overlay with scrollback that user described (transparent,
shows enemy name + health, message feed) is **NOT** this class. It is **TTextBar
at `cls_0x5a5560`**, vtable `0x005a5560`, global instance `0x65c5d0` — already
documented in sibling `docs/ui/forensics/TTextBar_SPEC.md`. Evidence: Wave-2A
identified `cls_0x5a5560` as TTextBar via the `"text bar"` / `"Trouble
initializing text bar"` string anchors at `FUN_0054bf70` + the unique WRITE-ref
to the global at `0x65c5d0`, with the leaf-TPane fingerprint vtable shape.
TTextBar has three TMosaicSurface child buffers (`+0x84`/`+0x88`/`+0x8c`), a
~1104-byte instance, and `texthealthbar`-based DrawHealthBlock; TConsolePane has
none of that. TConsolePane never appears in `recon/ghidra/cls_0x5b4f30_likely_TPlayScreen.cpp`
(grep returns nothing) — it is not in the gameplay screen's pane list.
**TConsolePane is the EDITOR class only.**

**TPlayScreen ctor cross-check.** `cls_0x5b4f30_likely_TPlayScreen.cpp` (the
TPlayer/TPlayScreen recon, 4070 lines) contains **zero** references to
`0x656ce8`, `TConsolePane`, `cls_0x5a4358`, `0x43e370`, `0x43f350`, or
`0x43f490`. The only callers of the TConsolePane global `DAT_00656ce8` in the
codebase are:
- `cls_0x5b93c4.cpp:1077` (TObjectInstance error-creation path → `meth_0x43dbc0`
  to log `"ERROR: Creating object\n"` into the console buffer)
- `cls_0x5a5320_TMapPane.cpp:1181` / `cls_0x5a5320_likely_TCharacter.cpp:1177`
  (Ghidra-flattened: the actual class here is TMapPane / a sibling editor pane
  → `virt_meth_0x43f350` which is TConsolePane's Input/line-push handler).

These are **editor diagnostic paths**, never reached during pure gameplay.

**Recon files read** (all paths relative to repo root):
- `recon/ghidra/cls_0x5a4358_TConsolePane.cpp` (the leaf class — 556 lines, 8
  bodies extracted)
- `recon/ghidra/cls_0x5a4358__vftable_5a4358.cpp` (vtable struct, 32 slots)
- `recon/ghidra/cls_0x5a42d4.cpp` (TTextPane base — 309 lines, including
  TTextPane::Initialize at `0x43da00` and the keypress dispatch at `0x43dfa0`)
- `recon/ghidra/cls_0x5a4494.cpp` (TPane base — first 120 lines confirm field
  layout `+0x4=x +0x8=y +0xc=w +0x10=h +0x14=newx +0x18=newy +0x1c=newwidth
  +0x20=newheight` from `virt_meth_0x491900_TPane_Initialize` at `:308-332`)
- `recon/ghidra/cls_0x5a68d8.cpp:1635-1690` (`meth_0x4be110` — the multi-line
  text composite that TTextPane DrawBackground calls)
- `recon/ghidra/cls_0x4a22f0.cpp` (the REJECTED candidate, read for elimination)
- `recon/ghidra/cls_0x5b4f30_likely_TPlayScreen.cpp` (verified no TConsolePane
  references)
- `recon/ghidra/_data.txt:71955-72008` (string xrefs `"offset: (%d, %d, %d)
  size: (%d, %d, %d)"` and `"(%d, %d)"`)
- `recon/discovered/renames/agent_ui_textbar.txt` (rename evidence)
- `recon/discovered/port_status/TTextBar.md` (sibling note: cls_0x5a4358 is
  TConsolePane, NOT TTextBar)
- `docs/ui/briefs/B_r4_textbar_assessment.md` (Wave-1C brief — full identity
  chain)
- `docs/ui/forensics/TTextBar_SPEC.md:1-60` (sibling spec — established that the
  in-game game-log lives at cls_0x5a5560, not cls_0x5a4358)
- `attic/src/editor.cpp:528-1100` (pre-release `TConsolePane::Initialize`,
  `ChainMouse`, `KeyPress`, `CharPress`, `MouseMove`, `MouseClick`,
  `DrawBackground`, `Input`, `Output`, `GetChar`, `GetLine`, `CommandThread`)
- `attic/src/editor.h:42-216` (TTextPane + TConsolePane class definitions)
- `src/revdefs.h:200-208` (`CONSOLEX=4 CONSOLEY=125 CONSOLEWIDTH=381
  CONSOLEHEIGHT=124 TEXTBUFSIZE=10000`)

Prior briefs consulted: `B_r3_playscreen_panes.md` (where `FUN_0054bf70` ↔
`"text bar"` was first anchored), `B_r4_textbar_assessment.md` (the identity
reversal — primary source).

**Status:** `forensics-partial`. Sufficient for reconstruction of the
EDITOR-mode console behavior, but several leaf vtable slot bodies are not in the
recon dump and would need a Ghidra extract to pin exactly (see §14):

- `virt_meth_0x43e440` (slot 1) — almost certainly `Close` per attic, body not
  extracted.
- `virt_meth_0x43f150` (slot 20) — pre-release pattern says TTextPane has a
  DrawBackground at this slot AND TConsolePane overrides it (since the
  TConsolePane vtable lists a different addr than the TTextPane vtable at the
  same offset — TConsolePane's `0x43f150` is a different body than TTextPane's
  `0x43db00`). Body not extracted.
- `virt_meth_0x43f160` (slot 21) — has xref to string `"(%d, %d)"` at
  `0x43f29c` (`_data.txt:72007`), matching attic `TConsolePane::DrawBackground`
  line 901 `sprintf(buf, "(%d, %d)", cx, cy)`. So slot 21 is the chained-mouse
  paint extension. Body not extracted (only call sites visible).
- `virt_meth_0x43e920` (slot 27) — has xref to `"offset: (%d, %d, %d)  size:
  (%d, %d, %d)"` at `0x0043eb61`, matching attic `TConsolePane::KeyPress`
  line 806. So slot 27 = `KeyPress`. Body not extracted.
- `virt_meth_0x43edc0` (slot 26) — almost certainly `MouseMove` (attic
  `TConsolePane::MouseMove` updates cx/cy from cursor pos). Body not extracted.
- `virt_meth_0x43ed90` (slot 28) — likely `CharPress` (small wrapper). Body
  not extracted.
- `virt_meth_0x43ee10` (slot 25) — has many string xrefs (`DAT_005cefe4`,
  `005cefe8`, `005cefec`, `005ceff0`) — likely `MouseClick` (closes/aborts
  chained mouse, emits prompt). Body not extracted.

The §3 frame table, §4 layout, and §6 algorithms are pinned from the bodies
that ARE extracted (Initialize / TTextPane base DrawBackground / the four
buffer-manipulation helpers `meth_0x43dbc0/0x43dca0/0x43dda0/0x43de60` + Input
and Output `0x43f350`/`0x43f490` + ChainMouse `0x43e510`). The §8 text-render
parameters are pinned from the extracted `meth_0x4be110` call site in TTextPane
base DrawBackground (`virt_meth_0x43db30`).

**Errors to close (if porting from any earlier scaffold):**
- Do NOT port `cls_0x5a4358` as the in-game game-log overlay. That is TTextBar
  at `cls_0x5a5560`. See sibling `TTextBar_SPEC.md`.
- Do NOT use the (REJECTED) `cls_0x4a22f0` "PERFECT" mapping — that class is a
  graphics-helper, not TConsolePane.
- Do NOT reproduce the Win32 thread or the kill/character `CreateEventA` event
  pair literally. The pre-release scaffold already wraps them in
  `#if 0 // TODO(port): Subsystem 4 — threading` (`attic/src/editor.cpp:543-560,
  568-576, 822-825, 970-972, 1004-1017, 1054-1097`). The port replaces the
  thread + 2 events with a non-blocking command queue per
  `[[feedback-threading]]` (no raw `std::thread`s).

**Snapshot note (supplementary — superseded by retail).** A pre-release
`TConsolePane` implementation exists at `attic/src/editor.cpp:528-1100` +
declaration `attic/src/editor.h:160-216`. Retail and pre-release behavior agree
**at the algorithm level** (text buffer + cursor + chained mouse + thread). The
pre-release pane is constructed with `TTextPane(CONSOLEX=4, CONSOLEY=125,
CONSOLEWIDTH=381, CONSOLEHEIGHT=124)` per `src/revdefs.h:200-205` — these are
**superseded by the retail values `(x,y,w,h) = (0, 0x164=356, 0x168=360,
0x7d=125)` set in the retail ctor `cls_0x5a4358.cpp:535-552`**. The retail
console covers the bottom band of a 640×480 (or 640×481 — y=356 + h=125 = 481)
editor display. **Use the retail values in §3-§4, not the pre-release defines.**

---

## §1 — Overview

`TConsolePane` is the **editor command line**. It does:

1. Holds a 10000-byte text buffer (`mbr_0x60`, the `text` ptr) plus a parallel
   10000-byte undo/scratch buffer (`mbr_0x64`). Both initialized at `'{' + null`
   (`'{'` = `0x7b` is a color-escape sentinel handled specially throughout the
   buffer-shifting code — see §6).
2. Draws as **black-background + plain text** — no chrome bitmap, no shadow on
   text (the TTextPane base DrawBackground passes `param_11=1` =
   left-align-only, no `0x100` shadow-build bit, to `meth_0x4be110` — see §8).
   The selected font is the **editor monospace bitmap font**, current global
   font index `DAT_0065bc40`.
3. Supports a **paged scrolling viewport** into the text buffer (cursor pos
   `mbr_0x70`, window-offset `mbr_0x68`, window width `mbr_0x8c =
   width/(fontW+2)`, window height `mbr_0x88 = height/(fontH+2)` — both
   computed at Initialize from the pane's `mbr_0x10` and the active font).
4. Handles **arrow keys / Home / End / PgUp / PgDn** + character entry via the
   TTextPane base dispatch `virt_meth_0x43dfa0` (already extracted). Extended
   behavior: the chained-mouse system at `mbr_0xa0/0xa4/0xa8` (a singly-linked
   list of `SChained` records holding pointer-callbacks to update a value as
   the mouse moves over a rect — used for live "drag-this-number" UI in scroll
   editing).
5. Has a **dedicated background thread** (`mbr_0xc8`) that runs
   `LAB_0043f5a0` (= `TConsolePane::CommandThread`) and blocks on a
   `WaitForMultipleObjects` of `KillEvent` (`DAT_00656b20`) +
   `CharAvailableEvent` (`DAT_00656b24`). The thread is the consumer of the
   GetLine API — when the user hits Return, the main thread fills `cmdline`
   (`DAT_00656db4`) and `SetEvent(CharAvailableEvent)` (see `virt_meth_0x43f350`
   at `cls_0x5a4358_TConsolePane.cpp:504-505`).

**Visibility.** The pane is **only displayed while the EXILE editor is
running**. It is allocated/wired by the editor's startup
(`FUN_0041ee50:0041ee79` is the global ctor — `_data.txt:127004`). The shipped
game does NOT show this pane — it has no path through TPlayScreen
(`cls_0x5b4f30_likely_TPlayScreen.cpp` contains no reference to the global).
For the Revisited port targeting GOG-shipped retail, this pane is **not part
of the player-visible HUD** — the reconstruction is informational / completeness
only, gated to editor builds.

---

## §2 — Asset roster (table)

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| (none — chrome) | — | — | — | The console has NO chrome bitmap. Background is a solid black fill (color `0`, `(*+0x64)` Box from `cls_0x5a42d4.cpp:119-120`). | — | `cls_0x5a42d4.cpp:119-120` |
| editor bitmap font | (in-binary, GDI HDC) | font index = `DAT_0065bc40` (a global selector) — likely the editor's small fixed-pitch font (a member of the `DAT_0065b020` HDC table; see UI_METHOD_MAP §5) | font width + height read from `*(int*)(hdc+0x50)` and `*(int*)(hdc+0x54)` in `meth_0x4be110` — unknown until the HDC table is decoded (UNCONFIRMED-1) | the text glyphs | none (HDC glyph render) | `cls_0x5a42d4.cpp:42-44, 57-59`; `cls_0x5a68d8.cpp:1662-1663` |
| `lrarrow`, `udarrow` (mouse-chain cursors) | EditorData (editor-only archive) | `"lrarrow"`, `"udarrow"` | unknown until measured | mouse-cursor swap for horizontal/vertical chained-mouse drag modes | full sprite | `attic/src/editor.cpp:585-588` (pre-release; retail equivalent: `meth_0x43e510` line `:428-438` calls `cls_0x46d6b0::meth_0x46d710(DAT_00656ce4)` — the editor pointer/cursor handle table indexed by `DAT_00656ce4`) |

**Asset section is intentionally minimal.** TConsolePane draws as text + box-
fill only — no bitmaps, no atlases. The "chained mouse cursor swap" is the only
asset path, and it uses the editor's pointer-handle table (`DAT_00656ce4`), not
data-archive bitmaps. There is nothing to dump with `tools/ui/dump_dat.py`
because there is no `.dat`-resident chrome.

---

## §3 — Coordinate frames & surfaces

### Pane position + size (retail values, DECISIVE)

Set in the leaf ctor `cls_0x5a4358_TConsolePane::cls_0x5a4358_TConsolePane`
(at `0x00445100`, `recon/ghidra/cls_0x5a4358_TConsolePane.cpp:535-552`):

| field (offset) | value (hex) | value (dec) | TPane semantics |
|---|---|---|---|
| `mbr_0x4` / `mbr_0x14` | `0` | 0 | `x` / `newx` — screen X |
| `mbr_0x8` / `mbr_0x18` | `0x164` | 356 | `y` / `newy` — screen Y |
| `mbr_0xc` / `mbr_0x1c` | `0x168` | 360 | `width` / `newwidth` |
| `mbr_0x10` / `mbr_0x20` | `0x7d` | 125 | `height` / `newheight` |

Pane rect = `screen:(0,356)..(360,481)`. TPane base `Initialize` at
`cls_0x5a4494.cpp:308-332` copies `mbr_0x14/0x18/0x1c/0x20` into
`mbr_0x4/0x8/0xc/0x10` and sets `mbr_0x40=1` (isopen) — the canonical
"finalize geometry" pattern (NOMENCLATURE §1).

> **Anchor:** TL (top-left) — both x and y are bare constants (no `pane_w - k`
> or `parent_h - k` form). The pane is **anchored to the bottom strip of a
> 640×481 editor display**. This differs from the pre-release `CONSOLEX=4,
> CONSOLEY=125, CONSOLEWIDTH=381, CONSOLEHEIGHT=124` of `src/revdefs.h:200-205`
> — retail moved the pane down and slightly narrowed it. **Use retail values.**

### Live width (`this+0xc`)

The runtime pane width field is `mbr_0xc` (= 360 for this pane). The base
TTextPane Initialize at `cls_0x5a42d4.cpp:57-58` uses `mbr_0x10` (height = 125)
not width to compute window-rows: `dVar1 = (int)this->mbr_0x10 / (iVar3 + 2);
this->mbr_0x94 = dVar1; this->mbr_0x88 = dVar1;`. So `mbr_0x88` = `mbr_0x94` =
`height / (font_h + 2)` = lines that fit in the viewport. With `mbr_0x10 = 125`
and a (say) 12-px font: `winheight = 125/14 ≈ 8` rows. (UNCONFIRMED-1: exact
font height depends on which HDC the global `DAT_0065bc40` selects.)

**Note on `mbr_0x84` and `mbr_0x98`.** Both set to `0x3c = 60` in TTextPane
Initialize. These are the pre-release `curheight` (defaulted 1 in attic
`editor.cpp:536`) and `wrapwidth` (set to `GetWidth()` in attic
`editor.cpp:541`). Retail uses constant `0x3c = 60` — likely a **default
column count** that gets corrected by the `(*+0x44)(...)` slot called at
`cls_0x5a42d4.cpp:60` (which is TPane's "geometry-changed" hook
`virt_meth_0x444fc0_44` — UI_METHOD_MAP §15a).

### Frame table (MANDATORY — §3 rule)

The console renders into the **display surface directly** — no intermediate
chrome/mosaic/scratch surface. The only frames are screen, pane-local, and the
display-local frame used by `meth_0x4be110` text composite.

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | (root) | — | `(0,0)` framebuffer top-left | global pixel space; `Display.Width()=640` per editor convention | `attic/src/editor.cpp:1933, 1939-1940` (uses `Display.Width()`) |
| **pane-local** | screen | TL | screen `(0, 0x164) = (0, 356)` | the pane's own rect; subsequent drawing args (0,0) and (0,3) are in this space | `cls_0x5a4358_TConsolePane.cpp:540-541, 544` (`mbr_0x4=0`, `mbr_0x8=0x164`); TPane Initialize `cls_0x5a4494.cpp:312` (`mbr_0x4 = mbr_0x14`) |
| **box-local** (background fill) | pane-local | TL | pane-local `(0, 0)` | the `(*+0x64)` Box call clears the pane rect to black at pane-local `(0,0)` with size `(mbr_0xc, mbr_0x10) = (360, 125)` | `cls_0x5a42d4.cpp:119-120` (`(*+100)(param_1, 0,0, this->mbr_0xc, this->mbr_0x10, 0, 0xff, 0)`) |
| **text-block-local** (text composite) | pane-local | TL | pane-local `(0, 3)` | text drawn starting at pane-local Y=3 (3 px top inset). Lines stack down at `(font_h+2)` per row (cite TTextPane DrawBackground: `dstY=3` literal at `cls_0x5a42d4.cpp:122`). | `cls_0x5a42d4.cpp:121-123` (the `meth_0x4be110(param_1, text_ptr, 0, 3, mbr_0x94, ...)`) |
| **buffer-offset** (logical) | — | — | offsets are byte indices into the 10000-byte `text` buffer at `mbr_0x60`; `mbr_0x68 = window-offset` (start of viewport), `mbr_0x70 = cursor-offset`. Not a screen frame — purely buffer addressing. | `cls_0x5a42d4.cpp:62-64`; `cls_0x5a4358_TConsolePane.cpp:64-66, 86-92` |

### Composition formula

For any element drawn by TTextPane / TConsolePane:

```
screen_x = pane_x + element_x_in_pane
screen_y = pane_y + element_y_in_pane
       = 0 + element_x_in_pane
       = 356 + element_y_in_pane
```

The Box-fill background is `pane-local (0,0) → screen (0, 356)`. The text
composite starts at `pane-local (0, 3) → screen (0, 359)`. Lines stack down
by `(font_h+2)` (the literal `(iVar1 + iVar2) * param_4` at `cls_0x5a68d8.cpp:1687`,
where `iVar1 = *(int*)(hdc+0x50)` (font width) and `iVar2 = *(int*)(hdc+0x54)`
(font height) — actually that's `(font_w + font_h) * lineCount`, see §6 for
why this is fishy — UNCONFIRMED-2).

### The scratch / mosaic surfaces: **NONE**

TConsolePane allocates **no** scratch / mosaic / chrome surfaces. Both
extracted parent calls in TTextPane DrawBackground pass `PTR_DAT_005d79e0`
(the display) directly as the draw target. This is the **direct-renderer
contract** (NOMENCLATURE §3) — composes straight to the swapchain. The port
should therefore implement the pane as a single composed render-target draw
(no intermediate surface), per `[[feedback-centralized-rendering]]`.

### Layout diagram

```
screen y
   0  ┌──────────────────────────── 640 ─────────────────────────────┐
      │                                                              │
      │    (editor: map pane, status bar, class pane, etc.)          │
      │                                                              │
      │                                                              │
 356  ├──────────── 360 ────────────┐ ......... 640                  │  ← pane-local origin
      │ ▒                            │   (gap)                       │
      │ ▒  (text body starts y=359  │                                │
      │ ▒   = pane-local y=3)       │                                │
      │ ▒  > prompt% _              │                                │
      │ ▒  > some output            │                                │
      │ ▒  > more output            │                                │
      │ ▒                           │                                │
      │ ▒                           │                                │
 481  └─────────────────────────────┘                                │
      │                                                              │
      └──────────────────────────────────────────────────────────────┘

  ▒ = solid black background (Box-fill color=0)
  text = editor monospace bitmap font, color=mbr_0x9c=0xffffffff (-1 = white default)
  lines stack DOWN from pane-local (0,3) by (font_h + 2) per row
```

> **Note: the right edge of the console (x=360) leaves a 280-px gap to the
> 640-px display edge.** The pre-release `EDMULTIPANEX` and `ClassPane` filled
> that gap with the toolbar + editor class pane (`attic/src/editor.h:336-338`).
> Retail editor likely does the same; this is **outside TConsolePane's
> responsibility** and not covered by this spec.

---

## §4 — Static element layout (table)

The console has only TWO drawn elements: the background fill and the text
block. There is no portrait, ring, icon, bar, or chrome.

| element | space | (x, y) | (w, h) | source rect | cite |
|---|---|---|---|---|---|
| **black background** | pane-local | `(0, 0)` → screen `(0, 356)` | `(360, 125)` | n/a (filled) | `cls_0x5a42d4.cpp:119-120` — `(*+100)(disp, 0,0, mbr_0xc, mbr_0x10, color=0, zpos=0xff, normal=0)` |
| **text body (line stack)** | pane-local | `(0, 3)` → screen `(0, 359)` | (`360`, `125-3=122` available) | n/a (HDC glyph composite) | `cls_0x5a42d4.cpp:121-123` — `meth_0x4be110(disp, mbr_0x68+mbr_0x60, 0, 3, mbr_0x94, DAT_0065bc40, 0, 0x80000000, mbr_0x9c, 0, 1, 0xffffffff)` |
| **cursor caret** (the `'{'` sentinel placed at `text[curoffset]`) | text-block-local | column = `mbr_0x74`, row = `mbr_0x78` (cursor x,y in chars) | font glyph cell | rendered as a `'{'` char — TTextPane buffer-update routines insert `0x7b` at the cursor offset (`cls_0x5a4358_TConsolePane.cpp:92, 315`); the multi-line text composite renders it inline (no special caret pass — the `'{'` IS the visible caret) | `cls_0x5a4358_TConsolePane.cpp:92` `*(undefined *)(dVar4 + this->mbr_0x60) = 0x7b;` and `:315`; cursor field `mbr_0x74` = `cursorx`, `mbr_0x78` = `cursory` (per `editor.h:82`) |

**There is no "mirrored side" or "target chip"** — the editor console is a
single instance, not a player/target pair. The mirror columns in the
NOMENCLATURE §4 anchor rules do not apply.

---

## §5 — Draw order / composition

Per-frame, the editor screen invokes TConsolePane's vtable. The relevant slots
are slot 20 (offset 80, `virt_meth_0x43f150`) — the DrawBackground entry — and
slot 21 (offset 84, `virt_meth_0x43f160`) — the chained-mouse paint extension.
Slot 21's role is confirmed by the `_data.txt:72007` xref of string `"(%d, %d)"`
to `0x43f29c` (inside `virt_meth_0x43f160`), matching attic
`editor.cpp:901` `sprintf(buf, "(%d, %d)", cx, cy)`.

**For the simple no-chained-mouse case (the common path), TTextPane base
DrawBackground at `virt_meth_0x43db00`/`virt_meth_0x43db30` is the entire body
of the draw.** TConsolePane's slot 20 (`0x43f150`) is a small wrapper that
likely just calls the base then overlays the chained-mouse text — see §6.

Exact sequence (from `cls_0x5a42d4.cpp:101-125` — the extracted TTextPane base
DrawBackground):

1. **Guard.** `if (mbr_0x50 != 0) { ... }` — `mbr_0x50` is a "needs-draw"
   flag (likely the dirty bit). When 0, skip.
2. **Box-fill.** `(*+0x64)(disp, 0,0, mbr_0xc, mbr_0x10, color=0, zpos=0xff,
   normal=0)` → clears the pane rect to **solid black** (color = 0). This is
   `TSurface::Box` (UI_METHOD_MAP §15a, slot `+0x64`). The destination surface
   is `PTR_DAT_005d79e0` (the display) — i.e. the (0,0) is **pane-local**,
   resolved to screen via the pane's stored origin in the display's blit setup
   (the display's clip/origin is set by the pane's own `Activate`-style hook
   `virt_meth_0x444fc0_44`, called separately at `:60` and `:106`).
3. **Text composite.** `meth_0x4be110(disp, mbr_0x68 + mbr_0x60, 0, 3, mbr_0x94,
   DAT_0065bc40, 0, 0x80000000, mbr_0x9c, 0, 1, 0xffffffff)`. This is the
   multi-line text wrapper (UI_METHOD_MAP §5). Args:
   - `dstSurface` = display
   - `text` = `mbr_0x60 + mbr_0x68` (text buffer + window-offset into buffer)
   - `dstX` = 0, `dstY` = 3 (pane-local; 3-px top inset)
   - `lineCount` = `mbr_0x94` (= height/(fontH+2), the visible-rows count)
   - `fontIdx` = `DAT_0065bc40` (current font global)
   - `wrap-arg` = 0
   - `drawmode` = `0x80000000` (`DM_USEDEFAULT` = opaque)
   - `color` = `mbr_0x9c` (= `0xffffffff` = -1 = "default" → resolves to font
     default white, see §8)
   - 0, 1, `0xffffffff` (format flags: `1` = left-align; no `0x100` shadow bit)
   Internally `meth_0x4be110` loops once per line, calling
   `meth_0x4be2b0_CompositeBuffer` (the GDI `DrawTextA` text renderer,
   UI_METHOD_MAP §5) per line. **No 3-pass shadow** (the shadow bit `0x400` on
   the font index isn't set — `DAT_0065bc40` is the bare editor-font index,
   not OR'd with `0x400`. UNCONFIRMED-3 — see §14.)
4. **`(*+0x44)` finalize.** Called at end of `virt_meth_0x43db00` (line 106).
   This is the surface "unlock / commit" hook (UI_METHOD_MAP §15a slot `+0x44`).

After step 4, if `mbr_0xa0` (the active chained-mouse `chained` ptr) is
non-null, the TConsolePane slot-21 extension `0x43f160` overlays the
chained-mouse text — body not extracted but the algorithm (from attic
`editor.cpp:890-920` `TConsolePane::DrawBackground`):

5. **Chained-mouse overlay (only if `mbr_0xa0 != 0` AND mouse moved).** Read
   current cursor `mbr_0xac` (cx) / `mbr_0xb0` (cy), compare to last
   `mbr_0xb4` / `mbr_0xb8`. If changed:
   - `meth_0x43dca0(this, oldbuflen)` to remove last printed string at cursor
     (the "RemoveText" path).
   - If `chained->xspan != 0`:
     - If `chained->yspan != 0` → `sprintf(buf, "(%d, %d)", cx, cy)` (the
       `"(%d, %d)"` string at `0x005ceff4`)
     - Else → `itos(cx, buf, ...)` (single integer)
     - Invoke `(*chained->func)(cx, cy)` to update the callback's target value.
   - Else (yspan only) → `itos(cy, buf, ...)`, invoke `(*chained->func)(cy, cx)`.
   - `meth_0x43dbc0(this, ...buf...)` to insert the formatted string at cursor.
   - Update `mbr_0xbc` (= oldbuflen), `mbr_0xb4`, `mbr_0xb8`.

**Single-pass; no atlas / no mosaic / no chrome / no shadow / no chroma key.**
The console is the simplest drawable pane in the codebase.

---

## §6 — Algorithms (pseudocode per helper)

### Helper 1 — `meth_0x43dbc0` (insert text at cursor)

**Address:** `0x0043dbc0`. **Sig (recon, line 51):**
`int meth_0x43dbc0(this, char* in_str /* via in_stack_00000004 */)`.

Inserts the C-string `in_str` into the text buffer at the current cursor
position (`mbr_0x70`), shifting tail right, updating cursor x (`mbr_0x74`) and
cursor y (`mbr_0x78`), then calls `meth_0x43dda0` (window-clip update) and
`(*vtable+0xa8)(this)` = `virt_meth_0x444fb0` (slot 10, the "made-dirty" hook).

Pseudocode:

```
int InsertText(this, in_str):
    text_len = strlen(this.text)         # text = mbr_0x60
    in_len  = strlen(in_str)
    this.textlen = text_len              # mbr_0x6c
    if in_len == 0: return 1
    if text_len + in_len > 9999: return 0   # buffer full guard, cls_0x5a4358_TConsolePane.cpp:83
    # shift [curoffset+1..end] right by in_len
    cursor = this.curoffset              # mbr_0x70
    memmove(text + cursor + 1 + in_len,
            text + cursor + 1,
            text_len - cursor)
    # copy in_str into buffer at curoffset
    memmove(text + cursor, in_str, in_len)
    cursor += in_len
    this.curoffset = cursor
    text[cursor] = '{'                  # cursor sentinel char (=0x7b)
    # count newlines in in_str; each one increments cursory and resets cursorx
    for each '\n' in in_str:
        in_str = past_that_newline
        this.cursorx = 0                 # mbr_0x74
        this.cursory += 1                # mbr_0x78
    # add length of last (post-final-\n) fragment to cursorx
    this.cursorx += strlen(in_str_after_last_newline)
    this.textlen += in_len
    UpdateWindow(this)                   # meth_0x43dda0
    (*vt+0xa8)(this)                     # virt_meth_0x444fb0 = "mark dirty"
    return 1
```

### Helper 2 — `meth_0x43dca0` (remove characters at cursor, with/without forward direction)

**Address:** `0x0043dca0`. **Sig (recon, line 119):**
`int meth_0x43dca0(this, int numchars, int forward /* via in_stack_00000008 */)`.

Removes `numchars` bytes from the buffer in direction `forward` (0 = backward
= backspace, !=0 = forward = delete).

Pseudocode:

```
int RemoveText(this, numchars, forward):
    text_len = strlen(this.text)
    this.textlen = text_len
    if forward == 0:                            # backspace
        # clamp numchars to amount available before cursor
        if numchars > this.cursorx - this.curstartx:
            numchars = this.cursorx - this.curstartx       # mbr_0x74 - mbr_0x7c
    if numchars < 0: return 0
    if text_len - numchars < 1:
        numchars = text_len - 1
    if forward == 0:
        # shift left by numchars, starting at curoffset
        memmove(text + this.curoffset - numchars,
                text + this.curoffset,
                text_len - this.curoffset + 1)
        this.curoffset -= numchars
        this.cursorx   -= numchars
    else:
        # forward: shift everything past cursor+1 left by numchars
        memmove(text + this.curoffset + 1,
                text + this.curoffset + 1 + numchars,
                text_len - this.curoffset - numchars + 1)
    return 1
```

### Helper 3 — `meth_0x43dda0` (UpdateWindow — clamp visible-window to cursor)

**Address:** `0x0043dda0`. **Sig:** `void meth_0x43dda0(this)`.

Pages the visible window (`mbr_0x68` = window-offset, `mbr_0x90` = windowy)
so that the cursor row (`mbr_0x78`) is on-screen. This is the autoscroll.

Pseudocode:

```
void UpdateWindow(this):
    while True:
        if this.windowy <= this.cursory:        # mbr_0x90 <= mbr_0x78
            if this.windowy + this.winheight <= this.cursory:  # mbr_0x94
                # cursor is BELOW visible window — scroll DOWN
                while this.windowy + this.winheight <= this.cursory:
                    # advance window-offset by one line in the buffer
                    p = this.window_offset_in_buf + this.text   # mbr_0x68 + mbr_0x60
                    iVar5 = 0
                    while *p != 0 and *p != '\n':
                        if *p != '{':   # color-escape sentinel
                            iVar5 += 1
                        p += 1
                    this.window_offset_in_buf += iVar5
                    if text[ this.window_offset_in_buf ] != 0:
                        this.window_offset_in_buf += 1
                    this.windowy += 1
            return
        # cursor is ABOVE visible window — scroll UP one line in the buffer
        if this.window_offset_in_buf > 0:
            # walk window_offset_in_buf back to previous newline
            while True:
                this.window_offset_in_buf -= 1
                if this.window_offset_in_buf < 0: break
                if text[ this.window_offset_in_buf ] == '\n': break
            if this.window_offset_in_buf > 0:
                # back one more for the newline before that one
                while True:
                    if text[ this.window_offset_in_buf - 1 ] == '\n': break
                    this.window_offset_in_buf -= 1
                    if this.window_offset_in_buf < 0: break
        this.windowy -= 1
```

**`'{'` semantics.** Through ALL buffer iteration loops, the byte `0x7b = '{'`
is treated specially: it does NOT count toward visible character width
(`if *p != '{': iVar5 += 1`). This is because `'{'` is the **color-escape
sentinel** — TTextPane's color-escape lexer consumes a `'{'` followed by some
hex digits and a `'}'` as a color-change marker, and renders the inside as a
control sequence, not as visible glyphs. The exact format is not exposed in
the extracted bodies (UNCONFIRMED-4); the implementor should preserve the
**don't-count-`{`** loop semantics until the lexer is reverse-engineered.

### Helper 4 — `meth_0x43de60` (SetCursor)

**Address:** `0x0043de60`. **Sig (recon, line 229):**
`int meth_0x43de60(this, int new_cursorx, int new_cursory, int ignorebounds /* via in_stack_0000000c */)`.

Moves cursor to `(new_cursorx, new_cursory)`. Returns 0 if cursor unchanged, 1 if moved.

Pseudocode (key body, `cls_0x5a4358_TConsolePane.cpp:230-323`):

```
int SetCursor(this, nx, ny, ignorebounds):
    if not ignorebounds:
        if nx < this.curstartx: nx = this.curstartx     # clamp to prompt boundary
        # clamp ny to within [curstarty, curstarty + winheight - 1]
        if this.curstarty <= ny:                        # mbr_0x80
            top = this.curstarty + this.winheight       # mbr_0x88
            if top <= ny:
                ny = top - 1
            else:
                ny = ny
        else:
            ny = ny
    if nx == this.cursorx and ny == this.cursory:       # no change
        return 0
    # erase '{' caret at old position by shifting tail left by 1
    p = this.text + this.curoffset
    memmove(p, p+1, strlen(p+1))
    # walk to position (nx, ny) from buffer start
    this.curoffset = 0
    # skip ny lines (newline-bounded), respecting '{' as non-counting
    while line_count < ny:
        for cur_x in range(...):
            ... advance, counting non-'{' as a step
            if hit '\n': line_count += 1; break
    # advance by nx columns on that line (counting non-'{')
    while col_count < nx and not '\n' and not '\0':
        this.curoffset += 1
        col_count += 1
    # shift the right side of buffer right by 1, then drop '{' caret at curoffset
    p = this.text + this.curoffset
    memmove(p+1, p, strlen(text) - this.curoffset)
    text[this.curoffset] = '{'                          # 0x7b
    if this.curoffset < this.window_offset_in_buf:
        this.window_offset_in_buf += 1                  # follow the shift
    this.cursorx = col_count                            # mbr_0x74
    this.cursory = ny                                   # mbr_0x78
    UpdateWindow(this)
    return 1
```

### Helper 5 — `virt_meth_0x43f350` (Input / line-feed dispatch)

**Address:** `0x0043f350` (vtable slot 31, offset 124).

The user-typed-Enter handler. Takes a buffer containing newline-separated lines,
inserts each as a typed command (via `meth_0x43dbc0`), advances the cursor to
the next line, and **fires the `CharAvailableEvent`** (DAT_00656b24) to wake
the command-processor thread.

Pseudocode (from `cls_0x5a4358_TConsolePane.cpp:462-510`):

```
int Input(this, char* in_str):
    p = strchr(in_str, '\n')
    while p != null:
        # copy this line into a 256-byte local stack buffer
        n = 0
        for ch in in_str:
            if ch == '\n' or n >= 255: break
            local_100[n] = ch; n += 1
        local_100[n] = 0
        # walk to the END of the current cursor line (count non-'{')
        line_end_col = 0
        scan = this.text + this.curoffset
        while *scan and *scan != '\n':
            if *scan != '{': line_end_col += 1
            scan += 1
        # move cursor there, insert the line, then insert "\n",
        # then jump cursor to position (curstartx, cursory-1, ignorebounds=true)
        SetCursor(this, this.cursorx + line_end_col, this.cursory)
        InsertText(this, local_100)      # the user-line gets appended
        InsertText(this, "\n")
        SetCursor(this, this.curstartx, this.cursory - 1)
        SetCursor(this, 0, this.cursory + 1)
        # advance past the newline in the input str
        in_str = p + (*p ? 1 : 0)
        # === WAKE THE COMMAND THREAD ===
        DAT_006581b8 = 0xd                              # = '\r' (signal to thread)
        SetEvent(DAT_00656b24)                          # CharAvailableEvent
        DAT_00656db4 = this.text + this.curoffset       # cmdline ptr
        p = strchr(in_str, '\n')
    InsertText(this, in_str)                            # tail (no trailing \n)
    return 1
```

`DAT_006581b8` and `DAT_00656db4` are the **shared `cmdchar` + `cmdline`
statics** from `attic/src/editor.cpp:55-57` (the TConsolePane statics). The
thread loop reads them after `WaitForMultipleObjects`.

### Helper 6 — `virt_meth_0x43f490` (Output)

**Address:** `0x0043f490` (vtable slot 32, offset 128).

The non-blocking write path. Counts newlines in `in_str`, bumps cursor y, then
`InsertText` — but **does NOT fire the CharAvailableEvent** (so the command
thread is not woken). This is the path used for printf-style logging into the
buffer (e.g. the TObjectInstance error path at `cls_0x5b93c4.cpp:1077`).

Pseudocode (from `cls_0x5a4358_TConsolePane.cpp:516-529`):

```
void Output(this, char* in_str):
    for p = strchr(in_str, '\n'); p != null; p = strchr(p+1, '\n'):
        this.curstarty += 1                             # mbr_0x80
    InsertText(this, in_str)                            # meth_0x43dbc0
    DAT_00656db4 = this.text + this.curoffset           # cmdline = end of buffer
```

### Helper 7 — `meth_0x43e510` (ChainMouse — set up a chained-mouse interaction)

**Address:** `0x0043e510`. **Sig (recon, line 374):**
`void meth_0x43e510(this, void(*cfunc)(int,int) /*param_1*/, int x0 /*param_2*/, int x1 /*param_3*/, int xcur /*param_4*/, int y0 /*param_5*/, int y1 /*param_6*/, int ycur /* via in_stack_0000001c */)`.

Builds an `SChained` record (24 bytes — allocated by `FUN_00482fb0(0x24)`),
fills its fields, links it to the end of the chained list, and on the **first**
record swaps the mouse cursor + calls `PlayScreen.SetExclusivePane(&Console)`.

The `SChained` struct layout (from the field stores at `cls_0x5a4358_TConsolePane.cpp:398-410`
+ attic `editor.h:138-145`):

| `puVar2[idx]` | byte off | semantic |
|---|---|---|
| `[0]` | 0x00 | `func` — `void (*)(int x, int y)` |
| `[1]` | 0x04 | `xmin` (= sorted-min of param_2/param_3) |
| `[2]` | 0x08 | `xspan` (= param_3 − param_2 after sort) |
| `[3]` | 0x0c | `ymin` |
| `[4]` | 0x10 | `yspan` |
| `[5]` | 0x14 | `oldxval` (= param_4) |
| `[6]` | 0x18 | `oldyval` (= in_stack_0000001c) |
| `[7]` | 0x1c | `next` (set to 0 on add) |
| `[8]` | 0x20 | `prev` (= old `mbr_0xa8` tail) |

Pseudocode:

```
void ChainMouse(this, cfunc, x0, x1, xcur, y0, y1, ycur):
    # sort
    if x1 < x0: swap(x0, x1)
    if y1 < y0: swap(y0, y1)
    rec = malloc(0x24)
    rec.func    = cfunc
    rec.xmin    = x0
    rec.xspan   = x1 - x0
    rec.ymin    = y0
    rec.yspan   = y1 - y0
    rec.oldxval = xcur
    rec.oldyval = ycur
    rec.next    = null
    rec.prev    = this.tail                # mbr_0xa8
    if this.tail != null:                  # appending
        this.tail.next = rec
    else:                                  # first record — switch mouse + grab focus
        this.head     = rec
        this.chained  = rec                # mbr_0xa0
        this.oldbuflen = 0
        this.oldcx    = x0 + 1
        this.oldcy    = y0 + 1
        if xspan == 0:
            SetMouseBitmap(EditorData.Bitmap("udarrow"))    # cls_0x46d6b0::meth_0x46d710(DAT_00656ce4)
        elif yspan == 0:
            SetMouseBitmap(EditorData.Bitmap("lrarrow"))
        else:
            SetMouseBitmap(PointerCursor)                   # FUN_0043a020(DAT_0065a28c)
        PlayScreen.SetExclusivePane(&Console)               # TCharacter::meth_0x48ed60 + 48eea0
    this.tail = rec                                         # mbr_0xa8
```

(The `TCharacter::meth_0x48ed60`/`meth_0x48eea0` calls at `:440-441` are the
exclusive-pane-set/get pair on the actual `PlayScreen` instance — Ghidra
flattened TCharacter into the PlayScreen access path.)

---

## §7 — Effects & shadows

**Per-side shadow offsets:** N/A. There is no shadowed bar, no shadowed icon —
the console has no chrome at all.

**Text 3-pass shadow:** **DISABLED.** The TTextPane base DrawBackground calls
`meth_0x4be110` with `param_5 = DAT_0065bc40` (the bare current-font index).
Inside `meth_0x4be2b0_CompositeBuffer`, the 3-pass black shadow is gated on
`param_8 & 0x400` (UI_METHOD_MAP §5, `:377`), and `DAT_0065bc40` does not have
bit `0x400` set (UNCONFIRMED-3 — depends on which font is selected for the
editor; the call site **does not OR in `0x400`** so even if the underlying font
has bit `0x400`, the shadow path requires the gate). The format word
`param_10 = 0` (no `0x100` shadow-build bit) further blocks any path that would
enable a shadow build.

**Chroma key:** N/A. The text is drawn via `DrawTextA` (GDI) — color from
`mbr_0x9c` (= `0xffffffff` = -1, which `meth_0x4be2b0` interprets as the
font's default color, typically white). No bitmap source means no magenta key.

**The pink-halo bug (NOMENCLATURE §4) cannot occur here** — there is no
chroma-key path involved. Don't worry about it.

**The fields written into the box-fill** (`color=0, zpos=0xff, normal=0`):
- color `0` = black (RGB565 `0x0000`)
- zpos `0xff` (= 255) = nominal front Z (the editor renders the console on
  top of the map view; this nudge keeps it above)
- normal `0` = no lighting normal (the editor uses no lighting on UI)

---

## §8 — Text rendering (table)

The console renders **exactly one DrawText call per visible row**, via the
loop inside `meth_0x4be110` → `meth_0x4be2b0_CompositeBuffer`.

| string | cell (space, x, y, w, h) | font | px | color | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| `mbr_0x68 + mbr_0x60` (the visible-window slice of the text buffer) — each row is one line of the scrollback | pane-local; row `i ∈ [0, mbr_0x94)` → `cell = (0, 3 + i * lineHeight, mbr_0xc, lineHeight)` (`mbr_0xc = 360`, `lineHeight = (font_w + font_h) + nothing more` — see UNCONFIRMED-2 below) | font index = `DAT_0065bc40` (global) | UNCONFIRMED — depends on which HDC `DAT_0065bc40` selects | `mbr_0x9c = 0xffffffff` = `-1` → "use font default" (resolves to white per `meth_0x4be2b0:378` color-byte-swap behavior; -1 RGB → BGR is still -1 → GDI takes it as `0x00FFFFFF` = white) | **left** (param_11 = 1 → `&1` set, NOMENCLATURE §2 / UI_METHOD_MAP §5) | **top** (no DT_VCENTER) | **none** (param_8 lacks bit `0x400`, param_10 lacks bit `0x100`) | n/a — raw text buffer (no `printf` format inside the base draw) | the text bytes are the user's typed-and-output history, accumulated in the 10000-byte buffer via `meth_0x43dbc0` (InsertText) | `cls_0x5a42d4.cpp:121-123` (`meth_0x4be110` call); `cls_0x5a68d8.cpp:1655` (font HDC lookup); `cls_0x5a4358_TConsolePane.cpp:540-552` (pane geom); `cls_0x5a42d4.cpp:44, 57-59` (font + line metrics) |
| `"(%d, %d)"` (chained-mouse value display) | pane-local; at the current cursor `cell = (mbr_0x74 * font_w, mbr_0x78 * font_h + 3, ...)` (rough; the buffer-insert path means this string ends up at the cursor offset and is laid out by the next DrawBackground pass) | same | same | same | left | top | none | `"(%d, %d)"` literal at `0x005ceff4` (`_data.txt:72007`) | `cx, cy` = `mbr_0xac, mbr_0xb0` (the chained-mouse current cursor pos) | xref site `0x43f29c` in `virt_meth_0x43f160` (body not extracted; signature inferred from string + attic `editor.cpp:901`) |
| `"offset: (%d, %d, %d)  size: (%d, %d, %d)"` (bounding-box display) | same | same | same | same | left | top | none | literal at `0x005cefb8` (`_data.txt:71980`) | `x, y, z, w, l, h` of the box object (from `editor.cpp:806`) | xref site `0x0043eb61` in `virt_meth_0x43e920` (= KeyPress, body not extracted; matches attic `editor.cpp:806`) |

**Lines stack DOWN from cell top by lineHeight.** This is the NOMENCLATURE §2
default; the multi-line text wrapper iterates lineCount times incrementing
`y += lineHeight` per pass.

**Font flag decoding (param_8 / param_10).** Per UI_METHOD_MAP §5:
- `DAT_0065bc40` low-byte = HDC table index (no shadow bit `0x400` confirmed
  absent in the call site)
- `param_10 = 1` → format flag `&1` = DT_LEFT (left-align horizontal)
- `param_10` does NOT have bit `0x10/0x20/0x40` → DT_TOP vertical (UI_METHOD_MAP
  §5 step 6)
- `param_10` does NOT have bit `0x100` → no shadow build

**Color source override:** the base call site passes `mbr_0x9c` as
`meth_0x4be110`'s `color` arg (which maps to the `meth_0x4be2b0` `param_7`).
`mbr_0x9c` is initialized to `0xffffffff` in TTextPane Initialize
(`cls_0x5a42d4.cpp:56`) — i.e. all bits set. After the GDI BGR byte-swap at
`meth_0x4be2b0:378` (UI_METHOD_MAP §5 step 9), the high byte is dropped and the
remaining `0x00FFFFFF` is the GDI COLORREF for white. So **text color = WHITE**
on **black background**.

---

## §9 — Animation & dynamic behavior

**No animation, no tweens, no ramps.** The console redraws when:

- `mbr_0x50 != 0` (the dirty-flag guard at `cls_0x5a42d4.cpp:104`) AND
- The host editor screen invokes its DrawBackground.

The dirty flag is set by `(*vt+0xa8)(this)` = `virt_meth_0x444fb0` (slot 10),
called at the end of every Insert/Remove/SetCursor that succeeds
(`cls_0x5a4358_TConsolePane.cpp:110, 232, 245, 254, 322`). So the redraw model
is **pure dirty-on-edit**: any text change marks dirty, the next frame redraws
the entire pane (Box-fill + line-stack composite), the surface `(*+0x44)` hook
clears dirty.

**Cache fields.** No explicit cache beyond the dirty bit. Each redraw
re-composites the full visible-window text — there is no per-line cache like
TPlyrStatusBar's stat-value cells.

**State machines / phases:** the chained-mouse state machine —

```
phase chainedMouse:
  IDLE      mbr_0xa0 == null
  ACTIVE    mbr_0xa0 != null
    └ entry:  ChainMouse(...) appends 1st record → switch cursor +
              PlayScreen.SetExclusivePane(this)
    └ in-state on MouseMove:  cx/cy from the chained-rect projection
              (cx = (cursorx * xspan / displayW) + xmin)
    └ on MouseClick LEFTUP:   advance to chained.next; if null, send
              Input("\n") + tear down list + restore cursor + release excl.
    └ on MouseClick RIGHTUP:  for each record, invoke func(oldxval, oldyval) +
              free + tear down + Output("\n%") + restore cursor + release excl.
```

(See attic `editor.cpp:838-888` `MouseClick`. Retail body at
`virt_meth_0x43ee10` (slot 25) is not extracted, but the xref of multiple
strings to it (`DAT_005cefe4`, `005cefe8`, `005cefec`, `005ceff0`) indicates
multiple `Output` calls — consistent with the abort/finalize path.)

**No 24Hz gate.** There is no per-tick animator — the console is event-driven.

---

## §10 — Input & dispatch (THIS PANE IS INTERACTIVE)

**The TConsolePane is a keyboard-driven editor pane.** It handles arrow keys,
home/end, pageup/pagedown via the TTextPane base `virt_meth_0x43dfa0` dispatch
(extracted at `cls_0x5a42d4.cpp:131-260`) — that body switches on `param_1`:

| `param_1` key | retail addr | semantic |
|---|---|---|
| `0x08` (VK_BACK) | `cls_0x5a4358_TConsolePane::meth_0x43dca0(this, 1)` | backspace 1 char |
| `0x21` (VK_PRIOR / PgUp) | `meth_0x43de60(this, cursorx, cursory - winheight)` | page up |
| `0x22` (VK_NEXT / PgDn) | `meth_0x43de60(this, cursorx, cursory + winheight)` | page down |
| `0x23` (VK_END) | `meth_0x43de60(this, cursorx + line_remaining, cursory)` | end of line |
| `0x24` (VK_HOME) | `meth_0x43de60(this, 0, cursory)` | start of line |
| `0x25` (VK_LEFT) | `meth_0x43de60(this, cursorx - 1, cursory)` (or `-6` if `DAT_0065c9e0 != 0` — CtrlDown? UNCONFIRMED-5) | left |
| `0x26` (VK_UP) | `meth_0x43de60(this, cursorx, cursory - 1)` | up |
| `0x27` (VK_RIGHT) | `meth_0x43de60(this, cursorx + 1, cursory)` (or `+6` if `DAT_0065c9e0 != 0`) | right |
| `0x28` (VK_DOWN) | `meth_0x43de60(this, cursorx, cursory + 1)` | down |
| `0x2e` (VK_DELETE) | `meth_0x43dca0(this, 1)` | forward delete (param_3=non-zero in dispatch — but here the body passes 1 — UNCONFIRMED-6) |
| `0x72` (VK_F3?) | "yank" — moves cursor to col 0, copies line, re-inserts at original cursor | (custom shortcut, attic line 2658+) |
| `0x59 / 0x79` (Y / 'y') | with `DAT_0065c9e0 != 0` (CtrlDown): duplicate-line | (custom shortcut) |

For **character entry** (`virt_meth_0x43e2b0`, `cls_0x5a42d4.cpp:266-307`),
TTextPane dispatches to `(*vt+0x4450d0)(this)` = `virt_meth_0x4450d0_124` =
**slot 31 in TConsolePane vtable = `virt_meth_0x43f350` = `Input`**. So every
printable char calls Input, which calls InsertText. The Enter key (`'\r'`) at
`:277-296` converts `\r` → `\n`, fills the parallel `mbr_0x64` undo buffer with
the line, then dispatches Input. **Input fires the CharAvailableEvent**, which
the command-processor thread is waiting on.

**Mouse:** `virt_meth_0x43f160` (slot 21, DrawBackground extension) reads
`cx/cy` updated by `virt_meth_0x43edc0` (slot 26 = MouseMove, body not
extracted). `virt_meth_0x43ee10` (slot 25 = MouseClick, body not extracted)
walks the chained list.

**Modal-state globals.**
- `DAT_006581b8` (`= cmdchar`, `editor.cpp:55`) — the most-recently-typed
  character, broadcast to the command-processor thread.
- `DAT_00656db4` (`= cmdline`, `editor.cpp:57`) — pointer into the text buffer
  at the current command-line start.
- `DAT_00656b20` (`= cmdevents[KILL_EVENT]`) — `CreateEventA` handle for
  shutting down the command thread.
- `DAT_00656b24` (`= cmdevents[CHAR_AVAILABLE_EVENT]`) — `CreateEventA` handle
  the thread waits on.
- `DAT_0065c9e0` — Ctrl-key-down state (UNCONFIRMED-5 — interpretation from
  context in `cls_0x5a42d4.cpp:187, 200, 235, 299`).
- `DAT_0065c9d8` — likely Alt-key-down or another modifier (UNCONFIRMED-5).
- `DAT_0065bc40` — current font index (a global font-selector that other panes
  also read).
- `DAT_00656ce4` — handle into `cls_0x46d6b0`'s resource table (for the lrarrow
  / udarrow / PointerCursor swap in ChainMouse).

**No command-id table** — TConsolePane does not dispatch via `TPlayScreen::
Command` (the cited `0x47cf40` in user instructions). It is keystroke-driven
into its own buffer + sends parsed lines into the global `Console.Input` →
CommandThread, which calls `CommandInterpreter` (attic `editor.cpp:1080`). The
command-id dispatch happens deep inside `CommandInterpreter`, not in this pane.

---

## §11 — Retail bugs NOT to reproduce

**None identified.** The console paint path is simple enough that the standard
retail UI artifacts (pink-halo from chroma-key text shadow, mis-aligned
baselines) don't apply:
- Pink halo requires a magenta colorkey on the rendered text, which requires
  the 3-pass shadow path enabled — both gated bits (`param_8 & 0x400` and
  `param_10 & 0x100`) are **off** here.
- Baseline issues are a font-render-helper concern, not a per-spec concern
  (NOMENCLATURE §2).

The pre-release source paging-on-overflow branch at attic
`editor.cpp:929-942` is **commented out** (`/* ... */`) — so if the user typed
9999 bytes the buffer wraps unsafely. **The port should add a real
ring-buffer paging path** rather than reproducing the broken commented-out
attempt. (This is a pre-release bug present in the snapshot, and the retail
size check at `cls_0x5a4358_TConsolePane.cpp:83` simply returns 0 instead of
overflowing — UI freezes at the 9999-byte boundary. **Don't reproduce: add a
real eviction policy.**)

---

## §12 — Reconstruction pseudocode

```cpp
// EDITOR-ONLY — not built into release game binary.

class TConsolePane : public TTextPane {
    // — TTextPane state (carry over from base):
    //   text       : char[10000]   = mbr_0x60 (heap; init = "{\0")
    //   undo       : char[10000]   = mbr_0x64 (heap; init = "{\0")
    //   window_off : int           = mbr_0x68     // byte offset into text
    //   textlen    : int           = mbr_0x6c
    //   curoffset  : int           = mbr_0x70     // byte offset of cursor in text
    //   cursorx,y  : int,int       = mbr_0x74, mbr_0x78  (in columns/rows)
    //   curstartx,y: int,int       = mbr_0x7c, mbr_0x80
    //   curheight  : int = 0x3c=60 = mbr_0x84     (legacy default)
    //   winheight  : int           = mbr_0x88 = mbr_0x10 / (font_h+2)
    //   winwidth   : int           = mbr_0x8c     (computed by virt_meth_0x444fc0)
    //   windowx,y  : int,int       = mbr_0x90, mbr_0x94
    //   wrapwidth  : int = 0x3c=60 = mbr_0x98
    //   color      : uint32        = mbr_0x9c = 0xFFFFFFFF (-1 = default white)
    // — TConsolePane additions:
    SChained* head      = nullptr;        // mbr_0xa4
    SChained* chained   = nullptr;        // mbr_0xa0
    SChained* tail      = nullptr;        // mbr_0xa8
    int cx, cy           = 0;             // mbr_0xac, mbr_0xb0
    int oldcx, oldcy     = 0;             // mbr_0xb4, mbr_0xb8
    int oldbuflen        = 0;             // mbr_0xbc
    TObjectInstance* box  = nullptr;      // mbr_0xc4
    TObjectInstance* axis = nullptr;      // mbr_0xc0
    HANDLE cmdthreadhandle = nullptr;     // mbr_0xc8 — see [[feedback-threading]]:
                                          //   port to worker-queue, NOT raw std::thread.

    // ctor — sets the retail pane rect (DIFFERENT from CONSOLE* defines).
    TConsolePane() : TTextPane(/*x=*/0, /*y=*/356, /*w=*/360, /*h=*/125) {}

    bool Initialize() override {
        TTextPane::Initialize();        // allocates text+undo, sets winheight,
                                        // sets mbr_0x9c = -1 (color), mbr_0x84/98 = 60
        // [pre-release defaults — retain]
        curstartx = strlen("% ");       // prompt width (PROMPT = "% ")
        // curheight=1 NOT set in retail per `0x3c` literal — UNCONFIRMED-7
        head = tail = chained = nullptr;
        box = axis = nullptr;
        wrapwidth = GetWidth();         // mbr_0x98 = mbr_0xc = 360

        // [pre-release thread + events — REPLACED in port]
        // Port: enqueue Input() lines into a thread-safe std::deque<std::string>;
        // poll from a worker (TaskQueue / Worker abstraction per
        // [[feedback-threading]]) that runs CommandInterpreter.
        return true;
    }

    void DrawBackground() override {
        // Slot 20 (0x43f150) wraps base + chained-mouse overlay.
        // Step 1: base TTextPane path.
        if (!dirty()) return;
        Surface* disp = Display();                         // PTR_DAT_005d79e0
        disp->Box(/*x=*/0, /*y=*/0, /*w=*/width(), /*h=*/height(),
                  /*color=*/0, /*zpos=*/0xff, /*normal=*/0);   // (*+0x64), pane-local
        // Multi-line text composite (text starts at window_off into buffer).
        disp->DrawMultilineText(
            /*text=*/text + window_off,
            /*dstX=*/0, /*dstY=*/3,        // pane-local; 3-px top inset
            /*lineCount=*/winheight,        // mbr_0x94
            /*fontIdx=*/CurFontIndex(),     // DAT_0065bc40
            /*pad=*/0,
            /*drawmode=*/DM_USEDEFAULT,     // 0x80000000
            /*color=*/color,                // mbr_0x9c = -1 → white
            /*hAlign=*/0, /*formatFlags=*/1, /*surfMode=*/-1);
        disp->Unlock();                                       // (*+0x44)

        // Step 2: chained-mouse overlay (slot 21 = 0x43f160 extension).
        if (chained && (cx != oldcx || cy != oldcy)) {
            RemoveText(oldbuflen, /*forward=*/false);
            char buf[64];
            if (chained->xspan) {
                if (chained->yspan)
                    sprintf(buf, "(%d, %d)", cx, cy);          // "(%d, %d)" @ 0x005ceff4
                else
                    itos(cx, buf, sizeof(buf));
                (*chained->func)(cx, cy);
            } else {
                itos(cy, buf, sizeof(buf));
                (*chained->func)(cy, cx);
            }
            InsertText(buf);
            oldbuflen = strlen(buf);
            oldcx = cx; oldcy = cy;
        }
    }

    bool Input(char* in_str) override {          // slot 31 = 0x43f350
        // (see §6 helper 5)
        // For port: enqueue each completed line into the command queue,
        // do NOT SetEvent + global cmdline pointer.
    }

    bool Output(char* in_str) override {         // slot 32 = 0x43f490
        // count newlines → cursory advance, then InsertText. Do NOT signal queue.
    }

    void ChainMouse(void (*cfunc)(int,int),       // see §6 helper 7
                    int x0, int x1, int xcur,
                    int y0 = -1, int y1 = -1, int ycur = -1);

    // Pre-release KeyPress + CharPress + MouseMove + MouseClick — bodies in
    // attic editor.cpp:699-888; retail slot wirings in §10.
};
```

---

## §13 — Port mapping notes

| retail primitive | port method | home |
|---|---|---|
| `(*+0x64)(disp, 0,0, w, h, color=0, zpos=0xff, normal=0)` (Box fill) | `Renderer->FillRect(target, x, y, w, h, RGBA{0,0,0,255})` or `TSurface::Box` (`src/surface.h:258-260`) | renderer |
| `meth_0x4be110` (multi-line text composite) | `DrawMultilineTextToTarget(atlas, text, x, y, lineCount, ...)` — **needs new primitive** wrapping `DrawTextToTarget` in a per-line loop (UI_METHOD_MAP §12: "missing → implement once in font.cpp"). The current `font.cpp` has `DrawTextToTarget` for single-line; the multi-line wrapper that respects embedded `\n` is not present. | font.cpp (NEW) |
| `meth_0x4be2b0_CompositeBuffer` (GDI DrawTextA single line) | `DrawTextToTarget(atlas, text, cellX, cellY, cellW, ETextAlign::Left, r,g,b, tw, th)` — already present per UI_METHOD_MAP §12 | font.cpp |
| `cls_0x46d6b0::meth_0x46d710(DAT_00656ce4)` (mouse-bitmap lookup) | `EditorAssets->GetCursor("lrarrow" / "udarrow")` — **editor-only primitive**; the port's release build doesn't need it | editor-asset cache |
| `CreateEventA` + `_beginthreadex` + `WaitForMultipleObjects` (command thread) | **REPLACE** with a `ThreadSafeQueue<std::string>` (per `[[feedback-threading]]`) consumed by a Worker pool — **DO NOT** raw-spawn std::thread. The producer side is `Input()`, the consumer runs `CommandInterpreter`. | a new `command_queue.{h,cpp}` |
| `SetExclusivePane(&Console)` (called from ChainMouse first record) | `Screen->SetExclusivePane(this)` (existing — TScreen has the slot) | screen |

**No new bitmap or atlas primitive is required for this pane** — only the
multi-line text wrapper (which IS a method-map gap; flagged as such here for
the map maintainer).

---

## §14 — UNCONFIRMED / open questions

1. **`DAT_0065bc40` font dimensions.** The selected font's `font_w` and
   `font_h` (read from `*(int*)(hdc+0x50)` and `*(int*)(hdc+0x54)` at
   `cls_0x5a68d8.cpp:1662-1663`) control window-height (`mbr_0x88 = mbr_0x10 /
   (font_h + 2)`), the row-stride in `meth_0x4be110` (line `:1687` uses
   `(iVar1 + iVar2) * param_4` which is suspicious — see UNCONFIRMED-2), and
   the column count. Without these the §3 / §6 size predictions ("about 8
   rows") are imprecise.
   **Impact if wrong:** the visible row count + line-stride wrong by ±1-2 px.
   **Resolve:** `DecompileAddr.java 0x004be2b0` (extract `FUN_004be2b0` fully —
   only a stub is in `cls_0x5a68d8.cpp`) and the font-init that populates
   `DAT_0065b020[]`. Cross-reference with the actual editor font asset in the
   shipped editor build.

2. **`meth_0x4be110` row-stride formula.** Line 1687:
   `(iVar1 + iVar2) * param_4` where `iVar1 = *(int*)(hdc+0x50)` and `iVar2 =
   *(int*)(hdc+0x54)`. If those are `font_w` and `font_h`, then row-stride
   would be `(font_w + font_h) * lineCount`, which doesn't make geometric
   sense for a vertical-stack-by-`font_h` layout. Likely one of `+0x50`/`+0x54`
   is `(font_h_top_padding)` or `(line_height_with_leading)`, and the formula
   is computing total composite height for the cells.
   **Impact if wrong:** the multi-line text composite's height may be over- or
   under-sized, clipping bottom rows.
   **Resolve:** `DecompileAddr.java 0x004be2b0` body + dump the HDC table init
   to see what fields `+0x50` / `+0x54` hold.

3. **Shadow gate `0x400` on `DAT_0065bc40`.** Conclusion in §7 is that the
   editor console renders **without** a text drop shadow. But `DAT_0065bc40`
   is a runtime-mutable global font selector — if at some editor mode it gets
   OR'd with `0x400` (the shadow bit), shadows would appear.
   **Impact if wrong:** §7 statement "text 3-pass shadow disabled" might need
   "disabled in default editor mode, may toggle in other modes".
   **Resolve:** grep for all writers to `DAT_0065bc40` in `_data.txt`; verify
   no `OR ... 0x400` instruction touches it.

4. **`'{'` color-escape lexer format.** Every buffer-iteration loop treats
   `'{'` as a non-width-counting character (the "if `*p != '{'` then count"
   pattern). This implies a `{HEXCOLOR}TEXT{}` style sequence. The actual
   lexer is NOT in the extracted bodies — `meth_0x4be110` and
   `meth_0x4be2b0_CompositeBuffer` are the renderers, but they get the literal
   buffer slice (including the `{}`s) and apparently strip/use them inside
   GDI rendering. The `cls_0x5a4358_TConsolePane::meth_0x43dbc0` insertion at
   line 92 writes `0x7b` ('{') as the cursor caret too, so '{' is overloaded
   as both "color marker" and "caret".
   **Impact if wrong:** ported console might render the cursor caret as a
   literal `{` character on-screen, or might miss color-escape sequences.
   **Resolve:** `DecompileAddr.java 0x004be2b0` full body + grep for any
   `*p == '{' || *p == '}'` branches inside.

5. **`DAT_0065c9e0` / `DAT_0065c9d8` modifier-key meanings.** Both appear in
   `cls_0x5a42d4.cpp:187, 200, 235, 299`. The `:187` branch on `DAT_0065c9e0`
   gates a `-6`/`-1` step (multi-char jump on Ctrl-arrow is the typical
   editor binding), so `DAT_0065c9e0 = CtrlDown` is the strong guess.
   `DAT_0065c9d8` at `:299` is checked alongside `DAT_0065c9e0` in the
   printable-char gate — likely `AltDown` or another modifier suppressing
   typing.
   **Impact if wrong:** the port miswires Ctrl-arrow word-jump.
   **Resolve:** grep for the key-state-update sites
   (`DecompileAddr.java <writer-of-DAT_0065c9e0>` from the `_data.txt`
   WRITE-mode xref list).

6. **`meth_0x43dca0` `forward` arg semantics.** The body uses `forward != 0`
   for "delete forward (DEL)", `forward == 0` for "backspace". But the
   dispatch at `cls_0x5a42d4.cpp:148, 214` calls `meth_0x43dca0(this, 1)` —
   both call sites pass `1` as `param_1` (the numchars), implicitly leaving
   the `forward` parameter (passed as a stack-pushed `in_stack_00000008`)
   from whatever the caller's stack had. This is suspicious — Ghidra may have
   conflated calling conventions.
   **Impact if wrong:** BS vs DEL key might behave wrong.
   **Resolve:** disassemble around `0x43dca0` callers to see what's on the
   stack just before the call.

7. **`mbr_0x84 = 0x3c` (= 60) at TTextPane Initialize.** Pre-release has
   `curheight = 1` set in `TConsolePane::Initialize`. Retail sets `mbr_0x84 =
   0x3c` in TTextPane (the base) — a different field, or pre-release lied?
   The cleanest interpretation is `mbr_0x84` is **not** `curheight` but a
   different field (e.g. tab stop width? max-column-count? UNCONFIRMED).
   **Impact if wrong:** tab/cursor-bounding might be wrong.
   **Resolve:** extract `virt_meth_0x444fc0_44` (TPane "geometry-changed"
   hook) to see how `mbr_0x88` and `mbr_0x84` interact.

8. **Slot 1 (`virt_meth_0x43e440`) body.** Pre-release Close is small
   (SetEvent KillEvent + WaitForSingleObject + CloseHandle×2). Retail body
   not extracted.
   **Impact if wrong:** thread shutdown wrong (but the port replaces this
   path anyway — see §13).
   **Resolve:** `DecompileAddr.java 0x0043e440`.

9. **Slots 20, 21, 25, 26, 27, 28 leaf-override bodies.** The §5/§10 mappings
   here are inferred from string xrefs + pre-release matching. The exact
   retail bodies are not in the recon dump. All of these are EDITOR-mode
   code paths; they do not affect the shipped HUD. Resolve only if porting
   the editor too: `DecompileAddr.java 0x0043f150 0x0043f160 0x0043ee10
   0x0043edc0 0x0043e920 0x0043ed90`.

10. **Initial `mbr_0xc4` (= `box`) and `mbr_0xc0` (= `axis`) are set to 0
    in `cls_0x5a4358_TConsolePane.cpp:353-354`** (the Initialize body). The
    `TObjectInstance*` semantics carry over from pre-release editor.h, but
    these are **gameplay-object pointers** (a bounding-box gizmo + an
    XYZ-axis gizmo placed at the selected object) — exclusive to the EDITOR.
    The port should `#ifdef EDITOR_BUILD` these fields per
    `[[project-editor-object-model]]`.

---

## Summary (≈150 words)

`cls_0x5a4358` is **TConsolePane** — the EXILE EDITOR's command-line console,
not the in-game game-log overlay. Three lines of evidence: two unique
`CreateEventA` failure strings (`"Could not create kill event…"` /
`"Could not create character event…"`) emitted only by
`TConsolePane::Initialize` in pre-release; the `_beginthreadex` +
2-event-wait pattern matches attic `editor.cpp:531-560` line-for-line; and the
inheritance chain `cls_0x5a4494_TPane` → `cls_0x5a42d4_TTextPane` →
`cls_0x5a4358_TConsolePane` is established via byte-identical
`virt_meth_0x43da00`. The OTHER candidate `cls_0x4a22f0` is a 76-byte
graphics-primitive helper (builds an `SDrawParam`-shape struct + paints) —
**rejected** as TConsolePane. The actual in-game **game-log overlay is
`cls_0x5a5560` = TTextBar** (sibling spec). TConsolePane renders at retail
`screen:(0,356)..(360,481)`, draws a solid-black `Box` then a multi-line text
composite at pane-local `(0,3)` with the editor font, white text, no shadow,
no chrome. Port the data model + buffer-shift helpers; replace the Win32
thread+events with a worker queue per `[[feedback-threading]]`.
