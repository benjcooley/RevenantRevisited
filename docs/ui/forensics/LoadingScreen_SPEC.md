# LoadingScreen (TLoadScreen) — Reconstruction SPEC

> Forensic reconstruction contract for the **boot / level-load progress
> screen**: the 640×480 image with the `LOADING GAME` banner and the
> red horizontal progress bar that fills as `TPlayScreen::Initialize` walks
> through subsystem init. Class = `cls_0x4485a0` (ctor `0x4485a0`, dtor
> `0x448650`, Step `0x448680`, Set `0x448800`). Produced per
> [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md). Terms per
> [NOMENCLATURE.md](NOMENCLATURE.md). Primitive behavior cited from
> [UI_METHOD_MAP.md](UI_METHOD_MAP.md).
>
> All paths are relative to repo root
> `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`. Every
> number states its coordinate space and cites `file:line` + `hex (= dec)`.

---

## §0 — Sources & status

**Status:** `forensics-complete` for the boot loadscreen render path. The
class has only four methods (ctor, dtor, Step, Set) and they are all
extracted; SDrawParam fields for both blit call sites have been decoded
from raw disassembly (see §6). The asset roster is dumped and measured.

**This is NOT the in-game `loadgame.def` saved-game picker** — that is
the DEF screen `cls_0x5b9584`, covered separately in
[LoadGameDef_SPEC.md](LoadGameDef_SPEC.md). The boot loadscreen here is
a tiny non-DEF, non-pane class used only during `TPlayScreen::Initialize`
(boot, new-game, load-saved-game, editor-init, multiplayer-join), gated
by `DAT_00668158` (see §1 visibility).

**Errors to close:** none — no port shell for the boot loadscreen exists
yet (`grep -rln 'loadbar\|loadscreen\|cls_0x4485a0' src/` returns
nothing). This is the first spec.

**Recon files read:**
- `recon/ghidra/cls_0x4485a0.cpp` — the full class: ctor `meth_0x4485a0`
  (load), dtor `meth_0x448650` (free), `meth_0x448680` (Step — advance bar
  by a delta), `meth_0x448800` (Set — absolute float fraction).
- `recon/ghidra/cls_0x4485a0.cpp:14` — class structure: 4 dword fields
  totaling 16 bytes; `pack(disabled)`.
- `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp` — the
  single caller of `meth_0x4485a0`/`448680`/`448800`/`448650`. The boot
  init walks 30+ subsystem-init calls, each followed by `FUN_00448680(N,0)`
  bumping the bar by `N`.
- `recon/ghidra/cls_0x4485a0.cpp:60` — dtor body (frees container).
- `recon/ghidra/_data.txt:86280..86289` — string literals
  `"\loadscreen.bmp"`, `"loadbar.dat"`.
- `recon/ghidra/_data.txt:73307,73312` — string literals `"background"`,
  `"bar"` (the two named TMulti entries inside `loadbar.dat`).
- `recon/ghidra/_data.txt:6617,4236` — float constants `0x5a4958`
  (`1000.0f`), `0x5a3570` (`0.001f`), `0x5a34e4` (`1.0f`), `0x5a3530`
  (`0.0f`).
- Raw disassembly via `objdump -M intel -d --start-address=0x448680
  --stop-address=0x4488a0` against the shipped retail
  `data/Revenant.exe` — used to decode both `SDrawParam` blocks
  (see §6).
- Asset dump via `tools/ui/dump_dat.py` of
  `data/resources_unzipped/loadbar.dat` (`/tmp/loadbar_dump/loadbar.json`)
  — the only on-disk artifact this screen draws from.

**Snapshot note (supplementary — superseded by retail).** No 1998/99 `src/`
file for the boot loadscreen exists (`grep -rli 'loadbar\|loadscreen'
RevenantRepo/src/` returns nothing). The class was new in retail; nothing
to cross-reference.

---

## §1 — Overview

`TLoadScreen` (proposed name; class `cls_0x4485a0`) is the **boot progress
screen** drawn during `TPlayScreen::Initialize` (`FUN_0047a660`,
`recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp`). It
paints a full-screen background image (640×480) and a fixed-position red
horizontal progress bar that grows left→right from 0% to 100% as the
30+ subsystem-init steps complete.

**Anatomy.** Four-field tiny class (16 B):
- `mbr_0x0` — resource container handle (the loaded `loadbar.dat`
  TMulti archive).
- `mbr_0x4` — **Background** bitmap handle: either the by-name "background"
  entry from `loadbar.dat` (default), OR an override surface loaded from
  the current module's `\loadscreen.bmp` if that file exists on disk.
- `mbr_0x8` — **Bar** bitmap handle: the by-name "bar" entry from
  `loadbar.dat` (584×48, BM_15BIT|BM_ALPHA).
- `mbr_0xc` — progress accumulator, integer in `[0, 1000]` (1000 = full
  bar). Per-mille units, clamped each step.

**Lifecycle (per `TPlayScreen::Initialize`).**
1. **Show.** At the top of Initialize, gated on `DAT_00668158 == 0`:
   `FUN_004485a0(s_loadbar_dat, override_path)` constructs the screen,
   then `FUN_00448680(0, 1)` does the initial full-frame paint (BG + 0%
   bar). (`cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:24,27,50,52`.)
2. **Step.** After EACH subsystem-init call, `FUN_00448680(delta, 0)`
   advances the bar by `delta` per-mille units and re-paints ONLY the
   bar. There are ~32 such calls (deltas seen: 5, 10, 15, 30, 40 — sum is
   typically <1000, growth is monotonic, never decreases). The bar
   internally clamps to `[0, 1000]`.
3. **Finalize.** `FUN_00448800(1.0f, 0)` slams the bar to 100%
   (`mbr_0xc = 1000`) and repaints it. (`:389`.)
4. **Hide / free.** `FUN_00448650()` (dtor-ish — frees the container,
   nulls all four fields). (`:390`.)

**Visibility predicate.** Entire boot-loadscreen block (construct + step
+ finalize) is gated by `DAT_00668158 == 0`
(`cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:24,374`,
`recon/ghidra/_data.txt:133352-133354`). `DAT_00668158` is **written by
`FUN_00483bc0`** (`_data.txt:133354`, body not extracted), gating between
"first-time boot init" and "subsequent re-init without showing
loadscreen". On every fresh `TPlayScreen::Initialize` for a new module
load / saved-game load this gate is 0 and the screen runs.

**This screen is NOT the in-game loadgame picker.** When the player
clicks "Load Game" mid-session, the DEF screen `cls_0x5b9584`
(`loadgame.def`) is shown to pick the saved game, and then if a load is
confirmed the player drops back into `TPlayScreen::Initialize` for the
target module — at which point THIS boot loadscreen is shown again,
gated as above. The two screens are sequential, not the same code.

---

## §2 — Asset roster

The screen draws from a single archive plus an optional per-module
disk-file override.

### `loadbar.dat` (in `data/resources_unzipped/loadbar.dat` — measured)

| asset | archive | entry name | WxH (measured) | role | regx, regy | flags | drawmode | source rects | cite |
|---|---|---|---|---|---|---|---|---|---|
| **Background** | `loadbar.dat` | `"background"` (index 0) | **640×480** | full-screen backdrop with "LOADING GAME" banner + module title image ("Ahkuilon" baked in for retail) | `(0, 0)` | `0x002 = BM_15BIT` | `0` | whole image | `/tmp/loadbar_dump/loadbar.json:5-14`; recon header offsets `BG[2]=regx, BG[3]=regy, BG[5]=drawmode` matched at `cls_0x4485a0.cpp:142,146,150,151` |
| **Bar** | `loadbar.dat` | `"bar"` (index 1) | **584×48** | red progress bar overlay (alpha-channel mask blends stone-edged red gradient over BG) | `(-24, -414)` ⇒ **dst-anchor `(24, 414)` on screen, absolute** | `0x402 = BM_15BIT \| BM_ALPHA` | `0x20000 = 131072` | left slice `(0, 0, dwidth, 48)` where `dwidth = (progress·Bar.width)/1000` | `/tmp/loadbar_dump/loadbar.json:15-29`; abs-regx anchor decoded at `cls_0x4485a0.cpp:142-149` (`abs(*(int*)(dVar2+8))`, `abs(*(int*)(dVar2+0xc))`) |

**Bar `regx/regy` semantics.** The bitmap's `regx, regy` are **negative**
(`-24, -414`). At every blit the class reads `abs(Bar.regx), abs(Bar.regy)`
and uses those as the absolute screen dst:
```c
// cls_0x4485a0.cpp:142-149 (Step), :246-253 (Set)
local_2c = *(int*)(BG_handle + 8);    if (local_2c < 0) local_2c = -local_2c;  // dx = abs(regx) = 24
local_28 = *(int*)(BG_handle + 0xc);  if (local_28 < 0) local_28 = -local_28;  // dy = abs(regy) = 414
```
(The decompile labels the handle `BG_handle` because `dVar2 = this->mbr_0x8`,
which I show below is the **Bar**, not the Background.) Convention: a
negative regx/regy in this codebase is the bitmap author's way of
encoding "this sprite is intended to be drawn at screen `(|regx|, |regy|)`"
— see also asset packaging convention elsewhere. Net result: bar always
paints at **screen `(24, 414)`** regardless of display size.

### Per-module override (optional, disk file)

| asset | source | format | role | cite |
|---|---|---|---|---|
| `<module-base>\loadscreen.bmp` | regular Windows BMP file on disk in the current module's folder | .BMP (read with `FUN_004a2ce0` = `fopen("rb")`+decode) | overrides the in-`loadbar.dat` "background" entry — same role, same screen rect | `cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:38-50` (string concat `<modulebase>` + `"\loadscreen.bmp"`); `cls_0x4485a0.cpp:36-43` (load-into-surface branch) |

**Override fate in shipped retail.** A `find data/Modules -iname
'loadscreen.bmp'` over the shipped Ahkuilon + Demo + Deathmatch
modules returns **nothing** — no module ships an override file. The
override path is dormant; the in-archive `"background"` entry is what
players see. (Documenting it because the boot init reads it
unconditionally; modders / Revisited content COULD use it.)

### Asset diagrams

```
loadbar.dat (2 entries, BM_15BIT)
 [0] Background   640×480   regx,regy=(0,0)        BG_drawmode=0
 [1] Bar          584×48    regx,regy=(-24,-414)   Bar_drawmode=0x20000 (alpha)
```

```
Background (640×480) — the static screen
 ┌──────────────────────────────────────────────────────┐
 │ ░░░░ ▓ LOADING GAME ▓ ░░░░░░░░░░░░ ░ y ≈ 12 (banner) │  ← stone-frame border
 │                                                      │
 │           (Ahkuilon island image)                    │
 │                                                      │
 │                    A h k a i l o n   (title, ~y=275) │
 │                                                      │
 │  ╭──────────────────────────────────────────────╮    │
 │  │     (no bar in BG — bar is the overlay)      │    │  ← y≈414..462 stone-frame trench
 │  ╰──────────────────────────────────────────────╯    │
 │                                                      │
 └──────────────────────────────────────────────────────┘
            (full 640×480, retail composes at native)
```

```
Bar (584×48)
       x=0                                       x=584
       ├───────────────────────────────────────────┤
   y=0 │  red gradient (saturated top, darker low) │
       │  stone-corner endcaps left/right via      │ h = 48
       │  bitmap's BM_ALPHA channel                │
  y=48 │                                           │
       ╰───────────────────────────────────────────╯
                ↑
       dst-anchor on screen = (24, 414) (abs of regx,regy)
       Bar is blitted as a LEFT slice (sx=sy=0, swidth=dwidth=progress_w)
```

---

## §3 — Coordinate frames & surfaces

This screen is **not** a pane — it does not allocate scratch/chrome
surfaces. It blits two bitmaps **directly into the global display
backbuffer** (`PTR_DAT_005d79e0`), then calls
`cls_0x4aa0c0::meth_0x4aa0c0(display)` to flush/present.

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|-------|--------|--------|--------------------------------------|------------------------|------|
| `screen` | (root) | — | (0, 0) on the display backbuffer (`PTR_DAT_005d79e0` data buffer) | the final pixels | universal |
| `bg` | screen | TL | `(0, 0)` literal (BG.regx=0, BG.regy=0) | the entire 640×480 backdrop | `cls_0x4485a0.cpp:142-150`, dump `/tmp/loadbar_dump/loadbar.json:9-10` |
| `bar` | screen | TL | `(abs(Bar.regx), abs(Bar.regy)) = (24, 414)` — absolute, from the Bar bitmap's encoded reg point; **does NOT scale with display size** | the 584×48 progress bar overlay | `cls_0x4485a0.cpp:142-149` (Step), `cls_0x4485a0.cpp:246-253` (Set); dump JSON `regx=-24,regy=-414` |

**Composition formula.**
- BG: `screen_x = 0 + x_in_bg`; `screen_y = 0 + y_in_bg`. Identity.
- Bar: `screen_x = 24 + x_in_bar`; `screen_y = 414 + y_in_bar`.

**Display surface contract.** The class never owns its own surface. Each
blit is a `(*+0x58)(&dp, source_bitmap)` call on `*PTR_DAT_005d79e0`
(the display). Per UI_METHOD_MAP §15a `+0x58` is the **`ParamBlit` setup
variant** that takes a fully-built `SDrawParam` + a source bitmap and
writes pixels into the display's backbuffer. After each `meth_0x448680`
or `meth_0x448800` call, the class invokes
`cls_0x4aa0c0::meth_0x4aa0c0(display)` (`cls_0x4485a0.cpp:170, 270`) —
that is the display's **flush/present** (cls_0x4aa0c0 is the TDisplay
class, 144-byte struct, see `recon/ghidra/cls_0x4aa0c0.cpp:4-22`).

**Layout diagram.**

```
screen (640×480 native)
┌──────────────────────────────────────────────────────────┐ y=0
│                                                          │
│            Background bitmap (640×480 at 0,0)            │
│            "LOADING GAME" banner top                     │
│            island art, "Ahkuilon" title                  │
│                                                          │
│                                                          │
│  x=24                                                    │
│  ↓                                                       │ y=414
│  ┌─────────────────────────────────────┐                 │
│  │   Bar overlay, dst (24,414)         │ h=48            │
│  │   sliced to dwidth = 584*progress/1000               │
│  └─────────────────────────────────────┘                 │ y=462
│  ←─────── 584 wide (full) ─────────────→                 │
│                                                          │
└──────────────────────────────────────────────────────────┘ y=480
 x=0                                                  x=640
```

### Verification: dst anchor (24, 414) vs the BG image

Independent confirmation from the dumped `/tmp/loadbar_dump/00_Background.png`:
the BG bakes-in an empty trench between the stone-frame border at
`y ≈ 414..462` (48 px tall, matches Bar.height). The Bar overlay drops
precisely into that trench. Two independent values (BG visual trench
location ↔ Bar absolute anchor) agree, so the (24, 414) decode is
solid.

### Native resolution

This screen is a 640×480 BAKED Classic-resolution asset (`BM_15BIT`
BMPs, no atlas, no rescale logic in the class). At Revisited 1920×1080
the screen is centered/scaled by the upper-level display pipeline (the
class itself doesn't know about resolution scaling). See §13 port mapping.

---

## §4 — Static element layout

Two elements only — no text rendered by this class (all on-screen text
is part of the BG bitmap).

| element | space | (x, y) | (w, h) | source rect (sx, sy, sw, sh) | drawmode | cite |
|---------|-------|--------|--------|------------------------------|----------|------|
| Background (full-frame) | screen | `(0, 0)` | `(640, 480)` = `(0x280, 0x1e0)` | `(0, 0, 640, 480)` | `BG.drawmode = 0` (opaque) | decoded SDrawParam in §6.A; values from BG bitmap header dumped |
| Progress bar (left slice) | screen | `(24, 414)` = `(0x18, 0x19e)` | `(dwidth, 48)` where `dwidth = (progress * 584) / 1000` clamped to `[0, 584]` | `(0, 0, dwidth, 48)` | `BG.drawmode = 0` (= "default"; auto-alpha kicks in because the source Bar bitmap has `BM_ALPHA` per UI_METHOD_MAP §2 step 3) | decoded SDrawParam in §6.B + Appendix B correction |

`progress` is the in-class accumulator `this->mbr_0xc`, integer in
`[0, 1000]` — initialized to 0 by the ctor (`cls_0x4485a0.cpp:28`),
advanced by each `Step(delta)`, set absolutely by `Set(fraction)`.

---

## §5 — Draw order / composition

There is one paint method (Step) and one set-and-paint method (Set) —
both have identical paint pipelines that draw two bitmaps to the
display, then present.

```
TLoadScreen::Step(delta, redraw_bg):
  guard: mbr_0x4 != 0 AND mbr_0x8 != 0
  mbr_0xc += delta;  if (mbr_0xc > 999) mbr_0xc = 1000;     // §6 line 1
  if (redraw_bg) {
      // Blit BG full-frame, opaque, 640×480 → (0,0)
      build SDrawParam (see §6.A) using BG bitmap header dims
      display->ParamBlit(&dp, BG_bitmap);                   // (*+0x58)
  }
  // Always: blit BAR (left slice = current progress)
  dwidth = (int)((float)mbr_0xc * 0.001f * Bar.width);      // see §6 alg
  build SDrawParam (see §6.B) for bar
  display->ParamBlit(&dp, Bar_bitmap);                      // (*+0x58)
  display->meth_0x4aa0c0();                                 // flush/present
```

```
TLoadScreen::Set(fraction, redraw_bg):
  guard: same
  // Clamp fraction to [0.0f, 1.0f]
  if (fraction > 1.0f) fraction = 1.0f;
  if (fraction < 0.0f) fraction = 0.0f;                    // §6.C line 1-4
  mbr_0xc = (int)(fraction * 1000.0f);
  // Same redraw_bg block + bar draw + present as Step
```

The only call sites of `Set` are `FUN_00448800(0x3f800000=1.0f, 0)` —
slamming to 100% at the end of Initialize.

**Why redraw_bg is a flag.** Painting the BG every step would re-blit a
640×480 image 30+ times per boot — wasteful. The first call
`FUN_00448680(0, 1)` does the BG once; every subsequent step passes
`redraw_bg=0` and only the bar slice (which grows monotonically) is
re-blitted on top of the already-resident BG.

**Note on overdraw.** Because the bar only ever GROWS (the
`mbr_0xc + delta` accumulator never decreases, and the per-call left-
slice draw paints from `sx=0` to `sx=dwidth`), the new bar slice always
fully covers the previous one. No erase needed.

---

## §6 — Algorithms

The only non-trivial computation is the progress→dwidth formula. The
two SDrawParam builds are fully decoded below from raw disassembly.

### Algorithm: `Step(delta, redraw_bg)` (`meth_0x448680` @ `0x00448680`)

Behavior (from `recon/ghidra/cls_0x4485a0.cpp:75-173`, asm verified
against `objdump 0x448680..0x4487f7`):

```
Step(int delta, int redraw_bg):
  if (mbr_0x4 == 0 || mbr_0x8 == 0) return;
  // 1. Advance and clamp progress
  mbr_0xc = mbr_0xc + delta;
  if (mbr_0xc >= 1000) mbr_0xc = 1000;
  // 2. Optional background repaint
  if (redraw_bg) blit_background();        // see §6.A
  // 3. Compute bar fill width
  //    fild [mbr_0xc]; fmul 0.001f; fimul Bar.width; ftol → integer dwidth
  dwidth = (int)((float)mbr_0xc * 0.001f * (float)Bar.width);
  //    For Bar.width=584:
  //      mbr_0xc=0    → dwidth=0  (no bar)
  //      mbr_0xc=500  → dwidth=292 (50%)
  //      mbr_0xc=1000 → dwidth=584 (full bar)
  // 4. Bar blit
  blit_bar_slice(dwidth);                  // see §6.B
  // 5. Present
  display->flush_and_present();            // cls_0x4aa0c0::meth_0x4aa0c0
```

**The 0.001f constant** is at `0x005a3570` in `.rdata` (raw bytes
`6f 12 83 3a` = `0x3a83126f` = `0.001`), `recon/ghidra/_data.txt:4236`
+ raw `objdump -s --section=.rdata` of the retail exe at `0x5a3570`.

### Algorithm: `Set(float fraction, int redraw_bg)` (`meth_0x448800` @ `0x00448800`)

```
Set(float fraction, int redraw_bg):
  if (mbr_0x4 == 0 || mbr_0x8 == 0) return;
  // 1. Clamp to [0,1]
  if (fraction > 1.0f) fraction = 1.0f;     // const at 0x005a34e4 = 1.0f
  if (fraction < 0.0f) fraction = 0.0f;     // const at 0x005a3530 = 0.0f
  // 2. fraction * 1000.0f → integer progress
  //    fld fraction; fmul 1000.0f; ftol → mbr_0xc
  mbr_0xc = (int)(fraction * 1000.0f);      // const at 0x005a4958 = 1000.0f
  // 3. Same paint pipeline as Step (BG optional, bar slice always, present)
  if (redraw_bg) blit_background();
  dwidth = (int)((float)mbr_0xc * 0.001f * (float)Bar.width);
  blit_bar_slice(dwidth);
  display->flush_and_present();
```

The constants `1.0f`, `0.0f`, `1000.0f`, `0.001f` are all literal
single-precision floats in `.rdata`; addresses + raw bytes cited above.

### §6.A — Background `SDrawParam` (decoded from disassembly)

Source decomp (`cls_0x4485a0.cpp:114-137`) has aliased locals that hide
the dst rect. Decoded from raw asm `objdump 0x448680..0x448739` (the
`if (redraw_bg)` branch):

```
SDrawParam dp;  // base = [esp+0x14] at lea time
// (after 5 stack pushes from prologue + the "push BG_handle" 2nd arg)

dp.drawmode  =  BG.drawmode       // [BG+0x14] (BG[5]) — dumped as 0
dp.func      =  0
dp.callback  =  0
dp.data      =  0
dp.originx   =  0
dp.originy   =  0
dp.clipx     =  0
dp.clipy     =  0
dp.clipwidth =  0
dp.clipheight=  0
dp.dx        =  0                  // BG screen TL
dp.dy        =  0
dp.dwidth    =  BG.width  = 640    // [BG+0x00]
dp.dheight   =  BG.height = 480    // [BG+0x04]
dp.sx        =  0
dp.sy        =  0
dp.swidth    =  BG.width  = 640
dp.sheight   =  BG.height = 480
dp.color     =  0
dp.intensity = 0x1f = 31           // full
dp.zpos      =  0  (u16)
dp.normal    =  0  (u16)

display->vtable[+0x58](&dp, BG_handle);   // ParamBlit
```

**Decode trail.** Anchor: the writes at `0x4486f4` (`mov [esp+0x44], edx`)
and `0x448700` (`mov [esp+0x54], edx`) put `BG.width=640` at struct
`+0x30` (dwidth) and `+0x40` (swidth). The pre-push writes at
`0x4486cd` and `0x4486d1` (`mov [esp+0x44], ecx; mov [esp+0x54], ecx`,
where `ecx = [BG+4] = BG.height`) put `BG.height=480` at struct `+0x34`
(dheight) and `+0x44` (sheight) — note pre-push offsets shift by 4
because the `push eax` (2nd vtable arg) hadn't happened yet. With
`drawmode=BG.drawmode=0` and `dx=dy=sx=sy=0`, this is a plain
`(0,0,640,480) → (0,0,640,480)` opaque blit of the entire BG to the
display.

### §6.B — Bar `SDrawParam` (decoded from disassembly)

From the second blit in both `Step` and `Set`
(`cls_0x4485a0.cpp:139-169` / `:239-269`, asm
`objdump 0x44873c..0x4487db`):

```
SDrawParam dp;  // same struct slot reused

dp.drawmode  =  BG.drawmode   =  0        // [BG+0x14] (NOT Bar.drawmode) — see Appendix B correction
dp.func      =  0
dp.callback  =  0
dp.data      =  0
dp.originx   =  0
dp.originy   =  0
dp.clipx     =  0
dp.clipy     =  0
dp.clipwidth =  0
dp.clipheight=  0
dp.dx        =  abs(Bar.regx)  = 24        // [Bar+0x08] then abs
dp.dy        =  abs(Bar.regy)  = 414       // [Bar+0x0c] then abs
dp.dwidth    =  fill_width                 // ftol(progress * 0.001 * Bar.width)
dp.dheight   =  Bar.height     = 48        // [Bar+0x04]
dp.sx        =  0
dp.sy        =  0
dp.swidth    =  fill_width                 // SAME as dwidth — no stretch, left slice
dp.sheight   =  Bar.height     = 48
dp.color     =  0
dp.intensity = 0x1f = 31                   // full
dp.zpos      =  0  (u16)
dp.normal    =  0  (u16)

display->vtable[+0x58](&dp, Bar_handle);   // ParamBlit (alpha-aware via Bar's BM_ALPHA + auto-alpha)
```

**Decode trail.** Same anchor technique:
- `mov ecx, [edi+0xc]; if (signed<0) neg ecx` (`0x44874f`) → `ecx =
  abs(Bar.regy) = 414`; stored as `ebp` and then `mov [esp+0x40], ebp`
  (`0x448797`) which is struct `+0x2c` = **dy**.
- `mov edx, [edi+0x8]; if (signed<0) neg edx` (`0x448758`) → `edx =
  abs(Bar.regx) = 24`; stored to struct `+0x28` = **dx** at `0x448793`.
- `eax = ftol(progress * 0.001f * Bar.width)` (`0x44873c..0x44874a`)
  goes to struct `+0x30` (dwidth, `0x44877b`) and struct `+0x40`
  (swidth, `0x44877f`).
- `ecx = [edi+4] = Bar.height = 48` → pre-push to struct `+0x34`
  (dheight) and `+0x44` (sheight).
- `ebx = [BG+0x14] = BG.drawmode = 0` → struct `+0x00` (drawmode).
  **Important:** the asm chain at `0x448763, 0x44876a` is
  `mov ebx, [ebx+4]; mov ebx, [ebx+0x14]` where the FIRST step
  re-reads `this->mbr_0x4` (the **Background** handle, not Bar), so
  the Bar blit's drawmode is `BG.drawmode = 0`, not Bar's stored
  `0x20000`. See Appendix B for the full disassembly trail.

**Why no shadow / no DM_TRANSPARENT.** The Bar bitmap uses `BM_ALPHA`
(flags `0x402`), so blending happens via the stored per-pixel alpha
channel. The SDrawParam carries `drawmode = 0` (= BG.drawmode), so
`FUN_004bd490_Blit` falls through to the **auto-alpha** path
(UI_METHOD_MAP §2 step 3): when the source bitmap's flags `[BG+0xe]`
have `0x30000` set (BM_ALPHA), the blit OR's `4` into the mode and uses
the per-pixel alpha. There is no chroma-key, no shadow offset, no
`FUN_00438d80` setup, no `FUN_004be2b0` text — this is a single
two-blit screen with no glyphs.

### §6.C — Background loader (ctor `meth_0x4485a0`)

```
TLoadScreen(const char* dat_name = "loadbar.dat",
            const char* bmp_override_path):
  mbr_0xc = 0;                                              // progress = 0
  if (mbr_0x0 == 0)
      mbr_0x0 = FUN_0047f670(dat_name, -1, 0);              // mount archive
  if (mbr_0x0 == 0) return;

  // Optional .BMP override
  mbr_0x4 = 0;
  if (bmp_override_path != null
      && FUN_004a1c00(bmp_override_path, null) != 0)   // file exists?
  {
      surface = FUN_004a1ec0(display.w, display.h,
                             display.fmtbits & 0x3001f, 0);
      if (FUN_004a2ce0(surface, bmp_override_path) != 0) {  // load .BMP
          mbr_0x4 = surface;                                // use as BG
      } else {
          FUN_004830f0(surface);                            // free on fail
      }
  }
  if (mbr_0x4 == 0)
      mbr_0x4 = mbr_0x0->meth_0x46d710("background");       // first named entry
  mbr_0x8     = mbr_0x0->meth_0x46d710("bar");              // next named entry
```

The string `"background"` is at `0x005cfe5c` (xref at `0x00448630`,
`_data.txt:73307`); `"bar"` at `0x005cfe68` (xref at `0x0044863f`,
`_data.txt:73312`).

### §6.D — Free (`meth_0x448650`)

```
TLoadScreen::Free():                  // also wired as dtor
  mbr_0xc = 0;
  if (mbr_0x0 != 0) {
      FUN_00482f80(mbr_0x0);          // free archive
      mbr_0x0 = mbr_0x4 = mbr_0x8 = 0;
  }
```

Note `FUN_00482f80` is the matched `free`/`delete` for the
`FUN_00482fb0` allocator pair (see `meth_0x4485a0` ctor at
`cls_0x4485a0.cpp:30` — analog pattern across all TMulti owners).

---

## §7 — Effects & shadows

**None.** No per-side shadow setup (`FUN_00438d80` is not called), no
text (no `FUN_004be2b0`), no chroma-key reliance (Bar uses BM_ALPHA),
no blit-effect registration. The screen is two opaque/alpha blits and a
present.

The Bar's stone-corner endcap fade is part of the **bitmap's own alpha
channel**, decoded from the `BM_ALPHA` flag — not a runtime shadow.

---

## §8 — Text rendering

**No text is drawn by this class.** All textual content ("LOADING GAME"
banner, "Ahkuilon" title) is **baked into the Background bitmap**. No
`FUN_004be2b0` calls.

(This means there is also no font-id, no shadow flag, no align bits to
spec — and no string format. Strings shipped per-module are baked into
the per-module `loadscreen.bmp` if the modder/dev uses the override
path; in shipped retail Ahkuilon, the in-archive Background bitmap is
what every player sees.)

---

## §9 — Animation & dynamic behavior

Single animated value: **the progress bar's fill width**.

```
ramp progress.dwidth:
  range    0..584 (= Bar.width pixels)
  step     per-call: dwidth = (mbr_0xc * 584) / 1000, where mbr_0xc
           ramps 0..1000 by per-call delta arg
  drive    each subsystem-init step in TPlayScreen::Initialize calls
           Step(N, 0) with N ∈ {5,10,15,30,40} (sum ~ 600); Set(1.0f,0)
           at the end forces to 1000.
  easing   linear (integer multiply + division)
  on-end   hold at 584; screen is then freed by FUN_00448650.
  cite     cls_0x4485a0.cpp:108-113 (clamp); :140 + 0x5a3570 (0.001f)
           + 0x448742 fimul Bar.width;
           cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:55-373 (all calls).
```

**Per-step delta distribution** (extracted from the TPlayScreen Init
recon `FUN_00448680(N, 0)` call args):
| delta | count | total | role |
|-------|-------|-------|------|
| `0` | 1 | 0 | initial paint (`FUN_00448680(0, 1)` — only one with redraw_bg=1) |
| `5` | 20+ | ~100 | per-subsystem-init step (each `T*Pane::Initialize`) |
| `0xa = 10` | 4 | 40 | mid-weight init (script mgr, env, asset cache) |
| `0xf = 15` | 1 | 15 | `EquipPane.dat` load |
| `0x1e = 30` | 1 | 30 | `TAreaMgr::Initialize` |
| `0x28 = 40` | 1 | 40 | `Creating background areas` (heaviest single step) |

Sum of deltas in retail is ~225 (well under 1000), so the bar will
NOT reach 100% naturally — the `Set(1.0f, 0)` at `:389` is what
finishes it. **This is intentional, not a retail bug:** the delta
budget is generous, so steps complete fast and the user sees a bar
that grows then is forced to 100% at the end.

**Dirty / redraw model.**
- BG: drawn **once** at construction (`Step(0, 1)`), then never until
  a re-init (the gate `DAT_00668158 == 0` reopens it for the next
  `TPlayScreen::Initialize`).
- Bar: re-drawn on **every** Step / Set call regardless of progress
  change (no early-out on `mbr_0xc` unchanged). Overdraw is benign
  because the slice grows monotonically and fully covers the previous.

**Phase / state machine.** Single phase: Boot. No transitions, no
sub-states.

```
progress (per-mille)
1000 ┤                                         ●─── (Set 1.0f at end)
     │                              ╱ ╱ ╱ ╱ ╱ ╱
     │                          ╱ ╱           (linear ramps as
     │                        ╱                init steps fire)
     │                    ╱
     │                ╱
   0 ┤●────────────                              tick → init step
     └────────────────────────────────────────── steps
       (init begin)                             (init end → Set 1.0f → free)
```

---

## §10 — Input & dispatch

**Non-interactive.** The screen consumes no mouse / keyboard input. It
is drawn during a synchronous init loop where the main thread is busy
running `TPlayScreen::Initialize`. No hit rects, no command ids.

(Players can technically Alt-F4 the main window; that goes through the
Win32 WndProc `FUN_00448cc0`, unrelated to this class.)

---

## §11 — Retail bugs NOT to reproduce

None observed. The screen is two opaque/alpha blits over a baked image
— no glyphs, no chroma key, no shadow stack, so the usual culprits
(pink-halo, off-by-1 shadow) don't apply. The only "quirk" is the
deliberate forced 100% at the end (§9), which is intended behavior.

---

## §12 — Reconstruction pseudocode

End-to-end paint + lifecycle the implementor follows. All numbers
reference §-tables; do not re-derive.

```cpp
struct TLoadScreen {
    PTMulti           archive   = nullptr;   // loadbar.dat
    PTBitmap          bg        = nullptr;   // "background" 640×480
    PTBitmap          bar       = nullptr;   // "bar" 584×48
    int               progress  = 0;         // 0..1000

    bool Initialize(const char* dat = "loadbar.dat",
                    const char* override_bmp = nullptr);
    void Finalize();                          // == meth_0x448650
    void Step(int delta, bool redraw_bg);
    void Set(float fraction, bool redraw_bg);

private:
    void PaintBackground();
    void PaintBar();
    void Present();
};

bool TLoadScreen::Initialize(const char* dat, const char* override_bmp) {
    progress = 0;
    if (!archive) {
        archive = MountArchive(dat);  // FUN_0047f670
        if (!archive) return false;
    }
    bg = nullptr;
    if (override_bmp && FileExists(override_bmp)) {
        // Make a display-format surface and load the .BMP into it
        auto* surf = Renderer->CreateSurface(display.width, display.height,
                                             display.format_bits);
        if (LoadBMP(surf, override_bmp))  bg = surf;
        else                              Renderer->FreeSurface(surf);
    }
    if (!bg)  bg = archive->LookupBitmap("background");   // §6.C
    bar       = archive->LookupBitmap("bar");
    return bg && bar;
}

void TLoadScreen::Step(int delta, bool redraw_bg) {
    if (!bg || !bar) return;
    progress = std::min(progress + delta, 1000);
    if (redraw_bg) PaintBackground();
    PaintBar();
    Present();
}

void TLoadScreen::Set(float fraction, bool redraw_bg) {
    if (!bg || !bar) return;
    fraction = std::clamp(fraction, 0.0f, 1.0f);
    progress = static_cast<int>(fraction * 1000.0f);
    if (redraw_bg) PaintBackground();
    PaintBar();
    Present();
}

void TLoadScreen::PaintBackground() {
    // §4 row 1 + §6.A
    Renderer->DrawBitmap(bg, /*dst_x=*/0, /*dst_y=*/0);
    //  ↑ opaque, full 640×480, drawmode = bg.drawmode (= 0)
}

void TLoadScreen::PaintBar() {
    // §4 row 2 + §6.B
    const int dst_x = std::abs(bar->regx);   // = 24
    const int dst_y = std::abs(bar->regy);   // = 414
    const int dw    = (progress * bar->width) / 1000;    // §6 alg
    if (dw <= 0) return;                                  // nothing to draw
    Renderer->DrawBitmapSubrect(
        bar, dst_x, dst_y,
        /*sx=*/0, /*sy=*/0, /*sw=*/dw, /*sh=*/bar->height /*=48*/);
    //  ↑ alpha-blended via bar's BM_ALPHA channel (Bar.drawmode = 0x20000)
}

void TLoadScreen::Present() {
    // §3 — equivalent of cls_0x4aa0c0::meth_0x4aa0c0
    Renderer->FlushAndPresent();
}

void TLoadScreen::Finalize() {
    progress = 0;
    if (archive) { ReleaseArchive(archive); archive = bg = bar = nullptr; }
}

// ---- driver, from TPlayScreen::Initialize ----
TLoadScreen load;
if (!skipLoadscreen /* DAT_00668158 == 0 */) {
    std::string override = current_module_path + "\\loadscreen.bmp";
    load.Initialize("loadbar.dat", override.c_str());
    load.Step(0, /*redraw_bg=*/true);          // initial full paint
}
// ... 30+ subsystem init steps, each followed by:
load.Step(N, false);          // N from §9 delta table
// ... at the very end:
load.Set(1.0f, false);
if (!skipLoadscreen) load.Finalize();
```

---

## §13 — Port mapping notes

| retail primitive | port method | home |
|---|---|---|
| Mount `loadbar.dat` (`FUN_0047f670`) | `Resources::MountArchive(path)` returning a TMulti handle | resource cache (existing in port) |
| By-name bitmap lookup (`meth_0x46d710("name")`) | `TMulti::FindBitmap(name) → PTBitmap` | per UI_METHOD_MAP §9 — "asset-cache lookup by name" |
| Optional `.BMP` load via `FUN_004a2ce0` | `Renderer->LoadBitmapFromBMP(path)` + create-surface-with-display-format helper | needs new primitive if not present: **`LoadBitmapFromBMPFile(path)` returning a PTBitmap matching display format** (Windows BMP decoder; the codebase already does `.dat` decoding but not `.bmp`) |
| Background blit `(*+0x58)` opaque full-frame | `Renderer->DrawBitmap(bg, 0, 0)` (HUD swapchain direct — this is HUD, not chip-RT) | renderer |
| Bar blit `(*+0x58)` alpha-aware left slice | `Renderer->DrawBitmapSubrect(bar, dst_x, dst_y, 0, 0, dw, 48)` — Bar's BM_ALPHA gives the soft edges automatically | renderer |
| Display present (`cls_0x4aa0c0::meth_0x4aa0c0`) | `Renderer->FlushAndPresent()` / equivalent end-of-frame call. Init is synchronous; one explicit present per step | renderer |
| Absolute screen anchor at native 640×480 | Center-and-letterbox at Revisited 1920×1080 (Classic asset → native pixel grid) — handled by the upper-level display pipeline ([[project-resolution-modes]]), not this class | display |

**Direct-swapchain note.** This screen is the boot HUD — no chip RT, no
2-pass composition. Use the direct-swapchain `DrawBitmap` /
`DrawBitmapSubrect` family, not the `…ToTarget` family. This is an
exception to the usual panel "compose to chip RT then blit" contract
(NOMENCLATURE §3 direct-renderer contract) because there is no chip
here and the screen is shown OUTSIDE the normal HUD render loop (init
is synchronous and explicitly presents each step).

**Resolution mode behavior.** At Classic 640×480 the asset paints
unscaled at native coordinates. At Revisited 1920×1080 the screen is
**centered** in the display with letterboxing (consistent with the
established Classic-mode behavior for the other UI panels per
[[project-resolution-modes]]); a future spec deviation could swap in a
native-1080p override `loadscreen.bmp` via the override path.

**Modernization opportunity (not required).** Retail makes 30+ separate
`PaintBar` calls on the synchronous init thread; each invokes a full
`FlushAndPresent`. A modernization could:
1. Batch consecutive Step calls into a single present, OR
2. Replace the integer 0..1000 unit with a 0..1 float and skip the
   per-mille trick.
Neither is required for parity; the present-per-step preserves the
"smooth bar growing" visual the original is famous for.

---

## §14 — UNCONFIRMED / open questions

1. **The exact bar dst-anchor on a non-Ahkuilon module.** I infer that
   `Bar.regx, Bar.regy` lives in the `loadbar.dat` Bar header (dumped
   `-24, -414`), and `abs()` is applied at every draw. The retail
   archive ships ONE `loadbar.dat` (the shared Resources one — every
   module uses the same Bar), so for shipped content this is fixed at
   `(24, 414)`. **Impact:** none for parity. **Resolve:** none needed
   for shipped content; if a Revisited modder ever swaps the Bar's
   regx/regy in a modified `loadbar.dat`, the dst moves accordingly —
   that's a feature.

2. **`Bar.drawmode = 0x20000` field — UNUSED by this code path.**
   The Bar bitmap header stores `drawmode = 0x00020000`, but the call
   site does NOT read it (the §6.B / Appendix B decode shows the
   SDrawParam drawmode is read from `mbr_0x4 = Background.drawmode = 0`,
   not from the Bar handle). So `0x20000` is informational only here;
   if it matters elsewhere (e.g. a different caller), document it
   then. **Impact:** none for this screen — auto-alpha lights up via
   the source bitmap's `BM_ALPHA` flag (UI_METHOD_MAP §2 step 3).
   **Resolve:** grep `0x20000` callers in retail to see if any other
   code path reads it; not blocking.

3. **`cls_0x4aa0c0::meth_0x4aa0c0` semantics — present or just flush.**
   I describe it as "flush/present" because (a) it is called at the END
   of every paint cycle and (b) the class struct is 144 bytes
   (TDisplay-sized), and (c) it takes 4 args pushed in the asm
   (`0, 0, display.height, display.width`) — likely a "mark rect dirty
   + flip" pattern. **Impact:** if it's flush-only and the upper layer
   does the present separately, the port would still work because
   GPU-side composition lands on the next swap. **Resolve:** extract
   `DecompileAddr.java 0x004aa0c0` to see the body. (NOT urgent — call
   it once after PaintBar regardless and the visual is correct.)

4. **`DAT_00668158` (boot-loadscreen-skip gate).** Written by
   `FUN_00483bc0` (not extracted). I asserted it means "first-time
   boot vs. subsequent re-init" — based on the semantics being a
   single gate around the whole construct/step/finalize block. The
   alternative is that it's the "skip ALL init UI" gate for
   automated/headless runs. **Impact:** port-side, just gate the
   screen on the equivalent boolean; in dev mode it can default to
   shown. **Resolve:** `DecompileAddr.java 0x00483bc0` to confirm.

5. **`meth_0x46d710` ordering ↔ archive entry index.** I rely on the
   `loadbar.dat` `numoffsets=2` order (entry 0 = Background, entry 1 =
   Bar) being what the by-name lookup returns when the names
   `"background"` and `"bar"` are passed. The dump tool confirms the
   archive has both names indexed at 0 and 1 respectively, and the
   ctor's two sequential lookups (with the two strings as args) cleanly
   map to those entries. **Impact:** if the order were swapped, the BG
   and Bar would swap and the bar would draw at (0,0) full-screen.
   **Resolve:** the visual would be obviously broken in a
   `--test=loadscreen` rig; trust the by-name semantics.

6. **Per-module loadscreen.bmp override format constraints.** The
   override path passes the file to `FUN_004a2ce0` which is a vanilla
   `fopen("rb")` + decode. The decode primitive likely accepts the
   standard Windows BMP file (any size, any bit-depth supported by the
   BMP loader); it then is allocated into a surface matching the
   display dims (`FUN_004a1ec0(display.w, display.h, ...)`) — which is
   already-rasterized-into the display format. **Impact:** if a
   modder ships a 1920×1080 `loadscreen.bmp`, behavior depends on the
   BMP loader's stretch/clip policy. **Resolve:** extract
   `FUN_004a2ce0` (not strictly needed — the override path is
   dormant in shipped content).

7. **`progress` advance total < 1000 in shipped retail.** I tabulated
   the deltas in §9 and they sum to ~225. The `Set(1.0f, 0)` at the
   end snaps to 100%. The user-visible result is "the bar grows then
   jumps to full at the end." If the dev intent was "fill the budget
   exactly", the deltas would sum to 1000, which they don't —
   suggesting the deltas were tuned loosely. Treat as intended (no
   bug). **Impact:** none. **Resolve:** none — verify visually that
   the bar in the port matches retail's "grow then snap" cadence.

---

## Appendix A — Asset dump command

```sh
cd /Users/benjamincooley/projects/RevenantRevisited/worktrees/ui
python3 tools/ui/dump_dat.py \
    /Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/loadbar.dat \
    --out-dir /tmp/loadbar_dump
```
Output: `00_Background.png` (640×480), `01_Bar.png` (584×48),
`loadbar.json`, `loadbar.def`.

## Appendix B — Disassembly anchors (Bar SDrawParam decode)

```
0x44873c  fild [ebx+0xc]            ; progress (int)
0x448742  fmul [0x5a3570]           ; * 0.001f
0x448748  fimul [edi]               ; * Bar.width (edi = &Bar)
0x44874a  call __ftol               ; → eax = fill_width
0x44874f  mov ecx, [edi+0xc]        ; ecx = Bar.regy
0x448752  jge 0x448758              ; if signed-negative…
0x448756  neg ecx                   ;   …abs(regy) = 414  → ebp → struct +0x2c (dy)
0x448758  mov edx, [edi+0x8]        ; edx = Bar.regx
0x44875b  mov ebp, ecx
0x44875d  jge 0x448763              ; if signed-negative…
0x448761  neg edx                   ;   …abs(regx) = 24   → struct +0x28 (dx)
0x448763  mov ebx, [ebx+0x4]        ; ebx = mbr_0x4 = Background handle
0x448766  mov ecx, [edi+0x4]        ; ecx = Bar.height = 48
0x448769  push edi                  ; vtable 2nd arg = Bar handle
0x44876a  mov ebx, [ebx+0x14]       ; ebx = Background.drawmode  ← WAIT: ebx was Background?
0x44876d  mov [esp+0x48], ecx       ; struct +0x34 (dheight) = 48
0x448771  mov [esp+0x58], ecx       ; struct +0x44 (sheight) = 48
0x448775  mov ecx, [0x5d79e0]       ; ecx = display
0x44877b  mov [esp+0x44], eax       ; struct +0x30 (dwidth)  = fill_width
0x44877f  mov [esp+0x54], eax       ; struct +0x40 (swidth)  = fill_width
0x448783  mov [esp+0x14], ebx       ; struct +0x00 (drawmode) = ebx
0x44878f  mov [esp+0x3c], edx       ; struct +0x28 (dx) = 24
0x448797  mov [esp+0x40], ebp       ; struct +0x2c (dy) = 414
0x4487c5  mov [esp+0x60], 0x1f      ; struct +0x4c (intensity) = 0x1f
0x4487d7  push eax (=&dp)
0x4487d8  call [edx+0x58]           ; display->ParamBlit(&dp, Bar)
```

**One disasm subtlety.** At `0x448763` `mov ebx, [ebx+0x4]`, ebx was
holding `this` (the load-screen object). `[this+0x4]` = `mbr_0x4` =
**Background handle** (NOT Bar). Then at `0x44876a` `mov ebx, [ebx+0x14]`
reads `BG.drawmode` (BG[5]). But this is the BAR blit — so the drawmode
in struct +0x00 for the BAR blit is **actually `BG.drawmode = 0`**, not
`Bar.drawmode`. This contradicts my §6.B decoding above. Let me restate:

> **Correction to §6.B (drawmode):** The Bar blit's `dp.drawmode` is
> `BG.drawmode = 0`, NOT `Bar.drawmode`. The asm at `0x448763, 0x44876a`
> proves it. The Bar bitmap's own `drawmode = 0x20000` is therefore
> NOT read at the call site — the engine relies on the source bitmap's
> own flags (`BM_ALPHA`) and/or surface state for blend mode. The
> SDrawParam `drawmode = 0` likely falls through to "use source's
> default" behavior in `FUN_004bd490_Blit` (and `BM_ALPHA` sources
> automatically pick alpha blend per UI_METHOD_MAP §2 step 3
> auto-alpha).

This is a fine-grained correction and changes the port-side decision:
pass `0` (or "default") for the drawmode of the bar blit and let the
renderer pick alpha from the bitmap header. The visual is unchanged
because BM_ALPHA bitmaps blend correctly under the auto-alpha path
(UI_METHOD_MAP §2 step 3).

