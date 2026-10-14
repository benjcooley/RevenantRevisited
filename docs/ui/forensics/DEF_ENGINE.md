# Retail DEF widget engine: how every widget's draw args are computed

Forensics for the port's `TDefPane` (src/defpane.cpp). Ground truth is the retail
binary run in the emulator (`recon/retail_asm/baseline/Revenant.rebuilt.exe`,
`tools/retail_runtime/slots/hud/options.py`), plus capstone disassembly of the
same memory image. Every claim cites an instruction address. Ghidra decomps were
used only to find structure layouts, and each one was re-checked against the
disassembly. Anything not confirmed is marked **UNCONFIRMED**.

Data used: `data/Resources/options.def` (the loose 1.22 copy, with toggles at
y=60/80/…/180 and "No Combat Results", which is what retail loads and what the
fixture draws), plus `widgets.def`, `savegame.def`, `loadgame.def`,
`ingamemenu.def` and `popup.def` from `RevenantRevisited/data/resources_unzipped/`.

The primitive traces this cites (every Put, PutSubrect, ParamBlit and Text of
a frame, with their call chains) came from scratch scripts over the fixture;
`tools/retail_ab/hud_ab.py options` reproduces the frames and compares the
port with them. The controller list's input and the key rebind are
[OPTIONS_CONTROLS.md](../../gameflow/forensics/OPTIONS_CONTROLS.md).

---

## 0. The text-compose call every widget makes

`0x004be2b0` is `__thiscall`. `this` is the destination surface, and the
arguments are pushed right to left:

`Text(x, y, w, h, char* text, int startLine, int fontId, COLOR colour, uint flags, uint blitMode)`

- **startLine** is always 0 from the DEF engine: pushed `0` at 0x42b968 (TEXT), 0x42cf7f (BUTTON),
  0x42d015 (BUTTON fallback), 0x42f6a8 (LIST field), 0x430fbb (LIST item),
  0x432855 (EDIT) and 0x43430e (DROPLIST).
- **colour** is a 4-byte struct (0x00RRGGBB) copied into the argument slot by
  `0x00419dd0` (`mov edx,[ecx]; mov [eax],edx`, which is a plain copy). That is
  why every call site pushes a dummy dword, then does `mov ecx,esp` and calls 0x419dd0.
  No DEF style can produce −1: every default is 0xFFFFFF (see the parsers below).
  Only a FIELD getter could write −1. **UNCONFIRMED** whether any getter does.
- **fontId** is the FONT.DEF index, 1-based, looked up by name through the font
  manager: `0x004acb30` with `this=0x0065b010`. The lookup is case-insensitive.
  Measured in the emulator: System 1, Dialog 2, Scroll 3, Book 4, Numbers 5,
  Small 6, **Med 7**, Large 8, Gold 9, GoldMed 10, SpellTitle 11, Version 12; an
  unknown name returns 0. On a 0 return every parser keeps the default
  `DAT_0065caec`, which is 7 ("Med") at runtime. Examples: 0x42a08f-0x42a093
  (labels), the TEXT parser (`cls_0x42b340` decomp), and 0x42f8c5-0x42f8d0 (FIELD).
- **blitMode** per the decomp (`FUN_004be2b0_CompositeBlit.cpp:149-153`): if bit 31
  (DM_USEDEFAULT) is set, the compose uses the font's own drawmode
  `| (mode & 0x7fffffff)`; otherwise it uses the mode as given. **UNCONFIRMED against
  the disassembly.** The value only matters for BMFONTs; "Med" is a WINFONT.

## 1. Engine skeleton

### 1.1 Widget base fields
Taken from `FUN_0042a350`, the base parse; offsets are confirmed by the draw code below.

| off | field |
|---|---|
| +0x08 | owning pane (draw skipped if 0) |
| +0x10 | type id (1 BITMAP … 8 DROPLIST) |
| +0x14 | control flags (`FLAGS` on the widget line) |
| +0x18 | NAME[32] |
| +0x38 | FIELD[32] |
| +0x58 | INDEX |
| +0x5c | TEXT (heap string) |
| +0x60/+0x64/+0x68/+0x6c | x, y, w, h (init −10000) |
| +0x70 | HOTKEY |
| +0x74 | GROUP |
| +0x78 | parent widget (a list's child scrollbar points here) |

The base parser takes **POS, NAME, FIELD, INDEX, TEXT, STRINGREF, FLAGS, GROUP,
HOTKEY, in that fixed order**, each optional (`FUN_0042a350`). The subclass
attribute parser then consumes the rest of the line, again in a fixed order.
This is why a widget line can carry two `FLAGS`:
- the first one (after TEXT) is the **control word** (+0x14);
- the one after `FONT "…"` belongs to the style and replaces the **text flags**.

Example: options.def `name0`, `FLAGS CTRLFLAG_CLEARBG FONT "Med" FLAGS TEXT_LEFT | TEXT_VCENTER`,
gives ctrl 0x1000 and text flags 0x41. The live dump (`widgets_dump.py`)
shows `flags=00001000 … flags=0x41`.

How a widget line uses its style (all `FUN_00…_ctor` decomps, confirmed live):
- The type's global style object is copied into the widget, then the remaining
  tokens of the widget line are parsed **on top of the copy** as overrides.
- A `STYLE` line re-parses the global object **from defaults (param 0)**: see the
  FRAME registration 0x42b2c0, `push 0` → 0x42aed0. So popup.def's `STYLE TEXT`/`STYLE BUTTON`
  fully replace the widgets.def ones; they are not merged.
- **UNCONFIRMED**: that the same holds for the TEXT/BUTTON STYLE registrations,
  and whether widgets.def is re-parsed on every DefScreen_Open, which would restore the
  defaults after a popup.

### 1.2 The pane draw, `0x00435de0`
This is the button pane's slot 20; Options' slot 21 `0x004451e0` is just `ret 8`.

1. If the pane is dirty (+0x50) or rebuilding (+0x150), the background surface
   (+0x84) is blitted to the draw surface at (0,0, full size): `0x438df0` at 0x435ee7,
   observed as the two `ParamBlit [0,0,640,480]` calls.
2. The widgets are walked in DEF order (array +0x98, count +0x88), 0x435f0c..0x435f4e. A widget is
   **skipped** if any of these holds:
   - its flags have **HIDDEN (2)**;
   - its pane (+8) is 0;
   - its flags have **DRAWTOBG (0x800)**: those are drawn into the background
     surface when it is rebuilt (0x435e8e..0x435ebd);
   - it is the pane's topmost widget (+0xa8).

   Otherwise it is drawn (`vtable+0x40`, `Draw(surface)`) if the widget is DIRTY (0x20) or
   the pane is dirty.
3. The topmost widget (+0xa8, e.g. an open droplist) is drawn **last**
   (0x435f50..0x435f7a).

Per-type Draw entries are in vtable slot 16 (+0x40):

| type | Draw | slot 17 (+0x44) DrawBackground |
|---|---|---|
| TEXT | 0x42b700 | – |
| BUTTON | 0x42cc30 | – |
| SCROLLBAR | 0x42ede0 | 0x42eb30 |
| LISTBOX | 0x431410 | 0x431160 |
| EDIT | 0x4328b0 | 0x4323b0 |
| BITMAP | 0x42ad10 | – |
| FRAME | 0x42b150 | – |
| DROPLIST | 0x4347d0 | 0x434500 |

### 1.3 Shared rules
- **Background restore.** Before drawing, a widget whose art is transparent re-blits
  its rect from the pane background surface (+0x84) with `ParamBlit`; with no background
  surface it uses `Box(…,0,…,0x20)` instead.
  - Rect: the widget rect expanded by its MARGINS, except TEXT, which grows by them only with
    TEXTFLAG_FRAME (0x0042b725; observed: `name0` restores (70,224,180,20) though the TEXT style has
    MARGINS 0 0 4 4).
  - Trigger: TEXT/BITMAP/FRAME do it when CTRLFLAG_CLEARBG (0x1000) is set; BUTTON when
    its resolved drawmode has 0x2100; SCROLLBAR/LISTBOX/EDIT when theirs has 0xb100
    or they have no bitmap.
  - This is invisible in a compose-from-scratch renderer.
  - Afterwards every Draw calls `surface->vt+0x20(x,y,w,h)` (the update rect) and clears DIRTY.
- **The restore is visible when widgets overlap.** It isn't only an
  invalidation: it erases whatever earlier widgets drew in the rect. On the
  Options screen the `name0..name7` TEXT widgets (CLEARBG, MARGINS 0 0 4 4) come
  after the controller list and erase its frame's inner line under the names
  column (x 70..253); on Save/Load the CLEARBG "gamename" TEXT erases the edge of
  the ClearFrame2 FRAME under it. The port reproduces it
  (`TDefPane::RestoreBackground`: the rect cleared, then the pane background's
  subrect).
- **A child scrollbar's restore is a no-op.** The list's scrollbar has no art:
  its DrawBackground restores the bar rect and redraws the parent list's
  DrawBackground into it, clipped (0x0042ebf3-0x0042ec15), which puts back what
  was there. The port skips both.
- **DM_USEDEFAULT (0x80000000) resolution for sprite blits.**
  - Frame/button: `(bitmap->drawmode & ~0x20010) | 0x20`, i.e. drop USEREG and BACKGROUND,
    add NORESTORE (0x42cd22-0x42cd30, 0x429b74-0x429b81). The alpha widget art
    (drawmode 0x22000) therefore blits with **0x2020**, as observed on every Put/PutSubrect.
  - List/scrollbar/edit backgrounds: `bm->drawmode & ~0x20000` (0x43121f, 0x42ebe9).
  - Scrollbar/spin parts: the per-part default is `(upBm->drawmode & ~0x20000) | 0x20`
    (EDIT parser decomp line 201; the parts show mode 0x2020 live, `+0xec/+0x108/+0x124 = 0x2020`).

### 1.4 The frame tiler, `0x00429ac0`
Signature: `__thiscall(frame, surface, bitmap, x, y, w, h, drawmode)`, `ret 0x1c`.

`frame` is a 9-dword struct: +0 NOCENTER, +4/+8/+0xc/+0x10 = FRAME l,t,r,b,
+0x14..+0x20 = MARGINS ml,mt,mr,mb (from `FRAME %i %i %i %i <MARGINS %i %i %i %i> <NOCENTER>`, 0x5ccf24).

```
X0 = x - ml          W = w + ml + mr        (0x429ae3-0x429b07)
Y0 = y - mt          H = h + mt + mb
mw = bm.w - l - r    mh = bm.h - t - b      (centre source size)
dm = (dm == USEDEFAULT ? bm.dm : dm) & ~0x20010 | 0x20      (0x429b68-0x429b81)
top row    (t>0): PutSubrect(X0,Y0, bm, 0,0,l,t)                       0x429ba8
                  for xx=X0+l; xx<X0+W-r; xx+=cw: cw=min(mw, X0+W-r-xx)
                      PutSubrect(xx,Y0, bm, l,0,cw,t)                  0x429c19
                  PutSubrect(X0+W-r,Y0, bm, l+mw,0,r,t)                0x429c5d
middle rows: for yy=Y0+t; yy<Y0+H-b; yy+=ch: ch=min(mh, Y0+H-b-yy)
                  left  PutSubrect(X0,yy, bm, 0,t,l,ch)                0x429cd8
                  centre (only if NOCENTER==0) tiles (l,t,cw,ch)       0x429d07 test, 0x429da0
                  right PutSubrect(X0+W-r,yy, bm, l+mw,t,r,ch)         0x429e50
bottom row (b>0): same as the top row with source y = t+mh               0x429ea2/0x429f21/0x429f68
```

**Retail drops alpha blits narrower than 4 px.** The 2D alpha blitter
(0x004b3510) returns without drawing when the blit is under 4 px wide
(0x004b3539: `cmp [param+0x30], 4; jl 0x4b3780`); height doesn't matter
(measured in the emulator: widths 1-3 draw nothing at any source x, 4 and up
draw). The tiler's last tile is often that narrow: the Options list
(VScrollRect, centre 40 px, run 482 px) ends each band with a 2 px tile at x 548,
so retail's frame has a 2 px gap in its top and bottom edges there. This is a
renderer limit of retail's, like the pink fringe (HUD_REBUILD.md §3): the port
draws the tile, and the A/B reports those pixels as the known retail bug
`narrow_alpha` (the fixture lists the dropped blits,
`primitives.narrow_alpha_drops`) instead of comparing them. From the game the
pane composes into an ARGB4444 texture (drawn by slot 23's overlay quad), and
those narrow tiles land: the from-game A/B shows no difference there.

In short: **the edges and centre are tiled, never stretched**; the last tile is
clipped; the frame is **drawn MARGINS outside the widget rect** (the art carries
its 4 px drop shadow on the right and bottom); NOCENTER leaves the centre unpainted.

Verified on the OK button. ClearFrame2 is 64×57 with FRAME 10 10 12 12 and MARGINS 0 0 4 4, so
W=104, H=34, and the tiles are x 180/190(42)/232(40)/272 and y 410/420(12)/432 (opts_default.txt).

---

## 2. TEXT, Draw `0x0042b700`

Style object, 76 bytes at widget +0x90 (TEXT parser 0x42b340, format strings
0x5cd174 `BGBITMAP`, 0x5ccf24 frame, 0x5cd188 `DRAWMODE`, and 0x5cd1ac
`FONT %30s <RECT %i %i %i %i> <COLOR %b %b %b> <FLAGS %d> <DRAWMODE %d>`):

- +0x90 BGBITMAP; +0x94 frame struct (NOCENTER, FRAME +0x98.., MARGINS +0xa8..+0xb4); +0xb8 frame drawmode
- +0xbc font; +0xc0..+0xcc RECT l t r b
- +0xd0 **text flags** (default 0x401); +0xd4 colour (default 0xFFFFFF)
- +0xd8 text drawmode (default 0x80000000)

**Control-flag → TEXT flag conversion**, done once in the ctor (`FUN_0042b5d0`
decomp; the live dump confirms 0x444 for the slider labels):
```
if (ctrl & TEXTFLAG_CENTER 0x40000)      f = f & ~(LEFT|RIGHT)  | CENTER   // &~5 |2
else if (ctrl & TEXTFLAG_RIGHT 0x80000)  f = f & ~(LEFT|CENTER) | RIGHT    // &~3 |4
if (ctrl & TEXTFLAG_TOP 0x100000)        f = f & ~(BOTTOM|VCENTER) | TOP   // &~0x60 |0x10
else if (ctrl & TEXTFLAG_BOTTOM 0x200000) f = f & ~(TOP|VCENTER) | BOTTOM  // &~0x50 |0x20
```
- No other control bit reaches the text flags. A control-level `TEXT_SHADOW` (0x400)
  in the widget's first FLAGS does nothing to the text.
- "Default is vcenter" in widgets.def only describes the style default 0x441.
- **TEXTFLAG_FIELDCLR (0x400000)** is used only in the field refresh (slot 4,
  0x42ba80): when the pane's getter returns a colour, it is stored permanently
  into +0xd4 (0x42bac9-0x42bad6).

**The Text call**, 0x42b8fd-0x42b976:
```
Text(x + R.l, y + R.t, w - R.l - R.r, h - R.t - R.b,
     buf, 0, font[+0xbc], colour, flags[+0xd0], drawmode[+0xd8] (raw, not converted))
```
- `buf`: if FIELD[0] is set, it comes from the pane getter `pane->vt[+0xa4](field, buf, 0x200, &colour, 0)`,
  which may replace the colour for this draw (0x42b8b2-0x42b8cc). Otherwise it is `sprintf("%s", text)`
  (0x42b8d4-0x42b8e9).
- Nothing is drawn if `buf` is empty (0x42b8f5).
- **TEXTFLAG_FRAME (0x10000)**: the BGBITMAP frame is tiled first with
  `0x429ac0(+0x94, surface, bm, x, y, w, h, frameDM)` (0x42b848-0x42b87d). Without the flag the
  style's ClearFrame3 is never drawn.

## 3. BUTTON, Draw `0x0042cc30`

**Style choice** in the ctor `FUN_0042c6a0`, by the **draw** bits, not the behaviour bits:
- BTNFLAG_DRAWRADIO 0x200000 → RADIO style `0x6562d8`;
- else DRAWTOGGLE 0x400000 → TOGGLE `0x656220`;
- else NORMAL `0x655910`.

GROUP ≥ 0 forces BTNFLAG_RADIO, and RADIO forces BTNFLAG_TOGGLE.

Style object, 0x2b dwords at +0x9c. Keywords in order, from `cls_0x42bd90`: UP DOWN SEL GRAY FRAME DRAWMODE
UPLABEL DOWNLABEL SELLABEL GRAYLABEL UPLABELRECT DOWNLABELRECT.

| field | widget offset |
|---|---|
| UP / DOWN / SEL / GRAY bitmaps | +0x9c / +0xa0 / +0xa4 / +0xa8 |
| has-FRAME flag | +0xac |
| frame struct | +0xb0 (NOCENTER), FRAME +0xb4.., MARGINS +0xc4..+0xd0 |
| DRAWMODE | +0xd4 (default 0x80000000) |
| has-label flags (UP / DOWN / SEL / GRAY) | +0xd8 / +0xdc / +0xe0 / +0xe4 |
| label structs, 16 B each | up +0xe8, down +0xf8, sel +0x108, gray +0x118 |
| UPLABELRECT | +0x128..+0x134 |
| DOWNLABELRECT | +0x138..+0x144 |

Label details:
- Each label struct is font, colour, flags, drawmode, parsed by 0x429fd0 with
  `%20s <COLOR %b %b %b> <FLAGS %d> <DRAWMODE %d>` (0x5ccf70).
  Defaults: font 7, white, flags 1, drawmode 0x80000000 (0x429ffa-0x42a030).
- In a STYLE (no base) DOWN/SEL/GRAY labels start as a copy of UPLABEL.
- `UPLABELRECT` also copies itself into DOWNLABELRECT.
- Live values for widgets.def NORMAL: up {7, 0xffffff, 0x4c2}, down {7, 0, 0xc2}.
  TOGGLE: up = down = {7, 0xffffff, 0x401}, rects (18,−1,0,0).

**State selection**, 0x42cc74-0x42cd0d. The first match wins, and it picks the bitmap,
the label struct and whether a label is drawn at all:
1. `ctrl & BTNFLAG_DOWN 0x10000` and a DOWN bitmap → DOWN bitmap, down label (if DOWNLABEL);
2. `ctrl & CTRLFLAG_SELECTED 8` and a SEL bitmap → SEL bitmap, sel label (if SELLABEL);
3. `ctrl & CTRLFLAG_DISABLED 4` and a GRAY bitmap → GRAY bitmap, gray label (if GRAYLABEL);
4. otherwise UP bitmap, up label (if UPLABEL).

BTNFLAG_DOWN is the checked state of a toggle (live: checked toggles have ctrl 0x450000)
and the held state of a push button (fixture `_down`: OK held →
`Text [180,410,100,30] 'OK' col 0x0 fl 0xc2`). widgets.def defines no SEL/GRAY, so
disabled buttons draw as UP: "Enhanced" has ctrl 0x450004 and draws normally.

**Face**:
- drawmode = `+0xd4`, or, if DM_USEDEFAULT, `(bm.dm & ~0x20010) | 0x20` (0x42cd11-0x42cd30).
- With a FRAME (+0xac≠0), tile `0x429ac0(+0xb0, surf, bm, x, y, w, h, dm)` (0x42ce96).
- **Without one, a single `Put(x, y, bm, dm, 0)` at native size** (0x42ceb3). The decision
  is by style, not by "toggle". widgets.def TOGGLE/RADIO have no FRAME, so they Put; NORMAL tiles.

**Label rect**, chosen by `ctrl & BTNFLAG_DOWN` only (0x42cec4-0x42cf3b):
```
L = (ctrl & 0x10000) ? DOWNLABELRECT : UPLABELRECT        // l t r b insets
x' = x + L.l   y' = y + L.t   w' = w - L.l - L.r   h' = h - L.t - L.b
Text(x', y', w', h', text[+0x5c], 0, label.font, label.colour, label.flags,
     ((label.dm == USEDEFAULT) ? 0 : label.dm) & ~0x10 | 0x20)        // 0x42cf3d-0x42cf86
```
The label is skipped when that state has no label (0x42ceb8-0x42cebe). Normal buttons therefore
get blit mode **0x20**.

**No-UP-bitmap fallback** (0x42cfb3-0x42d02b):
- a procedural bevel `0x42bbb0(surf, x, y, w, h, down)`;
- then `Text(x+d, y+d, w−d, h−d, NAME(+0x18), 0, font [0x65abc4]=6 "Small", colour from 0x429950, 0x442, 0x20)`
  with d = 2 when down.

No in-scope DEF uses this path.

## 4. SCROLLBAR, Draw `0x0042ede0`

**Style**:
- ctor `FUN_0042df00` / attr parser 0x42dfe0: `ctrl & SCROLLFLAG_HSCROLL 0x20000` →
  HSCROLL `0x655d38`, else **VSCROLL** `0x6560f8`.
- Orientation in the layout is taken from `ctrl & 0x10000` (VSCROLL); without it the bar
  is horizontal (0x42e2e6).
- Fields: value +0x94, min +0x98, max +0x9c (default 1000), page +0xa0 (default 100);
  style at +0xa4 (0x21 dwords).
- Style offsets: BGBITMAP +0xa4, has-FRAME +0xa8, frame struct +0xac (FRAME +0xb0.., MARGINS +0xc0..+0xcc),
  drawmode +0xd0.
- Parts, 7 dwords each `{U bm, D bm, a, b, relR/flag, relB, drawmode}`: UP +0xd4, DOWN +0xf0, THUMB +0x10c.
- Part syntax:
  - `UP|DOWN %78s <RELR> %i <RELB> %i` (0x5cd328/0x5cd36c): the bitmaps are `name+"U"` and `name+"D"`
    (0x5cd348/0x5cd34c).
  - `THUMB %78s %i %i <RELBR> %i` (0x5cd3b8).

**Size override** (0x42e02a-0x42e0bf). If there is a BGBITMAP:
- an omitted POS takes −reg, and an omitted size takes the bitmap size;
- otherwise a **VSCROLL forces w = bm.w − ml − mr** and an **HSCROLL forces h = bm.h − mt − mb**.

HScrollBar is 74×21 with MARGINS 0 0 4 4, so **h = 17** regardless of the DEF's 20 (live:
`pos=(420,70,160,17)`).

**Layout** (0x42e240-0x42e3c5):
```
up.x   = x + (up.relR ? w - up.a : up.a)       up.y   = y + (up.relB ? h - up.b : up.b)    (+0x140/+0x144)
down.x = x + (dn.relR ? w - dn.a : dn.a)       down.y = y + (dn.relB ? h - dn.b : dn.b)    (+0x148/+0x14c)
VSCROLL: trackStart = y + th.a ; trackEnd = y + h - th.b ;  thumb.y = trackStart ;
         thumb.x = x + (th.rel ? w - th.c : th.c)
HSCROLL: trackStart = x + th.a ; trackEnd = x + w - th.b ;  thumb.x = trackStart ;
         thumb.y = y + (th.rel ? h - th.c : th.c)                                         (+0x150/+0x154/+0x158/+0x15c)
```

**SetValue** (0x42e440): the value is clamped to [min,max], then
`pos = trackStart + (trackEnd − trackStart) * value / (max − min)`. Note that it uses
**value, not value − min** (0x42e478-0x42e49e); `pos = trackStart` when max = min.

**Draw**:
1. DrawBackground `0x42eb30(surf, −10000…)` (0x42ee14):
   - If a parent is set (+0x78) and `ctrl & 0x400`, it delegates to the parent's
     DrawBackground over the bar rect (0x42ebf3-0x42ec15).
   - Otherwise it restores, clips to the bar rect (+margins) (0x42ed28-0x42ed41), then either
     tiles the BGBITMAP `0x429ac0(+0xac, surf, bm, x, y, w, h, dm)` (0x42ed75) or does a single
     `Put(x, y, bm, dm|0x20, 0)` without a FRAME (0x42ed91).
2. Up arrow 0x42e7d0: `Put(up.x, up.y, (ctrl & 0x80000) ? upD : upU, up.dm, 0)` (0x42e869).
3. Down arrow 0x42e980: `Put(down.x, down.y, (ctrl & 0x100000) ? dnD : dnU, dn.dm, 0)` (0x42ea19).
4. Thumb 0x42e620: drawn only if max − min ≥ 1 and there is a thumb bitmap.
   `Put(thumb.x, thumb.y, (ctrl & 0x400000) ? thD : thU, th.dm, 0)` (0x42e6d0).
   The thumb is **fixed size, a single Put, no stretching**.

No text.

**Options slider**, POS 420 70 160 20 with HSCROLL style. The live draw:
- 18 PutSubrects of HScrollBar, tiled over (420,70,164,21);
- `Put(421,71) HScrollUpU`, `Put(568,71) HScrollDownU` (RELR 12 → 420+160−12);
- `Put(550,72) HScrollThumbU`: track 439..550, value = max, cross offset 2.

## 5. LISTBOX, Draw `0x00431410`

**Style** (ctor `FUN_00430090`):
- (VLIST 0x10000 and not NOSCROLL 0x40000) → VLIST style `0x655b48`;
- else (HLIST 0x20000 and not NOSCROLL) → HLIST `0x655c40`;
- else a third style `0x6559c0`. widgets.def defines no third style, so its contents
  are **UNCONFIRMED** (presumably defaults: no bitmap, no scrollbar).

Parser `0x42f9f0` keywords, in order: BGBITMAP, FRAME, DRAWMODE, `RECT %i %i %i %i`,
`ITEM %i %i`, `SELCOLOR %b %b %b`,
`FONT %30s <COLOR> <FLAGS> <DRAWMODE>`, `SCROLLBAR <RELRB> %i %i %i %i` followed by the
scrollbar part grammar (strings 0x5cd51c..0x5cd5ec).

Fields, live dump of the options list:

| field | offset | options value |
|---|---|---|
| format list | +0x90 | |
| count | +0x98 | 138 |
| top item | +0x9c | |
| selected | +0xa0 | −1 |
| visible = rows*cols | +0xa4 | 16 |
| rows | +0xa8 | 8 |
| cols | +0xac | 2 |
| itemw / itemh | +0xb0 / +0xb4 | 150 / 20 |
| content x, y, w, h | +0xb8..+0xc4 | 245, 225, 300, 160 |
| child scrollbar | +0xd4 | |
| BGBITMAP | +0xd8 | |
| has-FRAME | +0xdc | |
| frame struct | +0xe0 | NOCENTER 1, 8 26 26 26, 0 0 4 4 |
| drawmode | +0x104 | |
| ITEM | +0x108/+0x10c | |
| RECT | +0x110..+0x11c | 185 5 23 5 |
| SELCOLOR | +0x120 | |
| font | +0x124 | 7 |
| text flags | +0x128 | 0x4c2 |
| colour | +0x12c | 0xffffff |
| text drawmode | +0x130 | |
| has-SCROLLBAR | +0x134 | |
| RELRB values | +0x138..+0x144 | 21 4 5 4 |
| RELRB flag | +0x148 | |
| child style | +0x14c.. | |

**Layout** (0x4308b0):
```
cx = x + R.l ; cy = y + R.t ; cw0 = w - R.l - R.r ; ch0 = h - R.t - R.b
itemw = min(cw0, ITEM.w) ; itemh = min(ch0, ITEM.h)
cols = max(1, cw0 / itemw) ; rows = max(1, ch0 / itemh)
content = (cx, cy, cols*itemw, rows*itemh) ; visible = rows*cols
```
widgets.def's `ITEM 1000 16` therefore gives one full-width column. Options' `ITEM 150 20`
with cw0 = 512−185−23 = 304 gives **2 columns** at x 245 and 395.

**Child scrollbar rect** (0x4304a0), for VLIST:
- `sx = x + (rel ? w − a : a)`, `sy = y + b`, `sw = (x + (rel ? w − c : c)) − sx`, `sh = h − d − b`;
- flags `0x10000 | (list & 6) | 0x400`.
- HLIST is the transpose (0x430564-0x4305c8).

Options gives (551,224,16,162). The child gets the list's SCROLLBAR sub-style, which has
**no BGBITMAP**, so its background is the parent list's DrawBackground over that rect.
That is the second VScrollRect PutSubrect pass in the trace, clipped by 0x431371.

**Scroll range**, SetCount 0x430c50 for VLIST: `max = max(0, (count − visible + cols − 1) / cols)`,
counted in **rows** (0x430cc9-0x430d12); the value is the top **row**.
- Live: options max = (138−16+1)/2 = 61.
- One down-arrow click puts the thumb at 243 + 113·1/61 = **244**, three clicks at **248**
  (callers_scroll1/3).
- The top item is `row*cols` (0x430b28-0x430b41).

**Draw**:
1. DrawBackground `0x431160(surf, −10000…)` (0x431444): restore (drawmode & 0xb100), then
   tile the BGBITMAP `0x429ac0(+0xe0, surf, bm, x, y, w, h, dm)` (0x4313a5), or Put without a FRAME.
   **CTRLFLAG_CLEARBG does not suppress this**: the options list (ctrl 0x91000) draws
   VScrollRect over (60,220,516,174), border only because of NOCENTER.
2. The child scrollbar's Draw (0x431454).
3. DrawItems `0x431000(surf, 1)`, which walks `i = top … min(top+visible, count)−1` (0x43104b-0x43107b).
   For each item, DrawItem `0x430db0(surf, i, 1)`:
```
k = i - top
VLIST: col = k % cols ; row = k / cols        (row-major)        0x430e0e-0x430e45
HLIST: col = k / rows ; row = k % rows        (column-major)     0x430e49-0x430e80
X = cx + col*itemw ; Y = cy + row*itemh
if (i == selected && !(ctrl & LISTFLAG_VIEWONLY 0x200000))
    Box(X, Y, itemw, itemh, surface.ColorToPixel(SELCOLOR), …, 0x20)       0x430e99-0x430ee2
if (ctrl & LISTFLAG_USEFORMAT 0x80000)
    FormatDraw(fmt[+0x90], pane, surf, X, Y, itemw, itemh, i, i==selected)  0x430f43
else if (FIELD[0])   // the LIST's own FIELD name
    colour = +0x12c ; pane->vt[+0xa4](FIELD, buf, 0x80, &colour, i)
    Text(X, Y, itemw, itemh, buf, 0, font+0x124, colour, flags+0x128,
         ((dm+0x130 == USEDEFAULT) ? 0 : dm) | 0x20)                         0x430f4d-0x430fc2
```
The selection is **only that item's cell**. Selected text keeps its colour. Live (`_sel`):
`Box [395,245,150,20] col 0x323` (RGB565 of 0,100,30).
`0x4384e0` converts 0x00RRGGBB to the surface's 555/565/4444 pixel.

**Row format.** A `BEGIN … END` block is parsed by `0x42f700` with defaults =
the list's font, colour, flags and drawmode (call at 0x4301d1-0x430217).
- `FIELD|TEXT %i %i %i %i <FONT %20s> <FLAGS %d> <COLOR %b %b %b> <DRAWMODE %d> %40s` (0x5cd488)
  → item {type FIELD 1 / TEXT 2, x, y, w, h, font, flags, colour, `(dm==USEDEFAULT?0:dm)|0x20`, string}.
- `BITMAP %i %i %40s <DRAWMODE %d>` (0x5cd434) → type 0.
- Explicit FONT, FLAGS and COLOR **replace** the defaults; they are not OR'd.

**FormatDraw**, `0x0042f570` (cdecl, 9 args):
```
BITMAP: bm = pane->vt[+0xa0](name, i) ; Put(X+f.x, Y+f.y, bm, (f.dm==USEDEFAULT?bm.dm:f.dm)|0x20, 0)
FIELD/TEXT: colour = f.colour ; text = f.string ;
            FIELD: pane->vt[+0xa4](f.string, buf, 0x80, &colour, i), text = buf
            w' = min(f.x + f.w, itemw) - f.x ; h' = min(f.y + f.h, itemh) - f.y      (clipped to the cell)
            if text[0]: Text(X+f.x, Y+f.y, w', h', text, 0, f.font, colour, f.flags, f.dm)   0x42f650-0x42f6c9
```
The `selected` argument is unused.

**The Options controller list**:
- FIELD `0 0 150 20 FONT "Med" FLAGS TEXT_SHADOW|TEXT_CENTER COLOR 255 255 100 "key"`.
  Every key cell is `Text(245|395, 225+20r, 150, 20, key, 0, 7, 0xFFFF64, 0x402, 0x20)`, which is
  the observed call.
- Items are 2 per control (key 1, key 2): 69 controls give count 138.
- **The control names are NOT list cells.** They are the `name0..name7` TEXT widgets, whose
  text the Options pane sets to the names of the top 8 rows ("Invoke 1"…). After one scroll
  they read "Invoke 2"… (callers_scroll1).
- The headers "Control Name", "Key 1", "Key 2" are ordinary TEXT widgets.

## 6. EDIT, Draw `0x004328b0`

**Style**: EDITFLAG_SPIN 0x40000 → SPIN `0x655ab8`, else NORM `0x656180` (ctor `FUN_00431ca0`). MAXLEN
defaults to 20 and the text is truncated to it. Parser 0x431750 keywords, in order
(strings 0x5cd65c..0x5cd718): `BGBITMAP %78s <RECT %d %d %d %d>`, FRAME, DRAWMODE,
`FONT %30s <RECT> <COLOR> <EDITCOLOR> <FLAGS> <DRAWMODE>`, `SPIN UP %78s <RELR> %i %i DOWN %78s <RELR> %i %i <DRAWMODE>`.

Fields (style at +0xb8):

| field | offset |
|---|---|
| BGBITMAP | +0xb8 |
| **outer RECT** (after BGBITMAP) | +0xbc..+0xc8 |
| has-FRAME | +0xcc |
| frame struct | +0xd0 |
| drawmode | +0xf4 |
| font | +0xf8 |
| **text RECT** (after FONT) | +0xfc..+0x108 |
| flags | +0x10c (default 1) |
| colour | +0x110, copied to +0xa0 |
| EDITCOLOR | +0x114 (default 0xFFFF00) |
| text drawmode | +0x118 |

Draw sequence:
1. **Background** 0x4323b0: the frame is drawn **only if** there is a BGBITMAP, **not EDITFLAG_NOFRAME 0x10000,
   not CTRLFLAG_DISABLED 4** (0x4325c4-0x4325e0). It tiles over the rect inset by the outer RECT:
   `0x429ac0(+0xd0, surf, bm, x+O.l, y+O.t, w−O.l−O.r, h−O.t−O.b, dm)` (0x4325e9-0x432634).
2. **Text** 0x4326e0:
```
buf = text ; if editing(+0x90) && blink(+0x9c) < 24: strcat(buf, "|")      0x43276c-0x432784, caret 0x5cd790 = "|"
colour = editing ? EDITCOLOR(+0x114) : +0xa0
x' = x + O.l + T.l ; y' = y + O.t + T.t
w' = w - O.l - O.r - T.l - T.r ; h' = h - O.t - O.b - T.t - T.b                       0x4327a0-0x4327fa
flags = +0x10c ; EDITFLAG_CENTER 0x200000 → &~5|2 ; else EDITFLAG_RIGHT 0x400000 → &~3|4   0x4327fc-0x43281e
if buf[0]: Text(x', y', w', h', buf, 0, font+0xf8, colour, flags, dm+0x118 raw)          0x43285e
```
   **UNCONFIRMED**: that +0x90 means "editing" (the EDITCOLOR switch implies it) and the blink
   period of +0x9c. The threshold 24 is confirmed.
3. **Spin buttons** 0x432030/0x4321f0: `Put(+0xb0,+0xb4, pressed? upD:upU, +0x144, 0)` and the
   matching down button at +0xa8/+0xac. The positions are `x + (relR ? w − a : a)`, `y + b` (ctor).

Savegame `nameedit` (POS 372 89 208 20, NORM, no outer RECT, FONT RECT 4 2 6 2), derived:
- frame tiles over (372,89,212,24), border only;
- `Text(376, 91, 198, 16, "New Game", 0, 7, 0xFFFFFF, 0x401, 0x80000000)`;
- while editing, colour 0xFFFF00 with "|" appended in the on-phase.

## 7. FRAME, BITMAP, DROPLIST

**FRAME** (Draw 0x42b150; parser 0x42aed0 keywords `BITMAP`, `FRAME`, `DRAWMODE`):
- The rect is expanded by the margins; it is restored only with CLEARBG.
- Then `0x429ac0(+0x94, surf, bm+0x90, x, y, w, h, dm+0xb8)` (0x42b28f). No text.
- A widget-line `BITMAP "ClearFrame2"` replaces only the bitmap; the style's FRAME 14 14 16 16 /
  MARGINS / NOCENTER stay.

**BITMAP** (Draw 0x42ad10):
- `bm = +0x90`, or, when that is null and FIELD is set, `pane->vt[+0xa0](FIELD, −1)` (0x42ad2f-0x42ad53).
- Restore only with CLEARBG.
- **A single `Put(x, y, bm, dm+0x94 raw, 0)` at native size, no stretch** (0x42ae38).

**DROPLIST** (Draw 0x4347d0 → 0x4341a0 current item, 0x434350 button). No in-scope screen uses it.
- The current item `+0xc0` is drawn when 0 ≤ cur < count(+0xbc), in the item rect (+0xa4,+0xa8,+0xac,+0xb0).
- `DROPFLAG_DRAWSEL 0x20000` adds `Box(rect, pixel(SELCOLOR+0x18c), 0x20)`.
- DROPFLAG_USEFORMAT 0x40000 → FormatDraw. Otherwise, if FIELD is set, the getter text goes through
  `Text(rect, buf, 0, font+0x190, colour(+0x198 or getter), flags+0x194, (dm+0x19c→0)|0x20)` (0x4342a5-0x434317).
- The derivation of the item rect from ITEMRECT is **UNCONFIRMED**.

---

## 8. The observed Options calls, explained

| observed | widget / DEF | derivation |
|---|---|---|
| `Put(55,60) 16×17 0x2020` then `Text (73,59,232,21) 0x401 white f7 bm 0x20` | BUTTON POS 55 60 250 20, BTNFLAG_TOGGLE\|DRAWTOGGLE → TOGGLE style | No FRAME, so a native Put of CheckU/CheckD (CheckD when checked, BTNFLAG_DOWN). Label rect = (55+18, 60−1, 250−18−0, 20−(−1)−0); label flags TEXT_LEFT\|TEXT_SHADOW = 0x401, unchanged by the control word; blit 0x80000000→0, \|0x20. |
| `Text (263,70,144,20) 0x444 white f7 bm 0x80000000` | TEXT POS 260 70 150 20 FLAGS TEXT_SHADOW\|TEXTFLAG_RIGHT | style RECT 3 0 3 0 → (263, 70, 144, 20); style flags 0x441 with TEXTFLAG_RIGHT → (0x441 & ~3) \| 4 = 0x444. The control-level TEXT_SHADOW is irrelevant; the blit is the style DRAWMODE, raw. |
| `Text (73,200,94,20) 0x441` "Control Name" (also Key 1 (303,200,54,20), Key 2 (458,200,54,20)) | TEXT POS 70 200 100 20, no FLAGS | RECT 3 0 3 0, style flags 0x441 unchanged |
| `Text (245,225,150,20)` / `(395,225,150,20)` 0x402 0xFFFF64 bm 0x20 | LISTBOX POS 60 220 512 170 RECT 185 5 23 5 ITEM 150 20, FIELD 0 0 150 20 … "key" | content x = 60+185, y = 220+5; 2 columns of 150; FIELD flags 0x402 replace the list's 0x4c2; colour 255,255,100; FIELD drawmode default = list DM_USEDEFAULT → 0 \| 0x20 |
| `Text (73,224,174,20) 0x41 white bm 0x80000000` "Invoke 1" (+20 per row) | TEXT POS 70 224 180 20 NAME "name0" FLAGS CTRLFLAG_CLEARBG FONT "Med" FLAGS TEXT_LEFT\|TEXT_VCENTER | RECT 3 0 3 0 → (73, 224, 174, 20); the style's FONT…FLAGS replaces the flags with 0x41 (no shadow); CLEARBG only adds the restore ParamBlit of (70,224,180,20); the text is set by the pane |
| `Text (180,410,100,30) 0x4c2 white bm 0x20` "OK" | BUTTON POS 180 410 100 30, NORMAL style | UPLABELRECT 0 0 0 0; up label flags CENTER\|VCENTER\|SINGLELINE\|SHADOW; the frame is ClearFrame2 tiled over (180,410,104,34) |
| HScrollBar tiles over (420,70,164,21) | SCROLLBAR POS 420 70 160 20 HSCROLL | h forced to 21−4 = 17; MARGINS +4 +4 → 164×21 |
| VScrollRect tiles over (60,220,516,174) border only | LISTBOX bg | NOCENTER, MARGINS 0 0 4 4; drawn despite CLEARBG |
| `Put(552,225)`, `Put(552,374)`, `Put(553,243)` | list child scrollbar (551,224,16,162) | UP 1 1; DOWN 1 RELB 12 → 224+162−12; THUMB 19 30 2 → x 551+2, y 224+19 + 113·row/61 |

The blit-mode split in practice:
- **TEXT widgets pass their style DRAWMODE raw** (0x80000000 = use the font's default).
- BUTTON labels, list cells and droplist pass `(dm==DEFAULT?0:dm)|0x20` = **0x20**.
- EDIT passes its raw style DRAWMODE (0x80000000 in widgets.def).

## 9. Other in-scope screens (derived from the rules; not run)

- **In-game menu.**
  - BUTTONs, e.g. POS 212 63 141 27: frame tiles over (212,63,145,31),
    `Text(212,63,141,27,"Load Game",0,7,0xFFFFFF,0x4c2,0x20)`.
  - FRAME POS 24 62 176 215: ClearFrame1 (14 14 16 16, border only) over (24,62,180,219).
  - BITMAP "picture": a native `Put(26,65,…)`.
- **Popup** (popup.def redefines STYLE TEXT with FLAGS TEXT_CENTER|TEXT_SHADOW = 0x402):
  - message `Text(35,60,300,72,msg,0,7,0xFFFFFF,0x402,0x80000000)`: centred, **top-aligned**, word-wrapped;
  - yes/no `Text(84|201,139,93,27,…,0x4c2,0x20)`;
  - progress FRAME tiles ClearFrame2 with insets 14 14 16 16 over (42,119,300,42).
- **Save/Load list** (POS 284 117/87 298 257/287, widgets.def VLIST RECT 5 5 23 5,
  ITEM 260 18, FIELD `2 0 272 18 FLAGS TEXT_LEFT|TEXT_SHADOW "gamelist_name"`):
  - cx = 289, cy = 122 (save); 1 column; itemw 260, itemh 18; 13 rows (save) / 15 rows (load);
  - rows are `Text(291, 122+18r, 258, 18, name, 0, 7, 0xFFFFFF, 0x401, 0x20)`: w' = min(2+272,260)−2;
    flags exactly 0x401, so **no VCENTER**;
  - selection is `Box(289, 122+18r, 260, 18, SELCOLOR 0,112,74)`;
  - scrollbar rect (561, 121, 16, 249): arrows (562,122) and (562,358), thumb x 563, track 140..340.
- **Save/Load TEXT**:
  - "gamename" (55,277,210,40) CLEARBG with style flags TEXT_CENTER|SHADOW|ELIPSES →
    `Text(58,277,204,40,…,0x2402,0x80000000)`;
  - "Module:"/"Character:" TEXTFLAG_RIGHT → 0x444;
  - "modname"/"charname" CLEARBG → 0x441.

---

## 10. The port (src/defpane.cpp, 2026-10-13)

`TDefPane` follows sections 1-8:
- the parse: base attributes in order, the first FLAGS is the control word, the
  style's FONT ... FLAGS replaces the text flags; the TEXT control-flag
  conversion; the button style by DRAWTOGGLE / DRAWRADIO; the list style
  (NOSCROLL gets the undefined third style); the scrollbar style VSCROLL unless
  HSCROLL; the EDIT's two RECTs; labels and label rects with retail's
  defaults and copies;
- the draw: the frame tiler with MARGINS and NOCENTER, faces tiled only with
  FRAME, label rects inset on all four sides, the text compose with the
  widget's own flags (`DrawTextComposed`), the scrollbar's size from its art
  and its parts at their parsed places (D art while held), the list as a grid
  of items with its scrollbar from the SCROLLBAR clause, FIELD text from the
  pane's getter (`FieldText`), the selection Box on the item's cell, the EDIT's
  caret blink (48 pulses, on for 24), hidden widgets skipped, DRAWTOBG first and
  the edited EDIT last, and the background restores (section 1.3);
- the input: the click goes to the first widget under the pointer; toggles
  change on the press; the list's scrollbar takes its clicks first, its track
  pages by the visible items, and a release on an item raises event 5002
  (OPTIONS_CONTROLS.md section 4).

Deviations:
- A thumb's place is `(value - min) / (max - min)` along the track; retail's
  is `value / (max - min)`. Every DEF scrollbar's range starts at 0.
- Narrow alpha tiles are drawn (section 1.4).
- The pane is composed whole on every change, where retail redraws single
  cells inside input calls; the one place that shows is the rebind capture's
  "-" (OPTIONS_CONTROLS.md section 3.5), which the port shows at the release
  rather than at the first key.
- Format BITMAP items and DROPLIST aren't implemented; no in-scope screen has
  either (a list logs a BITMAP item).

## 11. UNCONFIRMED

- Blit-mode semantics inside 0x4be2b0: taken from decomp lines 149-153, not from the disassembly.
- STYLE TEXT/BUTTON registrations parse from defaults (shown only for FRAME, 0x42b2c0), and
  whether widgets.def styles are re-read on every screen open.
- `<NOCENTER>` absent on a per-widget FRAME override: kept or cleared?
- LISTBOX third style (0x6559c0) contents.
- EDIT +0x90 = editing, and the period of the blink counter +0x9c.
- The derivation of the DROPLIST item rect (+0xa4..+0xb0).
- HLIST top-item code at 0x430b33 uses `and edi, ebx` (not `imul`). It looks like a retail bug
  and is irrelevant for in-scope screens.
- §9 values are derived from the rules; they were not run in the emulator.
