# TLogoScreen (Main Menu) — Forensics Spec

> **Status:** `forensics-complete` for the **Animate paint** (backdrop + version
> text) and the **5-button placement + per-state sprites**. **Charselect/SelStart
> entry from NewGame is traced (does NOT fire — gated on entry-mode 3 which is
> the multiplayer-only hostgame/joingame path; §10a, added 2026-05-29).**
> **Version-text overlay fully verified (font path, color path, shadow gate —
> §8 + §14 item 3, all UNCONFIRMED items in that area resolved 2026-05-29).**
> Forensics-partial on the per-button **callback bodies** (LAB_0053a1f0..
> LAB_0053a2a0): their opcode sequences are decoded from disassembly here,
> but the global mutators they set (`DAT_0065d370`, `DAT_0065d358`,
> `DAT_0066fa68`, `DAT_0066fe88`, ...) are identified only by structure, not
> by purpose-name (the screen transition pipeline is part of the wider
> gameflow track; see §10 and §14).

---

## §0 — Sources & status

### Class identity
- **Class:** `TLogoScreen`, retail class id **`cls_0x5a5d18`** (vtable @
  `0x5a5d18`, 19 slots, 76 bytes). Confirmed by Wave-1 in
  `recon/discovered/renames/agent_screens.txt:32-38,195-224` and re-confirmed
  by Wave-4B (Tier 10 OOG closeout, B.r20) in
  `recon/discovered/renames/agent_ui_wave4_oog_closeout.txt:35-37,275-279`.
  The "MainMenu" backdrop + the 5 button literals
  (`MenuNewGame`/`MenuLoadGame`/`MenuMulti`/`MenuOptions`/`MenuExit`) +
  `"Revenant   v%d.%02d"` version string are the unambiguous string-anchor
  triplet — there is no other class matching all three.
- **Inherits:** `TScreen` (`cls_0x5a5ed4`). 13 logical TScreen virtuals; only
  slots **0 (~dtor), 1 (Initialize), 2 (Close), 5 (Animate)** are overridden;
  all other slots default-inherit `TScreen` base bodies
  (`agent_screens.txt:195-197`). This panel is "mostly passive."

### Retail addresses (and the recon files for each)
| Slot | Method | Addr | Recon file |
|---|---|---|---|
| 0 | `~TLogoScreen` | `0x488ea0` | `recon/classes_original/cls_0x5a5d18.cpp:37-54` |
| 1 | `Initialize` | `0x53a2c0` | `recon/discovered/cls_0x5a5d18_TLogoScreen_Initialize_53a2c0.cpp` (full body, 122 lines) |
| 2 | `Close` | `0x53a680` | `recon/classes_original/cls_0x5a5d18.cpp:202-216` |
| 5 | `Animate` (paint) | `0x53a6d0` | `recon/discovered/cls_0x5a5d18_TLogoScreen_Animate_53a6d0.cpp` (full body, 39 lines) |
| — | Button NewGame cb | `LAB_0053a1f0` | disasm (this spec §10) — no recon drop |
| — | Button LoadGame cb | `LAB_0053a220` | disasm (this spec §10) |
| — | Button Multi cb | `LAB_0053a240` | disasm (this spec §10) |
| — | Button Options cb | `LAB_0053a260` | disasm (this spec §10) |
| — | Button Exit cb | `LAB_0053a2a0` | disasm (this spec §10) |

### Other recon read
- `recon/ghidra/cls_0x5a5d18.cpp:58-206` — Ghidra OOAnalyzer view of
  Initialize/Close (cross-checked, identical to the discovered drop modulo
  the TPane child being recognized as `cls_0x5a4494_TPane`).
- `recon/ghidra/cls_0x5a3c68.cpp` — the **button widget class** (TButton)
  ctor `FUN_0042c400` (`:128-181`), the **state-driven paint**
  `virt_meth_0x42cc30` (`:499-664`), and the U/D/S/G **sprite-loader helper**
  `meth_0x42c850` (`:343-442`) — these tell us where the (x,y) come from
  (regx/regy of the U-sprite) and how the four state sprites map to the
  button.
- `recon/ghidra/cls_0x5b98b8.cpp:858-870` (`meth_0x436790` =
  TButtonPane::AddButton) and `:908-919` (`meth_0x436900` = child by index).
- `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:466-507`
  (commands 0x52/0x53/0x54 = Options/Load/Save) — used as the reference
  shape for the per-button "load-then-push-screen" pattern, NOT used by
  TLogoScreen directly (TLogoScreen dispatches via direct globals, not
  through the TPlayScreen command switch — see §10).
- `recon/discovered/cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp` — the
  closest analog (TDeathScreen uses the same TButton/TButtonPane harness for
  3 menu buttons). Cross-referenced to confirm `FUN_0042c400` arg shape and
  the `meth_0x436790` append pattern.
- Asset dump: `tools/ui/dump_dat.py
  /Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/menus.dat
  --out-dir /tmp/menus_dump` → 16 bitmaps (1 backdrop + 5×3 state strip),
  measured dims + regx/regy below.
- Visual cross-check: composited the 5 `U` sprites onto `MainMenu` at
  `(-regx, -regy)` each — placement matches the retail title screen
  pixel-for-pixel (verified by overlay, `/tmp/menus_dump/composited.png`).
- Raw disassembly: `objdump -d -M intel
  --start-address=0x53a1f0 --stop-address=0x53a2c0` and
  `--start-address=0x53a6d0 --stop-address=0x53a800` of
  `data/Revenant.exe` — used to recover the 5 button-callback stubs (no
  recon drop) and to read the exact byte stores that build the version-text
  color word (the Ghidra decomp uses `uStack_104..uStack_101` aliases that
  hide the actual order — see §8).

### Errors to close
- N/A — this is a greenfield spec; no engine port of `TLogoScreen` exists
  yet (`grep -n 'class TLogoScreen' src/` returns nothing; only `extern
  TLogoScreen LogoScreen` in `src/revenant.h:214`).

### Snapshot note (supplementary — superseded by retail)
There is **no snapshot `src/logoscreen.cpp` or `logoscreen.h`** in the
1998/99 source drop (`grep -rl 'LogoScreen\|MainMenu' src/`,
`legacy/`, `RevenantRepo/`, `Classes_*` — only the `_CLASSDEF(TLogoScreen)`
forward-decl in `src/revtypes.h:1161` survives). Treat the retail decomp as
both sole and authoritative.

---

## §1 — Overview

`TLogoScreen` is Revenant's **title / main-menu screen** — the first
interactive surface after the engine boot and intro Smacker
(`data/Disk2/MIX_LOGO.SMK` etc., handled separately). It paints:

1. A full-screen 640×480 backdrop bitmap (`MainMenu`) showing the Revenant
   logo, the title hero portrait, "CINIMATIX" credit, and the negative-space
   margin where the menu words sit.
2. Five menu **buttons** — `New Game`, `Load Game`, `Multiplayer`, `Options`,
   `Exit` — each a hand-baked text sprite (no in-engine glyph rendering for
   the menu words themselves), positioned by the **regx/regy of its `U`
   sprite** so the artwork-baked positions land on the right-margin slope of
   the backdrop.
3. A small **version text** in the bottom-left corner reading `"Revenant
   v1.22"` (`v%d.%02d` formatted, version bytes
   `DAT_005d79dc=1`, `DAT_005d79dd=22 = 0x16`).

The screen is **always visible while active** (no conditional visibility
predicate): it is constructed once at boot (after the intro pipeline), shown
via `cls_0x5b96f4::meth_0x48ed90` (= "register screen in the
screen-manager"), and torn down when any button transitions to the next
screen (`TPlayScreen` for NewGame, the Load/Options/Multi screens for the
others — see §10). Re-entered after returning from those flows.

---

## §2 — Asset roster

All assets live in **one archive**: `data/resources_unzipped/menus.dat`
(loaded once at Initialize, stored at `DAT_0066fcac`,
`recon/discovered/cls_0x5a5d18_TLogoScreen_Initialize_53a2c0.cpp:25`).
Dimensions measured by `tools/ui/dump_dat.py /tmp/menus_dump/menus.json`.

| asset (entry name) | archive | WxH | regx, regy | flags | role | cite |
|---|---|---|---|---|---|---|
| `MainMenu` | menus.dat | 640×480 | 0, 0 | `0x002` (BM_15BIT) | Full-screen backdrop | menus.json:5-19 |
| `MenuNewGameU` | menus.dat | 244×47 | -313, -138 | `0x402` (BM_15BIT + 0x400) | "New Game" — Up state (default, orange) | menus.json:21-35 |
| `MenuLoadGameU` | menus.dat | 250×45 | -312, -190 | `0x402` | "Load Game" — Up state | menus.json:37-51 |
| `MenuMultiU` | menus.dat | 322×43 | -271, -237 | `0x402` | "Multiplayer" — Up state | menus.json:53-67 |
| `MenuOptionsU` | menus.dat | 192×47 | -338, -281 | `0x402` | "Options" — Up state | menus.json:69-83 |
| `MenuExitU` | menus.dat | 124×48 | -371, -335 | `0x402` | "Exit" — Up state | menus.json:85-99 |
| `MenuNewGameD` | menus.dat | 244×47 | -313, -138 | `0x402` | "New Game" — Down state (gray; shown while pressed/disabled) | menus.json:101-115 |
| `MenuLoadGameD` | menus.dat | 250×45 | -312, -190 | `0x402` | "Load Game" — Down state | menus.json:117-131 |
| `MenuMultiD` | menus.dat | 322×43 | -271, -237 | `0x402` | "Multiplayer" — Down state | menus.json:133-147 |
| `MenuOptionsD` | menus.dat | 192×47 | -338, -281 | `0x402` | "Options" — Down state | menus.json:149-163 |
| `MenuExitD` | menus.dat | 124×48 | -371, -335 | `0x402` | "Exit" — Down state | menus.json:165-179 |
| `MenuNewGameS` | menus.dat | 244×47 | -313, -138 | `0x402` | "New Game" — Selected/highlighted state (purple+gold; hover) | menus.json:181-195 |
| `MenuLoadGameS` | menus.dat | 250×45 | -312, -190 | `0x402` | "Load Game" — Selected | menus.json:197-211 |
| `MenuMultiS` | menus.dat | 322×43 | -271, -237 | `0x402` | "Multiplayer" — Selected | menus.json:213-227 |
| `MenuOptionsS` | menus.dat | 192×47 | -338, -281 | `0x402` | "Options" — Selected | menus.json:229-243 |
| `MenuExitS` | menus.dat | 124×48 | -371, -335 | `0x402` | "Exit" — Selected | menus.json:245-259 |

**There is NO `MenuXxxG` glow sprite** for any of the five buttons in
`menus.dat`. The TButton sprite-load helper (`cls_0x5a3c68::meth_0x42c850`,
`recon/ghidra/cls_0x5a3c68.cpp:343-442`) tries to load `<base>U`, `<base>D`,
`<base>S`, `<base>G` from the multi (the four single-char suffixes
`DAT_005cd2cc/d0/d4/d8` = `"U"`, `"D"`, `"S"`, `"G"` — verified by `objdump
-s --section=.data --start-address=0x5cd2c0 --stop-address=0x5cd2e0`,
showing `... 55 00 00 00 44 00 00 00 53 00 00 00 47 00 00 00 ...` = the
four single-char strings). The `G` lookup returns null and the button paint
just skips the glow branch (`cls_0x5a3c68.cpp:562-569`).

The keycolor in every menu bitmap is `0x0` and `BM_15BIT` says the pixel
format is RGB555 — so transparency is the **global magenta key
(`0x7c1f`)**, per `src/bitmapdecode.cpp:59-66`. The buttons (244×47 etc.)
have transparent background; only the painted letter glyphs are opaque.
The backdrop itself has no transparency.

### Cursor + format-only assets (loaded but NOT drawn by TLogoScreen)
- `cursor` (string at `0x5e43e8`) — Initialize calls
  `cls_0x46d6b0::meth_0x46d710(DAT_0065abc0)` then `FUN_0043a020(cursor)`
  (`Initialize_53a2c0.cpp:26-27`) to install a cursor sprite. **The cursor
  is not in `menus.dat`** — it's loaded from a separate cursor multi
  referenced by `DAT_0065abc0` (the system cursor handle). Cursor drawing
  is handled by the system, not this paint method.
- Font index `DAT_00667cc0` = the `"SpellTitle"` font, registered at boot
  by `FUN_00485870:0x485e7e-0x485e83` (`mov dword ptr [0x667cc0], eax`
  after `call 0x4acb30` with arg `s_SpellTitle_005d8890`). The font itself
  lives in the global font table, not in menus.dat.

### Source-rect map
Every menu button blits its **whole** bitmap (no subrect / no atlas).
Source rect is `(0, 0, width, height)` for each entry.

---

## §3 — Coordinate frames & surfaces

### Pane position + size
`TLogoScreen` owns one child pane — a generic `TButtonPane` allocated at
Initialize (`Initialize_53a2c0.cpp:34-62`, recon offset to the new pane is
`this->mbr_0x88`). The pane's rect is initialised from
`PTR_DAT_005d79e0` (= the display struct pointer):

```
this_00->mbr_0xc/0x1c = display.height   (display+0x04)
this_00->mbr_0x10/0x20 = display.width   (display+0x08)
this_00->mbr_0x4/0x8/0x14/0x18 = 0       (origin)
```
(`Initialize_53a2c0.cpp:42-50`)

→ **The pane is full-screen 640×480** (Classic display = 640×480 — see
[[project-resolution-modes]]). The pane's vftable is then swapped to
`cls_0x5a4510__vftable_5a4510` (the default `TButtonPane` shape — see
`recon/ghidra/cls_0x5a4510.cpp:1-50`, its `~` calls
`cls_0x5b98b8::virt_meth_0x434f30`, the TButtonPane intermediate's dtor).
The pane has no chrome bitmap of its own — the backdrop is drawn **directly
to the display**, not into the pane (see §5).

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| `screen` | (root) | n/a | `(0,0)` | the 640×480 framebuffer | `recon/ghidra/cls_0x5a5d18.cpp:107-125` (display dims) |
| `backdrop-region` | screen | TL | `screen:(0, 0)` | the MainMenu 640×480 image | `Animate_53a6d0.cpp:21-22` |
| `button-pane` | screen | TL | `screen:(0, 0)` (full-screen) | the TButtonPane container holding the 5 buttons | `Initialize_53a2c0.cpp:42-50` |
| `button-i` (i=0..4) | button-pane | TL | `pane-local:(-regx_i, -regy_i)` = the values in §4 | one button's rect (`mbr_0x60/0x64` = x, y; `mbr_0x68/0x6c` = w, h) | `cls_0x5a3c68.cpp:169-178` (rect computation) |
| `version-cell` | screen | BL (conceptually) | `screen:(0xf, 0x1c2) = (15, 450)` with cell **w=0x96=150, h=0x28=40** and **v-align bottom** | the "Revenant v1.22" text cell | `Animate_53a6d0.cpp:35` (disasm 0x53a771-0x53a77d) |

**Composition formula.** Every drawable element resolves to:
- backdrop: `screen_pos = (0, 0)` (directly)
- button i: `screen_pos = button-pane.origin + button_i.origin =
  (0, 0) + (-regx_i, -regy_i) = (-regx_i, -regy_i)` (the pane is
  full-screen at origin → pane-local equals screen)
- version text: `screen_pos = version-cell.origin = (15, 450)` — directly.

**Anchor verification.** The display (and thus the pane) **does not scale**
in Classic mode (it's fixed 640×480). The buttons' x/y derive from a
**baked-in regx/regy on each U-sprite** — so they behave as
TL-anchored **at the regx/regy values**, NOT as right-/bottom-anchored to
the pane. This means in Revisited 16:9 the buttons would NOT auto-re-anchor
to the right edge — they'd stay glued to the same screen coords on a wider
canvas. (Out of scope here; see §13 / [[project-resolution-modes]] /
[[feedback-ui-retained-mode]] for how the port should re-anchor — typically
TR-anchor + offset from right.)

### Surfaces
There is **NO scratch surface and NO mosaic** in this paint. The backdrop
sprite goes straight to the display surface
(`PTR_DAT_005d79e0`), the buttons paint themselves into the display via
their own paint method on a separate render pass, and the version text is
composited through `FUN_004be2b0` against the display surface again. The
direct-renderer contract (NOMENCLATURE §3) is therefore "compose
backdrop+buttons+text into ONE fixed render target (the HUD swapchain),
blit once" — the port should still pass these through a single chip RT
even though retail draws each separately, to avoid scale/anchor drift.

### Layout diagram (chip = full screen, all coords pane-local = screen)

```
0                                                             640
0 ┌─────────────────────────────────────────────────────────────┐
  │                       ┌─────── (0,0) MainMenu 640x480 ─────┐│
  │                       │              ★ R E V E N A N T ★   ││
  │                       │                                     ││
  │                       │   (hero portrait painted in art)    ││
  │  138 ────────────┌────┴──────────────────────┐──┐           ││
  │                  │  MenuNewGameU 244x47       │  │ y=138    ││
  │                  │  x=313..557                │  │          ││
  │  185 ────────────┼────────────────────────────┤  │          ││
  │  190 ────────────┼───MenuLoadGameU 250x45────┐│  │          ││
  │                  │  x=312..562                ││  │          ││
  │  235 ────────────┼────────────────────────────┘│  │          ││
  │  237 ───────┌────┼──MenuMultiU 322x43──────────┼──┤          ││
  │             │  x=271..593                       │  │          ││
  │  280 ───────└────┼────────────────────────────┘│  │          ││
  │  281 ────────────┼────MenuOptionsU 192x47────┐│  │          ││
  │                  │  x=338..530                ││  │          ││
  │  328 ────────────┼────────────────────────────┘│  │          ││
  │  335 ────────────┼─MenuExitU 124x48──┐──────────┼──┘          ││
  │                  │  x=371..495        │         │             ││
  │  383 ────────────┘────────────────────┘         │             ││
  │                                                  │             ││
  │  ┌(15, 450) version cell 150x40, v-align BOTTOM             ││
  │  │ "Revenant   v1.22" — SpellTitle font, blue-violet         ││
  │  └───────────────────────────────────────────                 ││
  │                                                                ││
  └─────────────────────────────────────────────────────────────────┘
```

(Right edges of the 5 button rects are NOT aligned — `557, 562, 593, 530,
495` — because the visual right margin is baked into the artwork; the
TButton ctor places each sprite at its own `(-regx, -regy)`. See `/tmp/menus_dump/composited.png` for the pixel-accurate overlay verification.)

---

## §4 — Static element layout

All coordinates are **screen-space final composed values** (pane is
full-screen at origin, so pane-local = screen). The `(x, y)` is the
**top-left of the sprite blit**; `(w, h)` is the sprite size.

| element | space | (x, y) | (w, h) | source rect | drawmode | cite |
|---|---|---|---|---|---|---|
| `MainMenu` backdrop | screen | (0, 0) | (640, 480) | (0, 0, 640, 480) | DM_BACKGROUND (0x10) — see §11 caveat | `Animate_53a6d0.cpp:19-22` + disasm `0x53a6e7-0x53a700` |
| Button 0 `MenuNewGame*` | screen | **(313, 138)** | (244, 47) | (0, 0, 244, 47) | `0x20000` (DM_ALPHA-ish; bitmap drawmode field) | regx=-313, regy=-138 (menus.json:21-35); `cls_0x5a3c68.cpp:169-178` rect computation |
| Button 1 `MenuLoadGame*` | screen | **(312, 190)** | (250, 45) | (0, 0, 250, 45) | `0x20000` | regx=-312, regy=-190 (menus.json:37-51) |
| Button 2 `MenuMulti*` | screen | **(271, 237)** | (322, 43) | (0, 0, 322, 43) | `0x20000` | regx=-271, regy=-237 (menus.json:53-67) |
| Button 3 `MenuOptions*` | screen | **(338, 281)** | (192, 47) | (0, 0, 192, 47) | `0x20000` | regx=-338, regy=-281 (menus.json:69-83) |
| Button 4 `MenuExit*` | screen | **(371, 335)** | (124, 48) | (0, 0, 124, 48) | `0x20000` | regx=-371, regy=-335 (menus.json:85-99) |
| Version text cell | screen | (15, 450) | (150, 40) | n/a (text) | `0x10` (DM_BACKGROUND, surface drawmode) | `Animate_53a6d0.cpp:35` + disasm `0x53a771-0x53a77d` |

### How button (x, y) is derived (composition arithmetic)
The TButton ctor `FUN_0042c400` ends with
(`recon/ghidra/cls_0x5a3c68.cpp:169-178`):
```
pdVar3 = (dword *)this->mbr_0x9c;   // mbr_0x9c = handle of the U-sprite
if (pdVar3 != null) {
    dVar1 = pdVar3[3];                                   // bitmap.regy
    (this->cls_0x5a3ab8).mbr_0x60 = -pdVar3[2];          // x = -regx
    dVar4 = *pdVar3;                                      // bitmap.width
    (this->cls_0x5a3ab8).mbr_0x64 = -dVar1;              // y = -regy
    dVar1 = pdVar3[1];                                    // bitmap.height
    (this->cls_0x5a3ab8).mbr_0x68 = dVar4;                // w = width
    (this->cls_0x5a3ab8).mbr_0x6c = dVar1;                // h = height
}
```
The TBitmapData layout (`tools/ui/dump_dat.py:29-37`) confirms:
`[0]=width, [1]=height, [2]=regx, [3]=regy`. So **(x, y) = (-regx, -regy)
of the U-sprite**, and (w, h) is the U-sprite's own w/h. The S and D
state sprites have **identical w/h/regx/regy** (cross-check: NewGameU
244×47/-313,-138 ↔ NewGameS 244×47/-313,-138 ↔ NewGameD 244×47/-313,-138)
— they swap into place at the same screen rect, just with different
artwork.

### Mirrored side
N/A — no mirror; only one screen instance, no chip model.

---

## §5 — Draw order / composition

`TLogoScreen` paints through **slot 5 = Animate** (`virt_meth_0x53a6d0`).
The TScreen base `Animate` is invoked once per frame; this override appends
panel-specific work and chains to the base.

Pseudocode (from `Animate_53a6d0.cpp` + disasm `0x53a6d0-0x53a793`):
```c
void TLogoScreen::Animate() {
    int dirty = this->mbr_0x50;  // member at +0x50 = "needs full repaint" flag

    // (1) backdrop — only when dirty
    if (dirty != 0) {
        Bitmap* mm = FUN_0046d710("MainMenu", 0x10, 0);  // menus.dat lookup
        FUN_004bd680(display, 0, 0, mm /*, drawmode=0x10, 0*/);  // see §11 note
    }

    // (2) chain to base (which paints child panes — i.e., the button pane,
    //     which in turn paints its 5 buttons)
    TScreen::Animate(this);

    // (3) version text — only when dirty
    if (dirty != 0) {
        char buf[256];
        // pack color bytes into a 4-byte stack word, then copy via 419dd0
        uStack_104 = 0xff;   // byte 0
        uStack_103 = 0x48;   // byte 1
        uStack_102 = 0xaf;   // byte 2
        uStack_101 = 0xff;   // byte 3

        sprintf_s(buf, 256, "Revenant   v%d.%02d",
                   DAT_005d79dc /*=1*/, DAT_005d79dd /*=22 = 0x16*/);

        // copy 4 color bytes into a temp on the stack (this becomes param_9
        // to CompositeBuffer, used as the colorref source — see §8 below)
        FUN_00419dd0(&temp_word, &uStack_104);

        FUN_004be2b0(display,
                      /*dstX*/ 0x0f /*=15*/, /*dstY*/ 0x1c2 /*=450*/,
                      /*w*/ 0x96 /*=150*/, /*h*/ 0x28 /*=40*/,
                      buf,
                      /*p7_colorPtr*/ 0,
                      /*fontIdx*/ DAT_00667cc0 /*= "SpellTitle" font*/,
                      /*p9*/ &temp_word,
                      /*flags*/ 0x41 /*= LEFT | BOTTOM*/,
                      /*drawmode*/ 0x10 /*DM_BACKGROUND*/);
    }

    this->mbr_0x50 = 0;  // clear dirty
}
```

**Pass / slot structure.** The screen does NOT split paint across a
DrawBackground vs Draw vs Update set of vtable slots like the panel
classes do. All work runs in the single override of slot 5 (Animate). The
button pane's own paint (the 5 TButton sprites) is invoked by the chained
`TScreen::Animate` at `0x48ff00` (`Animate_53a6d0.cpp:24`), which walks
the screen's pane list (registered by `meth_0x48ed90` in Initialize, line
119) and calls each pane's Animate slot in turn.

**Draw order summary (front-to-back, last drawn = top):**
1. **MainMenu backdrop** (screen direct) — only on dirty frames.
2. **Button pane → 5 buttons**: each TButton paints its current state
   sprite (U / D / S, selected by widget flag bits — see §6) at
   `(-regx, -regy)`. Painted EVERY frame by the TButton base (no dirty
   gate on the button side in this screen — UNCONFIRMED-1).
3. **Version text** (screen direct via `FUN_004be2b0` composite) — only on
   dirty frames.

**Why "only on dirty frames" for backdrop + version.** The `mbr_0x50`
guard means these are an **invalidation cache**: on the first frame after
the screen becomes active (or after a button mouse-up clears a Down →
Up transition that touched the backdrop), the screen sets `mbr_0x50=1`,
the next Animate redraws them, and sets it back to 0. The button pane
itself owns its own re-paint logic (TButtonPane redraws hovered/pressed
buttons every frame so the hover S-state appears responsive). The
"dirty + repaint backdrop" pattern matches the standard TScreen idiom
([[feedback-versions-over-flags]] applies for the port — prefer a
monotonic int counter over a bool).

---

## §6 — Algorithms (per helper)

### 6.1 `FUN_0046d710("MainMenu", 0x10, 0)` (backdrop lookup)
Standard resource-by-name lookup on the menus.dat multi handle (`DAT_0066fcac`).
Behavior + signature documented in **UI_METHOD_MAP §9**. Returns a bitmap
handle whose first two ints are `width=640, height=480`. The trailing `0x10, 0`
args are tag/flag (the same shape used by every other panel's
`FUN_0046d710("BackPanel", 0x10, 0)` etc. — UNCONFIRMED bound to "kind/cache
hint", but not load-bearing for paint).

### 6.2 `FUN_004bd680(display, 0, 0, mm /*, drawmode, 0*/)` (backdrop stamp)
Sprite stamp at (x, y) into the implicit current draw surface (the global
display, here selected by `mov ecx, [0x5d79e0]` before the call). Behavior
documented in **UI_METHOD_MAP §4**.

**Drawmode caveat.** The Ghidra decomp prints `(0,0,uVar2,0x10,0)`, but the
raw disassembly at `0x53a6e7-0x53a700` only pushes 3 args after the
preceding lookup call cleans its 3 stack args — so the literal `0x10, 0`
that Ghidra reports are **stale register/stack inferences**, not actual
pushes. The agent_screens note `:36-37` reads this as "draws 'MainMenu'
sprite at (0,0,16)" and `16 = DM_BACKGROUND`, which is internally consistent
("draw to background"); the visually correct interpretation is **opaque
copy** either way (MainMenu's keycolor is 0 and the magenta-key path
matters only at the rendered-pixel level — there's no magenta in the
backdrop image). The port primitive map: **`DrawBitmap(bm, 0, 0)`**
(opaque copy of the whole 640×480 bitmap) — no shadow, no tint, no
chroma-key concern.

### 6.3 `FUN_004811b0(buf, 256, "Revenant   v%d.%02d", major, minor)` (sprintf-into-buffer)
sprintf-family `(buf, bufsize, format, args...)` — cdecl, caller cleans 5
args (`add esp, 0x14` at `0x53a749`). Same family as `FUN_0058b100`
documented in **UI_METHOD_MAP §10**, but with the safe `bufsize` arg
inserted between buf and format (so it's `_snprintf`, not `sprintf`).
Result: the 256-byte stack buffer `auStack_100` holds `"Revenant   v1.22"`
(or whatever version the bytes at `0x5d79dc/dd` say at boot).

**Format string verbatim** (at `0x5e443c`, verified by
`objdump -s --section=.data --start-address=0x5e4430
--stop-address=0x5e4450`): `"Revenant   v%d.%02d"` — note the **three
spaces** between "Revenant" and "v".

**Version source bytes** (at `0x5d79dc-dd`, verified by
`objdump -s --section=.data --start-address=0x5d79d0
--stop-address=0x5d79f0`):
- byte `[0x5d79dc] = 0x01 = 1` → major = 1
- byte `[0x5d79dd] = 0x16 = 22` → minor = 22

→ rendered string for the shipping retail build = **`"Revenant   v1.22"`**.
The version bytes are statically initialized in `.data`; no runtime mutator
is wired in retail (they are essentially a build-stamp).

### 6.4 `FUN_00419dd0(dst, src)` (4-byte color copy)
1-int memcpy. Behavior fully documented in **UI_METHOD_MAP §7**. Here used
to copy the packed color word `[0xff, 0x48, 0xaf, 0xff]` from `&uStack_104`
into a scratch stack slot that becomes `param_9` to `FUN_004be2b0` — see
§8 for why this is the route.

### 6.5 `FUN_004be2b0(display, x, y, w, h, text, p7, fontIdx, p9, flags, drawmode)` (text composite)
The GDI `DrawTextA` text renderer with optional 3-pass shadow. Behavior
fully documented in **UI_METHOD_MAP §5**. Call-site values for the version
text are given in §8.

### 6.6 The button paint (`cls_0x5a3c68::virt_meth_0x42cc30`,
`recon/ghidra/cls_0x5a3c68.cpp:499-664`)
This is the **TButton paint** that runs for each of the 5 buttons each
frame. Already documented in the same form by `TSideTabsPane_SPEC.md` (this
class re-uses the U/D/S sprite trio convention). The per-state branch is at
`cls_0x5a3c68.cpp:560-579`:
```c
if      (mbr_0x14 & 0x10000 && mbr_0xa0) sprite = mbr_0xa0;  // D (down/disabled)
else if (mbr_0x14 & 0x0008  && mbr_0xa4) sprite = mbr_0xa4;  // S (selected)
else if (mbr_0x14 & 0x0004  && mbr_0xa8) sprite = mbr_0xa8;  // G (glow, null here)
else                                       sprite = mbr_0x9c;  // U (up, default)
```
The selected state shows when the mouse hovers; the down state shows on
mouse-down. With `G` being null for all five buttons, the glow branch is
dead — buttons only show U / S / D.

The blit happens at `:629-631`: `FUN_004bd680(display, mbr_0x60, mbr_0x64,
sprite, drawmode)` — i.e. an opaque-or-alpha sprite stamp at the button's
(x, y) (= `(-regx, -regy)`). The drawmode (`local_70`) defaults to
`sprite.drawmode & 0xfffdffef | 0x20` (`:582`) which strips a couple of
bits then ORs in `0x20`. For these sprites with `drawmode = 0x20000`, the
resulting effective mode is `0x20030` — but in the port path it reduces
to "alpha blit with the bitmap's own alpha buffer" (the bitmaps have
`BM_15BIT` + the `0x400` flag bit which we haven't fully decoded —
UNCONFIRMED-2). The Ghidra-reported `mbr_0xac` ("inset" / "border")
branch is unreached for these buttons (the `+0xac` field is initialised to
0 in the ctor at `:115` and never written here).

**Port mapping for the button blits**: `Renderer->DrawBitmapTinted(stateSprite,
x, y, 1, 1, 1, alpha)` with the bitmap's own alpha — same idiom as
TSideTabsPane (which is the closest implemented analog). No shadow on the
menu buttons (the sprite trio is the entire visual; there's no `FUN_00438d80`
shadow setup in the TButton paint body).

---

## §7 — Effects & shadows

- **No drop shadow on the buttons.** The TButton paint body does not call
  `FUN_00438d80` at all (`recon/ghidra/cls_0x5a3c68.cpp:499-664` — grep
  `438d80` returns 0 matches in the body; only U/D/S/G sprite stamps).
- **No drop shadow on the backdrop.** `FUN_004bd680` at `Animate_53a6d0.cpp:22`
  is a single opaque stamp; no shadow rider.
- **Version text — NO 3-pass shadow** (RESOLVED 2026-05-29; was
  UNCONFIRMED-3). `FUN_004be2b0` triggers the 3-pass black shadow when
  `param_8 & 0x400` is set (UI_METHOD_MAP §5 step 8). Here `param_8 =
  DAT_00667cc0` (the SpellTitle font index). `DAT_00667cc0` is initialized
  at boot in `FUN_00485870:0x485e74-0x485e83` by `meth_0x4acb30(font_table,
  "SpellTitle")` — and `meth_0x4acb30`
  (`recon/classes_original/cls_0x45f7c0_TObjectClass.cpp:3820-3844`) is a
  plain **string-to-index** lookup that returns a small array index
  (`0..N-1`, where N is the registered-font count = 12; SpellTitle is
  index ~10). None of those small indices carry bit `0x400`. So
  `param_8 & 0x400 == 0` and the 3-pass shadow loop at `FUN_004be2b0:389`
  is **skipped**. The version text is rendered as a single GDI `DrawTextA`
  pass with no shadow. → **In the port, render the version text WITHOUT
  shadow** (single text draw, no 3-pass black underlay). Unlike the
  in-game HUD panels (TPlyrStatusBar etc.) which use the 0x401/0x402/0x404
  font-with-shadow ids via a custom caller-side packing, TLogoScreen
  passes the raw font index unmodified — geometry-correct for the title
  screen as designed.
- **Chroma key.** All button bitmaps use `keycolor=0x0`; per
  `src/bitmapdecode.cpp:59-66`, magenta (`0x7c1f` in RGB555) is the global
  transparent pixel. The button "background" (around the letterforms) is
  magenta-keyed transparent. The backdrop has no transparent regions.
- **Pink-halo bug**: the version text WILL exhibit the pink-halo artifact
  if the port uses retail's magenta-keyed scratch path — DO NOT reproduce.
  Render text with real alpha (see [[project-retail-pink-halo-bug]] /
  UI_METHOD_MAP §16 / `TPlyrStatusBar_SPEC.md:295` for the full analysis).

---

## §8 — Text rendering

ONE text element on this screen: the version string. Cell parameters
decoded from the disassembly at `0x53a76e-0x53a77f` (the
`FUN_004be2b0` call site).

| string | cell (space, x,y,w,h) | font | px | color (RGB) | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| `"Revenant   v1.22"` | screen, **(15, 450, 150, 40)** | `DAT_00667cc0` = `"SpellTitle"` font table index (`= meth_0x4acb30` array index ≈ 10/0xa, **no `0x400` shadow bit packed**) | runs the font's own pt size (set by the font definition; not observable from this site) | **R=0, G=0, B=0** (BLACK — `param_7 = 0` literal; see "Color source — correction" below) | **left** (`param_10 & 1`) | **bottom** (`param_10 & 0x40`) | **None.** `param_8 = DAT_00667cc0` is a small array index (< 256), so `param_8 & 0x400 = 0` → the 3-pass shadow gate (`:377,389`) is false. Resolves UNCONFIRMED-3. | `"Revenant   v%d.%02d"` (three spaces between "Revenant" and "v") | `%d` ← byte `[0x5d79dc]=1`; `%02d` ← byte `[0x5d79dd]=22=0x16` | `Animate_53a6d0.cpp:30,34-35` + disasm `0x53a72d-0x53a77f` |

### Color source — **CORRECTION (2026-05-29 forensics pass)**

> **The version text is rendered BLACK, not blue-violet.** Earlier versions of
> this spec routed the blue-violet color word through `param_9`. Re-reading
> the `FUN_004be2b0` body proves the color is taken from `param_7` (which the
> call site sets to `0`); the blue-violet word goes into `param_9` — a slot
> the function only treats as a **line-metrics pointer**, never as a color.
> This appears to be a **retail bug** (developer wrote color bytes into the
> wrong stack slot); the visual output is dark text on the dark right-margin
> of the backdrop. See "param_7 vs param_9 — resolved" below.

The byte stores (verified by disasm `0x53a710-0x53a73f`) write a 4-byte
color word into `uStack_104..uStack_101` (consecutive ascending stack
addresses):

```
[uStack_104] = 0xff   ; stored at 0x53a71a (mov al, 0xff; mov [esp+8], al)
[uStack_103] = 0x48   ; stored at 0x53a73f (mov [esp+0xd], 0x48 after a push)
[uStack_102] = 0xaf   ; stored at 0x53a73a (mov [esp+0xe], 0xaf after a push)
[uStack_101] = 0xff   ; stored at 0x53a71e (mov [esp+0xb], al = 0xff)
```

Then `FUN_00419dd0(this=&[esp+8], src=&uStack_104)` copies the 4 bytes —
i.e. the LE uint32 `0xffaf48ff` — into the stack slot that lives at the
**param_9 position** of the upcoming `FUN_004be2b0` call (verified by
`ret 0x4` in `FUN_00419dd0` at `0x419dda`, so the call is callee-cleans
and the slot survives). The intended decoded color was `0xff48af` = R=175
G=72 B=255 (deep blue-violet — the "Revenant" brand color, matching the
violet of the right-side menu sprite glyph strokes).

But `FUN_004be2b0` reads color **exclusively from `param_7`**, not param_9.

### `param_7` vs `param_9` — **resolved** (closes UNCONFIRMED-5 of UI_METHOD_MAP §17)

Verified by reading `recon/discovered/FUN_004be2b0_CompositeBlit.cpp` end-
to-end (the actual color/metrics dataflow, not just the recon's prose):

- **`param_7` is the text color** (`uint32_t` packed RGB; declared as `LONG*`
  by Ghidra but cast to `uint` at use). Used at `:378-379` for the BGR swap
  → `SetTextColor`, and propagated to `pLStack_1d4 = param_7` (`:176`) for
  the fast-path surface call (`:203`) and to `FUN_004acb80(..., param_7,
  10000)` at `:536`. UI_METHOD_MAP §5 step 9 is correct.
- **`param_9` is the per-line metrics pointer**. Only used in the fast path
  at `:185-199`: if `param_9 == 0xffffffff` use the font's default metrics
  (`hdc[0x10..0x12].unused`), else use `param_9` directly. Stored into
  `pLStack_318 = param_9` (`:199`). NEVER cast to a color, NEVER byte-
  swapped, NEVER fed to `SetTextColor`.

So at the TLogoScreen call site:

| arg | retail value | what `FUN_004be2b0` does with it |
|---|---|---|
| `param_7` | `0` (literal `push 0x0` at `0x53a76e`) | **`uStack_208 = 0 → SetTextColor(hdc, 0) = BLACK`** (`:378-379, :409`) |
| `param_9` | `0xffaf48ff` (the color word, **mis-routed**) | Treated as a line-metrics pointer. Since `param_9 != 0xffffffff`, the `if`-branch at `:185` is FALSE → `pLStack_318 = 0xffaf48ff` (`:199`). The value is never derefed in the fast path (it's stored for exception-trace bookkeeping only); when the call goes through the slow path (`:207+`), `param_9` is not consulted at all. **No effect on the visible output.** |

**Retail visual output:** the version text is rendered as black glyphs in the
SpellTitle font, against the dark right-margin of the MainMenu backdrop.
Hard to read at native 640×480 — it would have been visually fine if the
intended blue-violet had landed in `param_7`. This is documented as a
retail bug NOT to reproduce (see §11 item 5 below).

### Format-flag decode (`param_10 = 0x41`)

From UI_METHOD_MAP §5 step 6:
- `0x01` → DT_LEFT (h-align LEFT)
- `0x40` → DT_BOTTOM (`& ~0x2010 | 0x24`, sets DT_BOTTOM)
- `0x100` not set → no shadow-flag in `param_10` (the shadow is gated by
  `param_8 & 0x400` instead, on the font index — see §7)

→ **h-align = LEFT, v-align = BOTTOM**. Text hangs from the BOTTOM of the
cell — so the actual glyph baseline is near `y = 450 + h - baseline` ≈
`y ≈ 480 - lineHeight`. With h=40 the cell sits at screen-y 450..490 (just
past the 480 bottom — the cell extends below the visible area; this is
intentional, because DT_BOTTOM aligns to the cell BOTTOM and the cell
bottom = 490 is below the framebuffer's last row 479, so the glyph baseline
is effectively pinned ~1 line height above the bottom of the cell, landing
roughly screen-y 470..480 for typical 10-12pt body text). **The implementor
should not vertically center the text** — DT_BOTTOM is explicit, and the
NOMENCLATURE §2 v-align rule applies: "default is TOP unless DT_VCENTER /
DT_BOTTOM is actually set."

(NOMENCLATURE §2 "baseline" rule also applies — the exact in-cell baseline
is the responsibility of the shared `font.cpp` `DrawText*ToTarget` helper,
not this spec.)

---

## §9 — Animation & dynamic behavior

This screen has **NO time-based animation, NO tweens, NO phase
transitions**. The only "dynamic" behavior is the standard TButton state
machine driven by the input layer:
- mouse-over a button → button's widget flag `mbr_0x14 |= 8` (selected) →
  paint switches from `U` to `S` sprite next frame.
- mouse-down on a button → flag `mbr_0x14 |= 0x10000` (down) → paint
  switches to `D` sprite.
- mouse-up inside the button → the callback fires (see §10), the screen
  transition starts, and TLogoScreen tears down.

There is **no fade-in, no fade-out, no hover ramp** on the buttons (this is
distinct from TSideTabsPane, which has a `cur` ramp `0..8` over 8 ticks —
no equivalent ramp in TButton's base paint body).

### Dirty / redraw model
- The screen's `mbr_0x50` is the **dirty flag**: when non-zero, Animate
  repaints the backdrop and the version text; the buttons paint themselves
  every frame regardless. After paint, `mbr_0x50` is reset to 0
  (`Animate_53a6d0.cpp:37`).
- Initial value of `mbr_0x50` is `0` (set in Initialize at
  `Initialize_53a2c0.cpp:22`) — so on the first frame after Initialize, the
  backdrop and version text would NOT be drawn. They must be invalidated
  by some external mutator (the screen-show / activate path) — this
  mutator is part of the screen-manager pipeline (`cls_0x5b96f4`) and not
  visible in the TLogoScreen body itself. **In the port,** treat the
  screen as "always dirty" while it is the foreground screen, OR set
  `mbr_0x50 = 1` whenever the screen becomes active (matches retail's
  "first frame after show" behavior). Prefer a monotonic version counter
  per [[feedback-versions-over-flags]].
- The 5 buttons own their own dirty state (TButton paint always runs;
  the U/S/D sprite swap is the only "redraw" decision).

---

## §10 — Input & dispatch

### Hit rects
Each button's hit rect = its on-screen rect (no separate hit region —
TButton uses the visible sprite rect for hit testing). Same as the table in §4:

| button | hit rect (screen, x, y, w, h) |
|---|---|
| New Game | (313, 138, 244, 47) |
| Load Game | (312, 190, 250, 45) |
| Multi | (271, 237, 322, 43) |
| Options | (338, 281, 192, 47) |
| Exit | (371, 335, 124, 48) |

(The rects do not overlap, even though the right edges drift — verified
by composited overlay `/tmp/menus_dump/composited.png`.)

### Per-button callbacks (decoded from disassembly)
Initialize attaches `mbr_0x80 = &LAB_0053aNNN` to each child TButton and
sets `mbr_0x14 |= 0x40` (= "callback installed", from `cls_0x5a3ab8.cpp:122`
"`this->mbr_0x14 = _param_11 | 0x40`" when callback present). When TButton
detects a mouse-up inside its rect, it invokes `(*mbr_0x80)(this)` — the
generic widget-action handler. The five stubs are tiny:

#### LAB_0053a1f0 — New Game callback (disasm `0x53a1f0-0x53a215`)
```asm
push 0x0                ; param: char* preselect_savename = NULL    (the stack[+0x10] arg)
push -0x1               ; param_3 = -1   (init param_1+0x6e0 = "saved sector id"; -1 = none)
push -0x1               ; param_2 = -1   (init param_1+0x6dc = "saved area id"; -1 = none)
push 0x0                ; param_1 = 0    (init param_1+0x6d8 = entry-mode; 0 = NEW GAME)
mov  ecx, 0x65caf0      ; ecx = &DAT_0065caf0 (the TPlayScreen instance)
call 0x47f4c0           ; TPlayScreen::SetEntryMode (recon: cls_0x5a5320_TCharacter.cpp:8447-8468)
mov  [0x65d370], 0x65caf0   ; pending-screen ptr = TPlayScreen
mov  ecx, 0x65d358          ; ecx = the screen manager
jmp  0x48ea40               ; meth_0x48ea40 = "TScreen::CloseAndSwap" helper
```

**`meth_0x47f4c0` body** (recon `cls_0x5a5320_TCharacter.cpp:8447-8468`,
`(this, param_1, param_2, param_3, stack[0x10]=savename)`):
```c
if (param_1 != 4) {
    this->mbr_0x6d8 = param_1;   // entry-mode  (0 here)
    this->mbr_0x6e0 = param_3;   // saved sector id   (-1)
    this->mbr_0x6dc = param_2;   // saved area id     (-1)
    if (savename != NULL) { strncpy(&this->mbr_0x6e4, savename, 0x7f); this->mbr_0x763 = 0; }
    else                  { *(char*)&this->mbr_0x6e4 = 0; }   // empty save name
}
```
(`meth_0x48ea40` is identified — `recon/classes_original/cls_0x5a5320_TCharacter.cpp:8488-8517`
— it iterates the current screen's pane list calling Close on each, sets
the "changing" flag, and calls the vtable's `+0x100` post-close hook.)

→ **Action:** Mark TPlayScreen with `entry-mode = 0 (NEW GAME)`, clear the
saved-area/sector + savename fields, set the pending-screen global to
TPlayScreen (`DAT_0065caf0`), and close TLogoScreen.

#### LAB_0053a220 — Load Game callback (disasm `0x53a220-0x53a23e`)
```asm
mov  ecx, 0x65d358              ; ecx = screen manager
mov  [0x65d370], 0x66fa78       ; pending = DAT_0066fa78 (load-game screen)
call 0x48ea40                   ; CloseAndSwap (call, not jmp — has explicit ret)
mov  [0x66fa68], 0x0            ; reset DAT_0066fa68 (load-game state flag)
ret
```

→ **Action:** Set pending-screen to the Load Game UI (`DAT_0066fa78`,
populated by `FUN_00539380_LoadDef_loadgame` per the dispatch case 0x53 at
`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:475`), then close.

#### LAB_0053a240 — Multiplayer callback (disasm `0x53a240-0x53a253`)
```asm
mov  [0x65d370], 0x659bd8       ; pending = DAT_00659bd8 (multiplayer screen)
mov  ecx, 0x65d358
jmp  0x48ea40                   ; CloseAndSwap
```

→ **Action:** Set pending-screen to the Multiplayer UI (`DAT_00659bd8`).

#### LAB_0053a260 — Options callback (disasm `0x53a260-0x53a273`)
```asm
mov  [0x65d370], 0x66fe88       ; pending = DAT_0066fe88 (options screen)
mov  ecx, 0x65d358
jmp  0x48ea40                   ; CloseAndSwap
```

→ **Action:** Set pending-screen to the Options UI (`DAT_0066fe88`,
populated by `FUN_0053a8b0_LoadDef_options` — cross-ref the dispatch case
0x52 at `FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:466-472`, same
target).

#### LAB_0053a2a0 — Exit callback (disasm `0x53a2a0-0x53a2b3`)
```asm
mov  [0x65d370], 0x0            ; pending = NULL (no next screen)
mov  ecx, 0x65d358
jmp  0x48ea40                   ; CloseAndSwap
```

→ **Action:** Set pending-screen to NULL → the screen manager interprets
this as "quit / fall back to engine shutdown". (Cross-check with
`LAB_0053a280` at `0x53a280` which also exists in this stub block: `mov
[0x65d370], 0x667fd8` — that's an UNUSED 6th button slot for **Save Game**;
TLogoScreen's Initialize only wires 5 buttons → callback 4 = `&LAB_0053a2a0`,
the Exit handler, not `&LAB_0053a280`. The presence of `LAB_0053a280` is
a leftover linker artifact / dead code for a "Save" entry that was
present at some point during development.)

### Command-id convention
TLogoScreen does **NOT** use the central `FUN_0047cf40_TPlayScreen_DispatchCommand`
command switch — those command ids (0x52 = Options, 0x53 = Load, 0x54 =
Save, etc.) are for in-game escape-menu paths through TPlayScreen. The
title-screen buttons dispatch directly through their `mbr_0x80` callback
field, bypassing the command system. The two pipelines converge later (the
LoadGame/Options screens themselves are the same UI used by both entry
points — `DAT_0066fa78` is the same `LoadGameDef` target as the case 0x53
loaded value, and `DAT_0066fe88` is the same `OptionsDef` as case 0x52).

### Modal-state globals
- `DAT_0065d370` = "pending screen target" (the next screen to swap to).
- `DAT_0065d358` = "screen manager" instance (the `this` for `meth_0x48ea40`).
- `DAT_0065caf0` = global TPlayScreen instance (see UI_METHOD_MAP §15a
  context; widely referenced).
- `DAT_0066fa78` / `DAT_0066fe88` / `DAT_00659bd8` = the LoadGame /
  Options / Multiplayer screen instances.
- `DAT_0066fa68` = a per-Screen "is-active" flag for the Load screen; the
  LoadGame callback resets it to 0 so the load screen knows it's
  freshly-entered from the title.

These are not "modal selectors" in the radio-button sense — they're the
plumbing for screen transitions, not visual state on TLogoScreen.

### §10a — NewGame click → does it open the `selstart` character-pick overlay? (NO — fully traced)

**Question (2026-05-29 forensics request):** does the path
`MenuNewGame click → TPlayScreen → ??? → selstart.def overlay (FUN_00469370_LoadDef_selstart)`
exist? Verify with the click handler + the LoadDef_selstart hook.

**Answer: NO — single-player `New Game` does NOT trigger the SelStart
character-picker.** `selstart.def` is a **multiplayer-only** character
slot picker (joingame/hostgame entry point), not a single-player gate.

#### Trace (every step cited)

1. **TLogoScreen `New Game` click → `LAB_0053a1f0`** (this spec §10
   "LAB_0053a1f0"). Sets `TPlayScreen.mbr_0x6d8 = 0` (entry-mode = 0,
   "new game"), and queues TPlayScreen as the next screen.

2. **`ScreenManager.CloseAndSwap` → `TPlayScreen::Initialize` runs**
   (recon `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp`).
   First lines of Initialize:
   ```c
   // line 23:
   DAT_0066829c = (uint)(*(int *)(param_1 + 0x6d8) == 3);
   ```
   `DAT_0066829c` is the **SelStart gate flag** — it's `1` iff
   `entry-mode == 3`. For NewGame, `entry-mode == 0`, so
   `DAT_0066829c = 0`.

3. **The entry-mode switch at `Initialize:267-300`** dispatches by
   `mbr_0x6d8`:
   - **0 (NewGame):** `*(mbr_0x6e4)=0; mbr_0x6e0=-1; FUN_0044e460();
     FUN_0048e610_LoadNewGame(); goto LAB_0047b060;`
     (`Initialize_47a660.cpp:269-275`). `LoadNewGame` loads
     `data/Modules/Demo/newgame.sav` per
     `recon/discovered/player_init_notes.md:64-93`.
   - **1 (LoadGame):** uses `mbr_0x6e4` as savename; calls
     `FUN_0048e5b0` (`:276-289`).
   - **2 (Editor):** sets up editor init + Player::ClearChar;
     `FUN_0048e610_LoadNewGame()` still called (`:290-349`).
   - **3 (Multiplayer / hostgame-or-joingame):** zeros several DAT_*
     globals + `FUN_0057a620(-1, 0)` then falls through (`:291-300`).

4. **The SelStart hook at `Initialize:392-404`** runs **AFTER** the
   entry-mode switch (post-`LAB_0047b060`), and is gated:
   ```c
   if ( DAT_0066829c != 0 && DAT_00667fcc != 0
        && ( (DAT_00667fcc[0xdb] & 0x100000U) == 0
             || (*(*DAT_00667fcc + 0x1c0)() < 1) )
        && DAT_0066829c != 0 && DAT_00659800 == 0
        && ( (*(*DAT_00667fcc + 0x1c0)() < 1)
             || (DAT_00667fcc[0xdb] & 0x100000U) != 0 ) )
   {
       FUN_0047c580(0);
       FUN_00469370_LoadDef_selstart();           // <-- selstart.def overlay opens here
       FUN_0048ed90(&DAT_006597c0, 0xffffffff);
       FUN_0048ed60(&DAT_006597c0); FUN_0048eea0(uVar2, 7);
   }
   ```
   The first conjunct `DAT_0066829c != 0` requires `entry-mode == 3`.
   For NewGame (entry-mode 0), `DAT_0066829c == 0` and SelStart is
   **skipped**.

5. **All callers of `FUN_00469370_LoadDef_selstart`** (verified by
   `objdump -d Revenant.exe | grep 'call.*0x469370'` — two hits):
   - `0x47b226` (= TPlayScreen::Initialize `:399`, the gated hook above).
   - `0x47e407` (= a small TPlayScreen helper at `0x47e3c0` that runs
     the **same** `DAT_0066829c != 0` gate; verified at disasm
     `0x47e3c0-0x47e431`). Also entry-mode-3 gated.

   Both call sites require `entry-mode == 3`. There is **no third path**.

6. **`FUN_00469370_LoadDef_selstart` body** (recon
   `recon/discovered/FUN_00469370_LoadDef_selstart.cpp`):
   ```c
   iVar1 = FUN_00435150_DefScreen_Open(
       "selstart" /*0x5d3bc4*/, &DAT_00659cb8 /*screen this-ptr*/,
       0x11, 0x7e, 0x41, 0x18a, 0x13c, 0x15c, 0x54,
       "widgets" /*0x5d3bbc*/, "selstart" /*0x5d3bb0*/);
   if (iVar1 == 0) return 0;
   *(undefined4 *)(param_1 + 0x17c) = 0;   // 4 state slots zeroed
   *(undefined4 *)(param_1 + 0x180) = 0;
   *(undefined4 *)(param_1 + 0x184) = 0;
   *(undefined4 *)(param_1 + 0x188) = 0;
   return 1;
   ```
   Opens `selstart.def` (the char-pick widget panel) at
   screen-rect `(0x41=65, 0x18a=394, 0x13c=316, 0x15c=348)` =
   approximately `(65, 394, 316, 348)`, registered against `&DAT_00659cb8`.

#### What entry-mode 3 actually is

`entry-mode == 3` is set by **the multiplayer hostgame/joingame
"Start"-equivalent button** in the DEF screen dispatcher at
`virt_meth_0x469ab0` (the hostgame button-handler). Disasm at
`0x46a08a-0x46a09f`:
```asm
mov  eax, [0x658da0]            ; some saved-area id from the lobby UI
push ebx                        ; stack[0x10] = savename = NULL
push -0x1                       ; saved sector id
push eax                        ; saved area id from lobby
push 0x3                        ; entry-mode = 3
mov  ecx, 0x65caf0              ; TPlayScreen
call 0x47f4c0                   ; SetEntryMode(3, area, -1, NULL)
mov  ecx, 0x658dc8              ; lobby's own screen manager (NOT 0x65d358)
mov  [0x658de0], 0x65caf0       ; pending = TPlayScreen
call 0x48ea40                   ; CloseAndSwap
```
NB the lobby uses **its own** screen manager (`0x658dc8`/`0x658de0`),
distinct from the title screen manager (`0x65d358`/`0x65d370`). The
title screen's NewGame callback does NOT touch the lobby globals.

#### Conclusion

| from | entry-mode set | runs `LoadDef_selstart`? |
|---|---|---|
| TLogoScreen **NewGame** click (`LAB_0053a1f0`) | **0** | **NO** — goes through `FUN_0048e610_LoadNewGame` (loads `newgame.sav`) |
| TLogoScreen **LoadGame** click (`LAB_0053a220`) | unchanged | NO — goes through Load Game UI (`DAT_0066fa78`) |
| TLogoScreen **Multiplayer** click (`LAB_0053a240`) | unchanged | NO — goes through Multiplayer Lobby UI (`DAT_00659bd8`); SelStart fires *later*, from inside the lobby's hostgame/joingame "Start" button |
| Hostgame/Joingame **Start** button (`virt_meth_0x469ab0` @ `0x46a08a-0x46a09f`) | **3** | **YES** — TPlayScreen::Initialize:399 calls `FUN_00469370_LoadDef_selstart` |

**Implication for the port.** `TLogoScreen::OnNewGame_` should NOT
trigger any character-pick overlay. The single-player NewGame flow is
direct: title → TPlayScreen → load `newgame.sav` → in-game with the
preset Locke. The character picker is a multiplayer-only path through
the lobby flow.

(**Aside on what was checked in case future readers ask:** I additionally
verified that `meth_0x4acb30(this=&fontTable, "SpellTitle")` is a
string-to-array-index lookup returning a small int — see
`recon/classes_original/cls_0x45f7c0_TObjectClass.cpp:3820-3844`. The
SpellTitle font index lands at `DAT_00667cc0` and is roughly 10/0xa
in the boot-time order of 12 font registrations between
`0x485da2-0x485e88`. Bit `0x400` is NOT set in any of those indices,
so the version-text 3-pass shadow does NOT fire — closes UNCONFIRMED-3.)

---

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo fringe around the version text.** UI_METHOD_MAP §16 +
   `TPlyrStatusBar_SPEC.md:295`. The retail `FUN_004be2b0` path renders
   text against a magenta-cleared scratch and copy-keys magenta back out,
   leaving antialiased shadow-edge pixels pinkish. The port renders with
   real alpha (no chroma key for text), so this never appears.
2. **Stale-stack drawmode in the backdrop blit.** Ghidra reports the
   `FUN_004bd680` call as `(0, 0, mm, 0x10, 0)` but the disassembly only
   pushes 3 args (`eax, 0, 0`) — the trailing `0x10, 0` are register/stack
   artifacts from the preceding lookup call (UNCONFIRMED in
   UI_METHOD_MAP §4 — "trailing 0 arg inferred"). This is a **decomp
   pitfall**, not a bug to reproduce: the drawmode for the backdrop blit
   is whatever the system's "implicit current surface" default is —
   visually it must be **opaque** (the MainMenu image has zero
   transparency outside its drawn pixels).
3. **The dead `LAB_0053a280` "Save" callback stub.** Initialize wires only
   5 buttons, but the stub block at `0x53a1f0-0x53a2bf` contains 6 stubs
   (the 6th sets pending = `DAT_00667fd8` for what would be a Save-Game
   entry). Do not wire a 6th button — the menu has 5 entries, period.
4. **Default-fallback assets for `MenuXxxG`.** The TButton sprite loader
   silently returns null when `MenuXxxG` is not in `menus.dat`. This is
   not a bug — the menu intentionally has no glow state. Don't substitute
   a procedural glow per [[feedback-no-standins]] / [[feedback-ui-no-mock-use-retail]].
5. **Mis-routed version-text color (`param_9` instead of `param_7`).**
   `Animate_53a6d0.cpp:30-34` packs the intended blue-violet color
   `0xff48af` (R=175, G=72, B=255) into the slot that becomes `param_9`
   of `FUN_004be2b0`; `param_7` is left as `0`. Since `FUN_004be2b0`
   reads color from `param_7` (`:378-379`), the rendered text is black —
   not blue-violet. See §8 "Color source — CORRECTION". **Port should
   render the version text in the intended (R=175, G=72, B=255), NOT in
   black.** This is a developer typo not a design choice, and the brand-
   colored text is what the asset metadata + the surrounding violet menu
   sprite strokes suggest was intended. (Resolves UNCONFIRMED-5 of
   UI_METHOD_MAP §17 — that ambiguity was the recon trying to
   reconcile the live call site with the body, and the resolution is
   "param_7 = color; the call site has a bug".)
6. **Pre-NewGame `TPlayerManager::Clear` runs while the editor mode is
   live.** `Initialize_53a2c0.cpp:23` always calls
   `FUN_0051eda0_TPlayerManager_Clear()` regardless of the entry point.
   Re-entering the title screen from in-game (via in-game-menu → Quit to
   Title) would re-run this; harmless because all the player state is
   torn down in `TPlayScreen::Close` first, but the order is "title init
   wipes any residual player state" rather than "title init asserts
   player state is already wiped". Not a bug per se — call out so the
   port doesn't introduce ordering assumptions that aren't there.

---

## §12 — Reconstruction pseudocode

```cpp
class TLogoScreen : public TScreen {
public:
    bool Initialize() override;
    void Close() override;
    void Animate() override;          // paint (slot 5)

private:
    TMulti*       menus_dat_     = nullptr;    // mbr_~0x88 area, ctor-loaded
    TButtonPane*  button_pane_   = nullptr;    // mbr_0x88
    bool          dirty_         = false;      // mbr_0x50
    // 5 child TButtons live inside button_pane_

    // Button activation callbacks
    static void OnNewGame_(TButton*);   // LAB_0053a1f0 — switch to TPlayScreen
    static void OnLoadGame_(TButton*);  // LAB_0053a220 — switch to LoadGame screen
    static void OnMulti_(TButton*);     // LAB_0053a240 — switch to MP screen
    static void OnOptions_(TButton*);   // LAB_0053a260 — switch to Options screen
    static void OnExit_(TButton*);      // LAB_0053a2a0 — quit
};

bool TLogoScreen::Initialize() {
    dirty_ = false;
    PlayerManager.Clear();                         // FUN_0051eda0
    InputReset();                                  // FUN_00446cb0 — UNCONFIRMED purpose
    menus_dat_ = LoadMulti("menus.dat");           // FUN_0047f670
    auto cursorSprite = LookupSprite("cursor");    // via DAT_0065abc0 / FUN_0046d710
    InstallCursor(cursorSprite);                   // FUN_0043a020
    // (clip-rect / framebuffer init via FUN_00444e20 -> screen surface — UNCONFIRMED)

    button_pane_ = new TButtonPane();              // alloc 0x17c bytes
    button_pane_->SetRect(0, 0, display->w, display->h);  // = (0,0,640,480)
    button_pane_->Init();
    button_pane_->SetFlag(/*+0x60 bit*/ 0x2);     // see Initialize_53a2c0.cpp:62 — UNCONFIRMED meaning

    // 5 buttons, in this exact order; callbacks attached after
    constexpr std::pair<const char*, void(*)(TButton*)> kEntries[] = {
        {"MenuNewGame",  &OnNewGame_},
        {"MenuLoadGame", &OnLoadGame_},
        {"MenuMulti",    &OnMulti_},
        {"MenuOptions",  &OnOptions_},
        {"MenuExit",     &OnExit_},
    };
    for (auto& e : kEntries) {
        // TButton ctor signature: (multi, baseName, p3..p6=0, p7=-1, p8=0x10, p9=-1, p10=0)
        auto* b = new TButton(menus_dat_, e.first, 0,0,0,0,
                              /*p7*/ -1, /*flag*/ 0x10, /*p9*/ -1, /*p10*/ 0);
        button_pane_->AddChild(b);                 // FUN_00436790
    }
    for (int i = 0; i < 5; ++i) {
        auto* b = button_pane_->ChildAt(i);        // FUN_00436900
        b->callback   = kEntries[i].second;        // mbr_0x80
        b->widgetFlag |= 0x40;                     // mbr_0x14 |= 0x40 ("has callback")
    }

    ScreenManager.RegisterScreen(this);            // meth_0x48ed90
    return true;
}

void TLogoScreen::Animate() {
    if (dirty_) {
        auto* mm = menus_dat_->LookupBitmap("MainMenu");
        Renderer->DrawBitmap(mm, /*x*/0, /*y*/0);   // opaque blit, screen-direct
    }

    TScreen::Animate();   // paints button_pane_ → each child TButton stamps U/S/D

    if (dirty_) {
        char buf[256];
        int major = static_cast<int>(g_versionMajor);   // = 1 at DAT_005d79dc
        int minor = static_cast<int>(g_versionMinor);   // = 22 at DAT_005d79dd
        std::snprintf(buf, sizeof(buf), "Revenant   v%d.%02d", major, minor);

        // Color literal — INTENDED retail color (R=0xaf, G=0x48, B=0xff). The
        // retail call site mis-routes this through param_9 (line-metrics) and
        // param_7 (color) = 0, so retail renders the text BLACK — that's a
        // retail bug we DO NOT reproduce. See §8 + §11 item 5.
        constexpr Color kVersionText = {175, 72, 255, 255};   // (R, G, B, A)

        // Cell (15, 450, 150, 40), v-align BOTTOM, h-align LEFT.
        // NO drop shadow — SpellTitle font index doesn't have the 0x400 bit
        // (see §14 item 3, resolved). Use the plain (no-shadow) text helper.
        Renderer->DrawTextToTarget(
            spelltitle_font_atlas,
            buf,
            /*cellX*/  15,
            /*cellY*/  450,
            /*cellW*/  150,
            /*cellH*/  40,
            ETextAlign::Left,            // h-align (param_10 & 1)
            ETextVAlign::Bottom,         // v-align (param_10 & 0x40)
            kVersionText.r, kVersionText.g, kVersionText.b);
    }

    dirty_ = false;
}

void TLogoScreen::Close() {
    ScreenManager.UnregisterScreen(this);          // meth_0x48ee10
    button_pane_->Close();                          // (*+0x4)(button_pane_)
    delete button_pane_;
    delete menus_dat_;
    InstallCursor(nullptr);                         // FUN_0043a020(0)
}

// — Button callbacks — direct pointer-store + ScreenManager swap —
void TLogoScreen::OnNewGame_(TButton*) {
    PreparePlayScreen(/*flags*/ 0,0,0xffffffff,0xffffffff,0);  // FUN_0047f4c0
    ScreenManager.RequestSwap(&g_playScreen);                  // [0x65d370] = &TPlayScreen
    ScreenManager.CloseCurrent();                              // meth_0x48ea40
}
void TLogoScreen::OnLoadGame_(TButton*) {
    ScreenManager.RequestSwap(&g_loadGameScreen);              // [0x65d370] = DAT_0066fa78
    ScreenManager.CloseCurrent();
    g_loadGameActive = false;                                  // [0x66fa68] = 0
}
void TLogoScreen::OnMulti_(TButton*)   { ScreenManager.RequestSwap(&g_multiScreen);   ScreenManager.CloseCurrent(); }
void TLogoScreen::OnOptions_(TButton*) { ScreenManager.RequestSwap(&g_optionsScreen); ScreenManager.CloseCurrent(); }
void TLogoScreen::OnExit_(TButton*)    { ScreenManager.RequestSwap(nullptr);          ScreenManager.CloseCurrent(); }
```

The button paint happens inside the `TButtonPane`'s child-walk (which is
shared with TSideTabsPane / TDeathPane); the reconstruction agent does not
need to re-write per-button drawing here — that's handled once by the
TButton paint primitive.

---

## §13 — Port mapping notes

| Retail primitive | Canonical port method | Home |
|---|---|---|
| `FUN_0046d710(name)` resource lookup | `TMulti::Lookup(name) → PTBitmap` (cached at Init) | asset-cache |
| `FUN_0047f670("menus.dat")` archive load | `LoadMulti("menus.dat")` → `TMulti*` | asset-cache |
| `FUN_004bd680(display, x, y, sprite)` backdrop blit | `Renderer->DrawBitmap(bm, x, y)` (opaque) | renderer (§12 UI_METHOD_MAP) |
| TButton paint (`virt_meth_0x42cc30`) — U/S/D sprite stamp | `Renderer->DrawBitmap(stateSprite, x, y)` w/ bitmap's own alpha | renderer / TButtonPane shared paint |
| `FUN_004811b0(buf, sz, fmt, ...)` format | `std::snprintf` | std |
| `FUN_004be2b0(...) ` text composite w/ cell + h-align + v-align (NO shadow on this call — see §14 item 3) | `Renderer->DrawTextToTarget(atlas, str, cellX, cellY, cellW, cellH, ETextAlign::Left, ETextVAlign::Bottom, r, g, b)` | font.cpp (§12 UI_METHOD_MAP) |
| `FUN_00419dd0(dst,src)` 4-byte color copy | inline `kColor = {r,g,b}` literal in the call | none |
| `FUN_0043a020(cursor)` cursor install | `InputSystem::SetCursor(sprite)` | input |
| `meth_0x48ed90(screen, pane, -1)` add pane to screen | `Screen::AddPane(pane)` | screen base |
| `meth_0x48ea40(screen)` close-and-swap | `ScreenManager::CloseCurrent()` (drives the pending-target stored in `DAT_0065d370` equivalent) | screen manager |
| `meth_0x436790(pane, btn)` add child | `TButtonPane::AddChild(btn)` | base widget |
| `meth_0x436900(pane, i)` child by index | `TButtonPane::ChildAt(i)` | base widget |

**Needed primitive (existing).** Confirm `DrawTextShadowedToTarget` supports
**v-align BOTTOM** in addition to TOP/CENTER/BOTTOM — TPlyrStatusBar uses
TOP, TSideTabsPane uses TOP, this is the first panel needing BOTTOM. If
the shared text helper currently hard-codes DT_TOP, the v-align enum and
the GDI/atlas-baseline math need a one-line extension. (Per NOMENCLATURE
§2: the in-cell baseline math is the helper's job, not the spec's — the
helper just gets `ETextVAlign::Bottom` and produces the right `y`.)

**Resolution stance** ([[project-resolution-modes]]). The button (x, y)
values above are **Classic 640×480 absolute pixel coords**, baked into the
asset regx/regy. For Revisited 1920×1080, the right-side menu must
re-anchor so it sits along the right edge regardless of canvas width. The
recommended port approach: store each button as `(anchor=TR, offset_x =
640 - x_button)` and place at runtime as `(canvas_w - offset_x, y_button)`.
The version text is BL-anchored: store as `(anchor=BL, dx=15, dy=480-450=30)`
and place at `(15, canvas_h - 30 - cellH)`. **Both transformations are out
of scope for retail-faithful Classic mode** — only apply them in Revisited
mode.

---

## §14 — UNCONFIRMED / open questions

1. **TButton paint dirty model.** The buttons' own dirty / re-paint
   decision was not extracted. The recon body of
   `cls_0x5a3c68::virt_meth_0x42cc30` (`recon/ghidra/cls_0x5a3c68.cpp:499-664`)
   has an early-out on `mbr_0x14 & 0x100000` (`:540`), but it isn't clear
   whether the parent (TButtonPane) gates each button's paint per-frame or
   every frame. **Impact if wrong:** at worst, a button visually lags one
   frame behind its hover state — visually OK. **Resolve:**
   `DecompileAddr.java 0x434f30` (`cls_0x5b98b8::virt_meth_0x434f30`, the
   TButtonPane child-walk).

2. **TButton sprite drawmode flag `0x402 = BM_15BIT + 0x400`.** The 0x400
   bitmap-flag bit is not documented in `src/revdefs.h` `BM_*` enums. It
   is suspected to mean "this bitmap carries a real alpha channel separate
   from the chroma key" or "use saturated multiply" — but neither is
   confirmed. **Impact if wrong:** the button transparency might fall back
   to magenta-key-only (the visual would be acceptable since the
   transparent region around each letter is genuinely magenta in the
   stored RGB555 pixels), OR the buttons would lose anti-aliased edges.
   **Resolve:** dump pixel data for a small button + decode it manually
   against `src/bitmapdecode.cpp:59-66` (`tools/ui/dump_dat.py` does
   honor `BM_ALPHA=0x100` but the bit here is `0x400`); also grep
   `recon/discovered/` for any other panel that loads a bitmap with flags
   = `0x402` to see how it's drawn.

3. **~~`DAT_00667cc0` (SpellTitle font) shadow bit `0x400`.~~** **RESOLVED
   2026-05-29 — bit `0x400` is NOT set.** `meth_0x4acb30`
   (`recon/classes_original/cls_0x45f7c0_TObjectClass.cpp:3820-3844`) is a
   plain **string-to-array-index** lookup over a `cls_0x41c7f0` TArray of
   font names — returns `0..N-1` where N = registered-font count, or `0`
   on miss. The boot block at `0x485da2-0x485e88` registers 12 fonts in
   sequence; SpellTitle is roughly index 10. None of those small indices
   has bit `0x400` set. Therefore `param_8 & 0x400 == 0` in
   `FUN_004be2b0:377`, and the 3-pass shadow loop at `:389` is skipped
   for the version text. → **The version text has NO drop shadow** —
   the port must NOT draw a 3-pass shadow on the version line.
   (See also §11 item 5 — the version text is also rendered BLACK because
   `param_7 = 0`. So the retail rendering is "black text, no shadow, on a
   dark backdrop margin" — very hard to read. The port should render the
   intended blue-violet color WITHOUT shadow to match the geometry while
   fixing the color bug.)

4. **`FUN_00444e20` (called at Initialize line 29).** Returns something
   that's then copied via `FUN_00419dd0_BlitEffect_SetField` into the
   screen object. The body and purpose are unknown — likely "get current
   clip rect / screen framebuffer info" but not confirmed. **Impact if
   wrong:** minor; this is part of the init sequence's "wire up the
   screen's render target" plumbing. **Resolve:**
   `DecompileAddr.java 0x00444e20`.

5. **`FUN_00446cb0` (called at Initialize line 24).** Argless call,
   presumably "reset input state" or "stop any current audio". Not
   extracted. **Resolve:** `DecompileAddr.java 0x00446cb0`.

6. **`FUN_0046d710()` argless call at Initialize line 26.** Two lines after
   the menus.dat load. This is an oddity: every other call to `FUN_0046d710`
   in the codebase takes a name or a container handle (UI_METHOD_MAP §9).
   The Ghidra view (`recon/ghidra/cls_0x5a5d18.cpp:88`) actually shows
   `iVar7 = cls_0x46d6b0::meth_0x46d710(DAT_0065abc0)` — so this is the
   cursor sprite lookup (`DAT_0065abc0` = cursor multi handle), and the
   "discovered" decomp lost the arg. Not load-bearing once that's
   understood. **Resolve:** none needed — the Ghidra ghidra/ view is
   authoritative.

7. **TLogoScreen child-pane vt-swap purpose.** The pane is initialised with
   `cls_0x5a4494__vftable_5a4494` (TPane base) then immediately swapped to
   `cls_0x5a4510__vftable_5a4510` (`Initialize_53a2c0.cpp:40,56`). The
   only difference observed is the dtor (`recon/ghidra/cls_0x5a4510.cpp`
   chains to `cls_0x5b98b8::virt_meth_0x434f30` instead of the base). The
   swap is therefore "make this a TButtonPane instead of a plain TPane".
   **Impact if wrong:** none — the port should just construct a
   `TButtonPane` directly.

8. **Button widget flag `0x10` (passed as `param_8` to TButton ctor).**
   The ctor passes `param_8 = 0x10` which is the 9th arg of `FUN_0042c400`
   (`cls_0x5a3c68.cpp:128-138`); it gets packed into `(undefined2)param_8`
   which becomes the cls_0x5a3ab8 `mbr_0x70` field (`cls_0x5a3ab8.cpp:115`).
   The semantic is UNCONFIRMED — likely a widget-class id or a "drawing
   mode hint" — but it's not visually load-bearing in the paint body.

9. **`param_7 = 0xffffffff` and `param_9 = 0xffffffff`** to TButton ctor.
   The cls_0x5a3ab8 base ctor at `FUN_0042c2d0` (`cls_0x5a3c68.cpp:80-124`)
   shows these end up as `mbr_0x74` (=keycommand?) and `mbr_0x60..0x6c`
   override (=initial rect, but immediately overwritten by the regx/regy
   logic). `0xffffffff` = -1 = "default / unset". Not visually load-bearing.

10. **Initial `mbr_0x50 = 0` (dirty=false) at Initialize line 22.** Means
    the first paint pass would skip the backdrop + version text. Where the
    flag gets set to 1 is not visible in this method body. Suspected: the
    screen-manager's "show screen" path
    (`cls_0x5b96f4::meth_0x48ed90` → some onShown hook) — but not
    extracted. **Workaround in port:** treat the screen as dirty whenever
    it's the foreground (the buttons need every-frame repaint anyway, and
    the backdrop+text are cheap to re-blit), OR set the version counter
    to "stale" whenever the screen activates. See
    [[feedback-versions-over-flags]].

11. **Visual verification deferred.** This spec was authored from the
    decomp + asset measure + overlay composite (`/tmp/menus_dump/composited.png`).
    A `--test=ui-mainmenu` test mode that exercises this paint against
    the actual port primitives would visually confirm: (a) the version
    text color is the intended blue-violet not magenta (the byte-swap
    direction); (b) the version text v-align is BOTTOM not TOP; (c) the
    buttons all sit at the right pixel coords; (d) no pink halo. See
    [[feedback-test-modes-for-ui]] and [[reference-visual-verification]]
    for the test-rig pattern.
